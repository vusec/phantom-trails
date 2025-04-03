#!/usr/bin/env python3

import os
import subprocess as sp
import sys

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

def print_color(c, content):
    print(c + content + Colors.ENDC)

GOOD = "[" + Colors.OKGREEN + "OK" + Colors.ENDC + "]"
FAIL = "[" + Colors.FAIL + "FAIL" + Colors.ENDC + "]"

expectations = {
    'meltdown-us.bin': 'Meltdown_LOAD_PFAULT',
    'spectreRSB-static.bin': 'Spectre_RSB',
    'spectreRSB.bin': 'Spectre_RSB',
    'spectrev1.bin': 'Spectre_v1_static',
    'spectrev1_training.bin': 'Spectre_v1_static',
    'spectrev2.bin': 'Spectre_v2',
    'spectrev4-single-load.bin': 'Spectre_v4',
    'spectrev4-stl.bin': 'Spectre_v4',
}

TESTS_FOLDER = '/Samples/build/bins/pocs'
ok = True

for f in expectations.keys():
    print(f"• Testing {f}... ")
    result = sp.run(["phantom-trails", "run", TESTS_FOLDER + '/' + f], capture_output=True, text=True)
    bug = None
    for l in result.stderr.split('\n'):
        if l.strip().startswith('Found issue:'):
            bug = l.split(':')[1].strip()
            break

    if bug == expectations[f]:
        print(GOOD + ' ' + bug)
    else:
        print(FAIL + ' ' + str(bug))
        ok = False

if not ok:
    print_color(Colors.FAIL, 'Some tests failed!')
    sys.exit(-1)
else:
    print_color(Colors.OKGREEN, 'All tests passed.')
