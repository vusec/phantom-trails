#!/usr/bin/env python3

import sys
import subprocess as sp
import os
import time
from datetime import datetime

script_dir = os.path.dirname(os.path.realpath(__file__))


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


def get_coverage_modes():
    modes = []
    mode_path = script_dir + "/../AFL/instrumentation/HWFuzzing/CoverageModes.def"
    with open(mode_path) as mode_file:
        for line in mode_file.readlines():
            # Get rid of whitespace
            line = line.strip()
            line = line.replace(" ", "")
            # Comments that can be ignored.
            if line.startswith("//"):
                continue
            # Delete the macro.
            line = line.replace("COVERAGE_MODE(", "")
            line = line.replace(")", "")
            modes.append(line)
    return modes


def assert_mode_str(mode_str):
    known_modes = get_coverage_modes()
    for mode in mode_str[:].split(","):
        if not mode in known_modes:
            err_msg = "Unknown coverage mode: " + mode + "\n"
            err_msg += "\n * "
            err_msg += known_modes.join("\n * ")
            stop(err_msg)


def build_image(mode, config):
    assert_mode_str(mode)

    build_args = ["./docker/build_impl.sh", "--build-arg"]
    build_args += ["COVERAGE=" + mode]
    build_args += ["--build-arg"]
    build_args += ["BOOM_BUILD_FLAGS=--config=" + config]
    sp.check_call(build_args)


class FuzzJob:
    def __init__(self, cmd: [str], env_vars: [str]):
        self.cmd = cmd
        self.env_vars = env_vars


class RunningJob:
    def __init__(self, popen: sp.Popen, bound_core: int):
        self.process = popen
        self.bound_core = bound_core


class FuzzWorkers:
    def __init__(self, core_offset, cores):
        self.children = []
        self.max_workers = cores
        self.free_cores = set(range(core_offset, core_offset + self.max_workers))
        info(f"Running {self.max_workers} parallel jobs.")
        self.queue = []
        self.check_interval_secs = 10

    def get_next_free_core(self):
        if len(self.free_cores) == 0:
            stop("Couldn't find a free core")

        result = list(self.free_cores)[0]
        self.free_cores.remove(result)
        return result

    def queue_cmd(self, cmd: [str], env_vars: [str]):
        self.queue.append(FuzzJob(cmd, env_vars))

    def _start_another_run(self):
        if len(self.queue) == 0:
            return False
        # Pop the next job.
        to_run = self.queue[0]
        self.queue = self.queue[1:]

        full_cmd = to_run.cmd[:]
        core_to_use = self.get_next_free_core()
        full_cmd.append(str(core_to_use))

        # Create the full env var list for the job.
        job_env = os.environ.copy()
        for var in to_run.env_vars:
            job_env[var] = to_run.env_vars[var]

        info(f"Spawning fuzz job on core {core_to_use} at " + timestr())
        spawned = sp.Popen(full_cmd, env=job_env, stdout=sp.DEVNULL, stderr=sp.DEVNULL)

        self.children.append(RunningJob(spawned, bound_core=core_to_use))
        return True

    def run(self):
        while len(self.queue) != 0 or len(self.children) != 0:
            new_children = []
            for child in self.children:
                poll_result = child.process.poll()
                if poll_result is None:
                    new_children.append(child)
                    continue
                self.free_cores.add(child.bound_core)
            self.children = new_children

            while len(self.children) < self.max_workers:
                if not self._start_another_run():
                    break
                time.sleep(2)

            time.sleep(self.check_interval_secs)
