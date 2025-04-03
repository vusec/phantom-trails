#!/usr/bin/env python3
import subprocess as sp
import os
import argparse
import shutil
from datetime import datetime
import time

parser = argparse.ArgumentParser(description='Kills afl.')
parser.add_argument('-q', '--quiet', dest="quiet", default=False, action="store_true",
                    help="Don't print any output")
parser.add_argument('-e', '--expected', dest="expected_file", type=str,
                    help="The path to the file containing the expected bugs. This will cause the script to kill AFL once all bugs are found.")
parser.add_argument('-o', '--out', dest="out", type=str, required=True,
                    help='The causes directory to scan')

args = parser.parse_args()
print_info = not args.quiet

causes = os.path.join(args.out, "causes")
start_time = os.path.getmtime(os.path.join(args.out, "start_time_marker"))

expected_bugs = set()
if args.expected_file:
    with open(args.expected_file) as f:
        for raw_bug_name in f.read().split("\n"):
            bug = raw_bug_name.strip()
            if len(bug) == 0:
                continue
            expected_bugs.add(bug)

    if print_info:
        print("Expected to find: " + " ".join(expected_bugs))

def get_found_bugs() -> set:
    result = set()
    for file in os.listdir(causes):
        if not os.path.isfile(os.path.join(causes, file)):
            continue
        result.add(file.split("%")[0].replace("_", " "))
    return result

def get_found_bugs_with_time() -> dict:
    result = {}
    for file in os.listdir(causes):
        full_path = os.path.join(causes, file)
        if not os.path.isfile(full_path):
            continue
        t = os.path.getmtime(full_path)
        bug_name = file.split("%")[0]
        # if we already saw this bug and the TTE is smaller, then overwrite
        # the TTE time. We just want the minimum time for the final result.
        if bug_name in result.keys():
            if t < result[bug_name]:
                result[bug_name] = t
        else:
            result[bug_name] = t
    return result


if args.expected_file:
    while True:
        time.sleep(2)
        found = get_found_bugs()
        if found.issuperset(expected_bugs):
            if print_info:
                print("Found all bugs")
            break
        else:
            if print_info:
                print("Found bugs: " + "|".join(list(found)))
                print("Missing bugs: " + "|".join(list(expected_bugs.difference(found))))


with open(os.path.join(args.out, "bug_timings"), "w") as f:
    bugs_and_times = get_found_bugs_with_time()
    for bug in bugs_and_times.keys():
        bug_time = bugs_and_times[bug]
        print("Start time: " + str(bug_time))
        f.write(str(bug_time - start_time))
        f.write("|" + bug + "\n")


# Stop AFL++
if args.expected_file:
    if print_info:
        print("Killing all afl instances.")
    time.sleep(3)
    sp.call(["killall", "-s", "SIGKILL", "afl-fuzz"])
