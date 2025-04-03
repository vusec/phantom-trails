#!/usr/bin/python3

import os
import sys
import argparse
from datetime import datetime
import subprocess as sp
import shutil
import time
import signal
import atexit

# ------ Logging
class Colors:
    """
    Colors for console output.
    """
    HEADER = "\033[95m"
    OKBLUE = "\033[94m"
    OKCYAN = "\033[96m"
    OKGREEN = "\033[92m"
    WARNING = "\033[93m"
    FAIL = "\033[91m"
    ENDC = "\033[0m"
    BOLD = "\033[1m"
    FAINT = "\033[2m"
    UNDERLINE = "\033[4m"


GOOD = "[" + Colors.OKGREEN + "OK" + Colors.ENDC + "]"
FAIL = "[" + Colors.FAIL + "FAIL" + Colors.ENDC + "]"

verbose_enabled = False


def verbose_info(reason):
    if verbose_enabled:
        print("(" + Colors.FAINT + Colors.OKBLUE + reason + Colors.ENDC + ")")


def timestr() -> str:
    return datetime.now().strftime("%m/%d/%Y, %H:%M:%S")


def info(reason):
    print("\r• " + reason + Colors.ENDC)


def warning(reason):
    print(Colors.BOLD + Colors.WARNING + "\rwarning: " + Colors.ENDC + reason)


def fatal(reason):
    print(Colors.FAIL + "\rfatal: " + Colors.ENDC + reason)


def save_file_or_dir(to_save, out):
    verbose_info(f"Saving {to_save} to {out}")

    target_path = out + "/" + to_save.replace("/", "_")
    if os.path.isfile(to_save):
        shutil.copy(to_save, target_path)
    else:
        shutil.copytree(to_save, target_path)


def save_run_info(output_dir, coverage):
    output_dir += "/run-info/"
    os.makedirs(output_dir, exist_ok=True)
    save_list = ["Dockerfile", "Fuzzer", "ArchSim", "AFL/instrumentation"]
    for s in save_list:
        save_file_or_dir(s, out=output_dir)

    with open(output_dir + "/coverage-mode", "w") as f:
        f.write(coverage)


# ------ Actions
subproc = None


def signal_handler(signum, frame):
    global subproc
    if subproc:
        while subproc.poll() is None:
            fatal("Trying to kill all subprocesses...")
            sp.call(["killall", "-s", "SIGKILL", "sim-fuzzer"])
            sp.call(["killall", "-s", "SIGKILL", "run-FuzzConfig"])
            sp.call(["killall", "-s", "SIGKILL", "run-SmallFuzzConfig"])
            sp.call(["killall", "-s", "SIGKILL", "run-MDSConfig"])
            time.sleep(3)

    sys.exit(0)


