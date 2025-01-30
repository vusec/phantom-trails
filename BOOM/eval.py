#!/usr/bin/env python3

import subprocess as sp
import os
import argparse
from datetime import datetime
import time

from eval_utils import *

fuzz_py = os.path.join(script_dir, "fuzz.py")

parser = argparse.ArgumentParser(description="Runs the full evaluation.")
parser.add_argument(
    "-r",
    "--runs",
    dest="runs",
    default=None,
    type=int,
    help="The number of eval runs to perform.",
)
parser.add_argument("-s", "--sudo", action="store_true")
parser.add_argument("-v", "--verbose", action="store_true")
parser.add_argument(
    "-c",
    "--cleanup",
    action="store_true",
    default=False,
    help="Cleanup the queue folder after fuzzing to save space.",
)
parser.add_argument(
    "-f",
    "--fuzz",
    dest="fuzz_driver",
    type=str,
    default="libafl",
    choices=["libafl", "aflpp", "aflpp_notrim"],
)
parser.add_argument(
    "-n", "--cores", dest="cores", type=int, required=True,
    help="Number of cores to use for jobs",
)
parser.add_argument(
    "--config", default="FuzzConfig", dest="config", type=str, help="The BOOM config to fuzz."
)
args = parser.parse_args()

fuzz_args = []
if args.sudo:
    fuzz_args += ["--sudo"]
if args.verbose:
    fuzz_args += ["--verbose"]
if args.cleanup:
    fuzz_args += ["--cleanup"]
fuzz_args += ["--fuzz", args.fuzz_driver]
fuzz_args += ["--cores", str(args.cores)]
fuzz_args += ["--runs", str(args.runs)]
fuzz_args += ["--config", str(args.config)]

modes_to_eval = [
#    "Taint",
#    "Toggle,SplitCriticalEdges,BasicBlock",
    "SplitCriticalEdges,BasicBlock",
# Alternatives, didn't turn out to be interesting.
#    "Toggle",
#    "Edge",
#    "Toggle,Edge",
]
for mode in modes_to_eval:
    assert_mode_str(mode)

for mode in modes_to_eval:
    info("Evaluating mode: " + Colors.HEADER + mode.replace(",", "+"))

    tag_name = "eval-" + mode.replace(",", "-")

    final_cmd = [fuzz_py] + fuzz_args[:]
    final_cmd += ["--mode", mode]
    final_cmd += ["--tag", tag_name]
    sp.check_call(final_cmd)

    # Wait a few seconds so the user can SIGINT the process...
    time.sleep(10)
