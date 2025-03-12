#!/usr/bin/python3

import os
import sys
import argparse
from datetime import datetime
import subprocess as sp
import shutil
import time
import signal, atexit

# ------ Utils

# Colors for console output.
class Colors:
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


def stop(reason):
    print(Colors.FAIL + "\rerror: " + Colors.ENDC + reason)
    sys.exit(1)


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

cur_pid = None

def signal_handler(signum, frame):
    if cur_pid:
        print("Quitting...")


class BoomFuzzer(object):
    def parse_args(self, args):
        # Check if coverage mode is allowed.
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
        if args.fuzz:
            self.fuzz = True
        else:
            self.fuzz = False

        # Output folder.
        out_dir = args.out
        if not args.out:
            top_dir = os.path.join(os.getcwd(), "results", "fuzzing" if self.fuzz else "manually_started")
            out_dir = os.path.join(top_dir, f"{self.boom_config}-{self.coverage_string}")
        if os.path.exists(out_dir):
            warning("Output directory already exists, appending timestamp: " + out_dir)
            out_dir += "-ts-" + datetime.now().isoformat()
        os.makedirs(out_dir, exist_ok=True)
        self.out_dir = out_dir

        # Jobs
        self.jobs = args.jobs
        if not args.jobs:
            self.jobs = os.cpu_count()

        # Verbose?
        self.debug = args.debug
        self.no_rebuild = args.no_rebuild

    def __init__(self, args):
        self.parse_args(args)

        if args.fuzz:
            self.start_fuzzer()
        elif args.run:
            self.start_container(args.run)
        elif args.start:
            self.start_container("")
        else:
            self.build_container()

    def build_container(self):
        os.environ["CONTAINER_NAME"] = self.container_name

        build_args = ["./docker/build_impl.sh"]
        build_args += ["--build-arg", f"COVERAGE={self.coverage}"]
        boom_build_flags = f'--config={self.boom_config}'
        if self.debug:
            boom_build_flags += ' --debug '
        build_args += ["--build-arg", f"BOOM_BUILD_FLAGS='{boom_build_flags}'"]
        build_args += ["--build-arg", f"NPROCS={self.jobs}"]

        info("Building docker image " + self.container_name)
        info(f"Feedback: {self.coverage_string}")
        info(f"BOOM config: {self.boom_config}")
        sp.check_call(build_args)

    def start_container(self, cmd):
        os.environ["CONTAINER_NAME"] = self.container_name
        os.environ["OUTPUT_DIR"] = self.out_dir

        if not self.no_rebuild:
            self.build_container()

        info("Output folder " + self.out_dir)
        if cmd:
            sp.check_call(["./docker/start_impl.sh", cmd])
        else:
            sp.check_call(["./docker/start_impl.sh"])

    def start_fuzzer(self):
        os.environ["AFL_QUIET"] = "1"
        os.environ["CONTAINER_NAME"] = self.container_name
        os.environ["OUTPUT_DIR"] = self.out_dir
        os.makedirs(os.path.join(self.out_dir, "causes"), exist_ok=True)

        if not self.no_rebuild:
            self.build_container()
        save_run_info(self.out_dir, self.coverage)

        # TODO: doesn't seem to be able to kill everything?
        # signal.signal(signal.SIGINT, signal_handler)
        # signal.signal(signal.SIGTERM, signal_handler)

        info(f"Spawning fuzz job at " + timestr())
        info("Output folder " + self.out_dir)
        full_cmd = ["./docker/start_impl.sh", "/utils/fuzz_impl_libafl.sh", self.boom_config]
        spawned = sp.Popen(full_cmd, stdout=sp.DEVNULL, stderr=sp.DEVNULL)
        cur_pid = spawned.pid
        while spawned.poll() is None:
            time.sleep(10)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(
        description="Helper script for evaluating PhantomTrails on BOOM.")
    parser.add_argument(
        "--coverage", choices=['SW', 'Taint'], dest="coverage", type=str, default="SW", help="The coverage mode to use.")
    parser.add_argument(
        '--config', choices=['FuzzConfig', 'MDSConfig', 'SmallFuzzConfig'], default="FuzzConfig", help="The BOOM config to build.")
    parser.add_argument(
        "-n", "--name", dest="name", type=str, help="Name of the container.")
    parser.add_argument(
        "-o", "--out", dest="out", type=str, help="Path of the output folder.")
    parser.add_argument(
        "-j", "--jobs", dest="jobs", type=int, help="The number of build jobs.")
    parser.add_argument(
        "--debug", dest="debug", default=False, action="store_true", help="Build a debuggable chip.")
    parser.add_argument(
        "--no-rebuild", dest="no_rebuild", default=False, action="store_true", help="Use existing container.")

    group = parser.add_mutually_exclusive_group()
    group.add_argument('--start', dest="start", action="store_true", help="Start a shell in the container.")
    group.add_argument('--run', dest="run", type=str, help="Run a script in the container.")
    group.add_argument('--fuzz', dest="fuzz", action="store_true", help="Run a fuzzing campaign in the container.")
    args = parser.parse_args()

    BoomFuzzer(args)