class CmdHandler(object):
    def __init__(self, args):
        self.parse_args(args)

        if not args.no_rebuild:
            self.build_container()

        if args.subparser_name == 'fuzz':
            self.start_fuzzer(args.quiet)
        elif args.subparser_name == 'shell':
            self.start_container()
        elif args.subparser_name == 'run':
            self.start_container(args.cmd)
        elif args.subparser_name == 'test':
            self.start_container('/scripts/sanity-check-bugs.py')

    def parse_args(self, args):
        # Select coverage mode.
        self.coverage_string = args.coverage
        self.coverage = "SplitCriticalEdges,BasicBlock" if args.coverage == "SW" else args.coverage
        # Set config.
        self.boom_config = args.config

        # Container name.
        container_name = args.name
        if not args.name:
            container_name = "boom-fuzz."
            container_name += self.boom_config
            container_name += "." + self.coverage_string
        self.container_name = container_name.lower()

        # Are we fuzzing son?
        self.fuzz = True if args.subparser_name == 'fuzz' else False

        # Output folder.
        out_dir = args.out
        if not args.out:
            top_dir = os.path.join(
                os.getcwd(), "results", "fuzzing" if self.fuzz else "manually_started")
            out_dir = os.path.join(
                top_dir, f"{self.boom_config}-{self.coverage_string}")
        if os.path.exists(out_dir):
            warning(
                "Output directory already exists, appending timestamp: " + out_dir)
            out_dir += "-ts-" + datetime.now().isoformat()
        os.makedirs(out_dir, exist_ok=True)
        self.out_dir = out_dir

        # Jobs
        self.jobs = args.jobs
        if not args.jobs:
            self.jobs = os.cpu_count()

        # Verbose?
        self.debug = args.debug

    def build_container(self):
        """
        docker build.
        """
        os.environ["CONTAINER_NAME"] = self.container_name

        cmd = ["./docker/build_impl.sh"]
        cmd += ["--build-arg", f"COVERAGE={self.coverage}"]
        boom_build_flags = f'--config={self.boom_config}'
        if self.debug:
            boom_build_flags += ' --debug '
        cmd += ["--build-arg", f"BOOM_BUILD_FLAGS='{boom_build_flags}'"]
        cmd += ["--build-arg", f"NPROCS={self.jobs}"]

        info("Building docker image " + self.container_name)
        info(f"Feedback: {self.coverage_string}")
        info(f"BOOM config: {self.boom_config}")
        sp.check_call(cmd, env=os.environ.copy())

    def start_container(self, cmd):
        """
        docker run <CMD>.
        """
        os.environ["CONTAINER_NAME"] = self.container_name
        os.environ["OUTPUT_DIR"] = self.out_dir

        info("Output folder " + self.out_dir)
        sp.check_call(
            f"./docker/start_impl.sh {cmd}", shell=True, env=os.environ.copy())

    def start_fuzzer(self, quiet):
        """
        Start 'phantom-trails fuzz' inside of the container.
        """
        os.environ["AFL_QUIET"] = "1"
        os.environ["CONTAINER_NAME"] = self.container_name
        os.environ["OUTPUT_DIR"] = self.out_dir
        os.makedirs(os.path.join(self.out_dir, "causes"), exist_ok=True)

        save_run_info(self.out_dir, self.coverage)

        # TODO: doesn't seem to be able to kill everything?
        signal.signal(signal.SIGINT, signal_handler)
        signal.signal(signal.SIGTERM, signal_handler)

        info(f"Spawning fuzz job at " + timestr())
        info("Output folder " + self.out_dir)
        full_cmd = f"./docker/start_impl.sh phantom-trails fuzz --config={self.boom_config}"

        global subproc
        if quiet:
            # Run in background, just regularly check if it's done.
            subproc = sp.Popen(full_cmd, shell=True, env=os.environ.copy(),
                               stdout=sp.DEVNULL, stderr=sp.DEVNULL)

            while subproc.poll() is None:
                time.sleep(10)
        else:
            # Show the actual stdout output (TUI).
            sp.call(full_cmd, env=os.environ.copy(), shell=True)

        info("Finished fuzzing at " + timestr())
        #  Print results.
        info("===== TTE stats =====")
        sp.check_call(
            ["python3", "scripts/eval-results-folder.py", "-z", self.out_dir])
        info("===== ITE stats =====")
        sp.check_call(
            ["python3", "scripts/eval-results-folder.py", "-t", self.out_dir])


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Helper script for building and running PhantomTrails on BOOM.")
    subparsers = parser.add_subparsers(dest="subparser_name")

    # Common flags.
    parent_parser = argparse.ArgumentParser(add_help=False)
    parent_parser.add_argument("--coverage", dest="coverage", type=str,
                               choices=['SW', 'Taint'],
                               default="SW",
                               help="The coverage mode to use.")
    parent_parser.add_argument("--config", dest="config", type=str,
                               choices=['FuzzConfig',
                                        'MDSConfig', 'SmallFuzzConfig'],
                               default="FuzzConfig",
                               help="The BOOM config to build.")
    parent_parser.add_argument("-n", "--name", dest="name", type=str,
                               help="Name of the container.")
    parent_parser.add_argument("-o", "--out", dest="out", type=str,
                               help="Path of the output folder.")
    parent_parser.add_argument("-j", "--jobs", dest="jobs", type=int,
                               help="The number of build jobs.")
    parent_parser.add_argument("--debug", dest="debug", action="store_true",
                               help="Build a debug chip.")
    parent_parser.add_argument("--no-rebuild", dest="no_rebuild", action="store_true",
                               help="Use existing container.")

    # Commands
    subparsers.add_parser("shell", parents=[parent_parser],
                          help="Build container and start a shell.")

    run_cmd = subparsers.add_parser("run", parents=[parent_parser],
                                    help="Run a specific command in the container.")
    run_cmd.add_argument("cmd", type=str,
                         help="Command to run in the container.")

    subparsers.add_parser("test", parents=[parent_parser],
                          help="Run the detector's PoC testsuite.")

    fuzz_cmd = subparsers.add_parser("fuzz", parents=[parent_parser],
                                     help="Run a fuzzing campaign.")
    fuzz_cmd.add_argument("--quiet", dest="quiet", action="store_true",
                          help="Don't show the fuzzer's TUI.")

    args = parser.parse_args()

    if args.subparser_name is None:
        parser.print_help()
        sys.exit(0)

    CmdHandler(args)
