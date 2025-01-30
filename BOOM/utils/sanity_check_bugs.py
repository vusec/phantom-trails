#!/usr/bin/env python3
import subprocess as sp
import os
import argparse
import shutil
from datetime import datetime
import time

parser = argparse.ArgumentParser(description='Sanity checks for found bugs.')
parser.add_argument('-c', '--causes', dest="causes", type=str, required=True,
                    help='The causes directory to scan')
parser.add_argument('-f', '--filter', dest="filter", type=str, default="",
                    help='The causes directory to scan')

args = parser.parse_args()
causes = args.causes

to_check = []
for file in os.listdir(causes):
    if not os.path.isfile(os.path.join(causes, file)):
        continue
    if ".json" in file:
        continue
    if not args.filter in file:
        continue
    to_check.append(os.path.join(causes, file))

for bug in to_check:
    args = ["./run-FuzzConfig", bug, "/boom-wrapper/boot/bootrom.rv64.img", "0", "20", "/boom-wrapper/init/init"]
    print("Input:" + bug)
    sp.call(args)
