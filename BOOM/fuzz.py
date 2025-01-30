#!/usr/bin/env python3

import subprocess as sp
import os
import argparse
import glob
import shutil
from datetime import datetime
import time

import eval_utils
from eval_utils import *

parser = argparse.ArgumentParser(description="Runs several fuzzing campaigns.")
parser.add_argument(
    "-t",
    "--tag",
    dest="tag",
    default="unnamed",
    type=str,
    help="Tag to use for the output directory name.",
)
parser.add_argument(
    "-m", "--mode", dest="mode", type=str, help="The coverage modes to use."
)
parser.add_argument(
    "--config", default="FuzzConfig", dest="config", type=str, help="The BOOM config to fuzz."
)
parser.add_argument(
    "-r", "--runs", dest="runs", type=int, help="The number of eval runs to perform."
)
parser.add_argument(
    "-w", "--core-offset", dest="core_offset", type=int, default=0,
    help="Start allocating bound cores from this specified core (0-based)."
)
parser.add_argument(
    "-c", "--cores", dest="cores", type=int, required=True,
    help="Number of cores to use for jobs",
)
parser.add_argument("-s", "--sudo", action="store_true", default=False)
parser.add_argument(
    "-l",
    "--cleanup",
    action="store_true",
    default=False,
    help="Cleanup the queue folder after fuzzing to save space.",
)
parser.add_argument("-v", "--verbose", action="store_true", default=False)
parser.add_argument(
    "-f",
    "--fuzz",
    dest="fuzz_driver",
    type=str,
    required=True,
    choices=["libafl", "aflpp", "aflpp_notrim"],
)

args = parser.parse_args()
tag = args.tag
if "," in tag:
  print("Can't use commas in tag name, this breaks docker's mount args")
  sys.exit(1)
eval_utils.verbose_enabled = verbose = args.verbose
mode = args.mode
fuzz_driver = args.fuzz_driver
cleanup = args.cleanup

os.environ["DOCKER_BUILDKIT"] = "1"

docker_cmd = ["docker"]
if args.sudo:
    docker_cmd = ["sudo", "docker"]

assert_mode_str(args.mode)


def save_file_or_dir(to_save, out):
    verbose_info(f"Saving {to_save} to {out}")

    target_path = out + "/" + to_save.replace("/", "_")
    if os.path.isfile(to_save):
        shutil.copy(to_save, target_path)
    else:
        shutil.copytree(to_save, target_path)


def save_run_info(output_dir):
    output_dir += "/run-info/"
    os.makedirs(output_dir)
    save_list = ["Dockerfile", "Fuzzer", "ArchSim", "AFL/instrumentation"]
    for s in save_list:
        save_file_or_dir(s, out=output_dir)

    with open(output_dir + "/coverage-mode", "w") as f:
        f.write(args.mode)


fuzz_workers = FuzzWorkers(args.core_offset, args.cores)


def queue_run(tag_dir, run_index, config):
    """
    Launches the docker container for the fuzzer.
    """
    # Make a subdirectory for our current run.
    output_dir = os.path.join(tag_dir, str(run_index))
    if os.path.exists(output_dir):
        stop("Output directory already exists: " + tag)

    os.makedirs(output_dir)
    output_dir = os.path.realpath(output_dir)

    job_env = {}
    # start_impl.sh reads this env var to store the output.
    job_env["OUTPUT_DIR"] = output_dir

    # Create the folder where we should store causes to.
    os.makedirs(os.path.join(output_dir, "causes"))

    script_to_call = "utils/fuzz_impl_" + fuzz_driver + ".sh"
    fuzz_workers.queue_cmd(
        cmd=["./docker/start_impl.sh", script_to_call, config], env_vars=job_env
    )


def do_fuzz_runs(args):
    # Make a unique persistent directory for our fuzzer output.
    tag_dir = os.path.join("results", tag)

    if os.path.exists(tag_dir):
        warning("Output directory already exists, appending timestamp: " + tag_dir)
        tag_dir += "-ts-" + datetime.now().isoformat()

    container_name = "boom-fuzz.mode-"
    container_name += args.mode.replace(",", "-").lower()
    os.environ["CONTAINER_NAME"] = container_name

    os.environ["AFL_QUIET"] = "1"

    info("Building docker image " + container_name)
    build_image(args.mode, args.config)

    # Save data we might need later for debugging.
    save_run_info(tag_dir)

    info(f"Queuing {args.runs} jobs")
    info("Output directory: " + tag_dir)
    # Do the actual fuzzing runs.
    for i in range(0, args.runs):
        queue_run(tag_dir, i, args.config)

    fuzz_workers.run()


do_fuzz_runs(args)
