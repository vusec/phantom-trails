#!/usr/bin/env python3

import subprocess as sp
import sys
import os
import argparse
import shutil
import os.path as path
from datetime import datetime
import statistics as stats

parser = argparse.ArgumentParser(description="Evaluate fuzzing results folder.")
parser.add_argument("-v", "--verbose", action="store_true")
parser.add_argument("-p", "--plot", action="store_true")
parser.add_argument("-s", "--stats", action="store_true")
parser.add_argument("-z", "--stats_single", action="store_true", default=None)
parser.add_argument("-t", "--iters_single", action="store_true", default=None)
parser.add_argument(
    "-m", "--merge", action="store_true", help="Merge static/dynamic bug types."
)
parser.add_argument("-l", "--list", action="store_true", help="List all measurements.")
parser.add_argument(
    "-f", "--format", action="store", choices=["png", "svg"], default="png", help="Output format"
)
parser.add_argument("folders", nargs="+")

args = parser.parse_args()
verbose = args.verbose


class IterLogItem:
    def __init__(self, time, iterations, execs_per_sec):
        self.time = time
        self.iters = iterations
        self.execs_per_sec = execs_per_sec
        assert execs_per_sec > 0

# Represents a single run/fuzzing campaign that stopped once all expected
# bugs where found.
class Run:
    def _time_to_iterations(self, time):
        last_iters = 0
        delta_secs = time
        for i in self.time_to_iterations_log:
            last_execs = i.execs_per_sec
            if i.time > time:
                break
            last_iters = i.iters
            delta_secs = time - i.time

        assert delta_secs >= 0
        return int(last_iters + delta_secs * last_execs)

    def _get_total_iter_count(self, folder):
        counter_folder = os.path.join(folder, "counters")
        lines = []
        for c in os.listdir(counter_folder):
            if not c.startswith("inputs_"):
                continue
            with open(os.path.join(counter_folder, c), "r") as f:
                for line in f.readlines():
                    lines.append(line)
        return len(set(lines))

    def _init_aflpp(self, folder):
        max_secs = 1
        with open(path.join(folder, "bug_timings")) as timings:
            for line in timings.readlines():
                parts = line.split("|")
                secs = int(float(parts[0]))
                bug = parts[1].strip()
                self.tte_seconds[bug] = secs
                max_secs = max(secs, max_secs)
        if len(self.tte_seconds.keys()) == 0:
            print("Failed to find any bugs in " + str(folder))
            sys.exit(1)
        execs_per_sec = self._get_total_iter_count(folder) / max_secs
        for bug in self.tte_seconds.keys():
            self.tte_iterations[bug] = self.tte_seconds[bug] * execs_per_sec

    def _init_libafl(self, folder):
        with open(path.join(folder, "iterations_time")) as f:
            last_time = 0
            last_iterations = 0
            for line in f.readlines():
                parts = line.split(" ")
                time = int(parts[0])
                iterations = int(parts[1])
                # Unused:
                # coverage = int(parts[2])

                delta_time = time - last_time
                delta_iters = iterations - last_iterations

                if delta_time == 0:
                    execs_per_sec = 1
                else:
                    execs_per_sec = delta_iters / delta_time

                self.time_to_iterations_log.append(
                    IterLogItem(time, iterations, execs_per_sec)
                )

                last_time = time
                last_iterations = iterations

        with open(path.join(folder, "found_all")) as f:
            for line in f.readlines():
                parts = line.split("$")
                time = int(parts[0])
                kind = parts[1].strip()
                if args.merge:
                    kind = kind.replace(" static", "").replace(" dynamic", "")
                if not kind in self.tte_seconds.keys():
                    self.tte_seconds[kind] = time
                    self.tte_iterations[kind] = self._time_to_iterations(time)

    def __init__(self, folder):
        self.tte_seconds = {}
        self.tte_iterations = {}
        self.time_to_iterations_log = []
        self.folder = folder

        if os.path.exists(path.join(folder, "found_all")):
            self._init_libafl(folder)
        else:
            self._init_aflpp(folder)


    def get_causes(self):
        return self.tte_seconds.keys()

class UnitSecs:
    def __init__(self):
        self.name = "time"
        pass

    def format(self, seconds : int) -> str:
        result = ""
        h = seconds // 3600
        if h != 0:
            result += str(h) + "h "

        m = seconds % 3600 // 60
        if h != 0 or m != 0:
            result += str(m) + "m "

        remaining_secs = seconds % 3600 % 60
        result += str(remaining_secs) + "s "
        return result.strip()

class UnitIters:
    def __init__(self):
        self.name = "iterations"
        pass

    def format(self, iterations : int) -> str:
        # return str(iterations // 1000) + "k"
        return str(iterations)

class Stats:
    def __init__(self, unit):
        self.unit = unit
        self.data = []

    def append(self, raw_data : int):
        self.data.append(raw_data)

    def print(self):
        print(" Number Measurements: " + str(len(self.data)))
        print(f" In {self.unit.name}:")
        print("   Geomean: " + self.unit.format(stats.geometric_mean(self.data)))
        print("      Mean: " + self.unit.format(stats.mean(self.data)))
        print("   std dev: " + self.unit.format(stats.stdev(self.data)))
        print("       Min: " + self.unit.format(min(self.data)))
        print("       Max: " + self.unit.format(max(self.data)))

    def print_single(self, id):
        return self.unit.format(self.data[id])


# A series of fuzzing runs/campaigns using the same feedback mechanism.
class RunList:
    def __init__(self, name):
        self.runs = []
        self.name = name

    # Scans all
    def scan(self, folder):
        # Scan all subfolders with numbers as name.
        # In case we deleted a subfolder because the fuzzer crashes etc.,
        # we continue to scan the rest.
        for i in range(0, 1000):
            run_path = folder + "/" + str(i)
            if not os.path.exists(run_path):
                continue
            if not os.path.exists(path.join(run_path, "found_all")) and not os.path.exists(path.join(run_path, "bug_timings")):
                continue
            self.runs.append(Run(run_path))

    def scan_one(self, folder):
        run_path = folder + "/"
        if not os.path.exists(run_path):
            return
        if not os.path.exists(path.join(run_path, "found_all")) and not os.path.exists(path.join(run_path, "bug_timings")):
            return
        self.runs.append(Run(run_path))

    def get_iters_for(self, bug) -> Stats:
        result = Stats(UnitIters())
        for run in self.runs:
            if bug not in run.tte_iterations:
                result.append(0)
            else:
                result.append(run.tte_iterations[bug])
        return result

    def get_secs_for(self, bug) -> Stats:
        result = Stats(UnitSecs())
        for run in self.runs:
            if bug not in run.tte_seconds:
                result.append(0)
            else:
                result.append(run.tte_seconds[bug])
        return result

    def get_simple(self, bug) -> Stats:
        result = Stats(UnitSecs())
        for run in self.runs:
            if bug not in run.tte_seconds:
                result.append(0)
            else:
                result.append(run.tte_seconds[bug])
        return result

    def common_bugs(self):
        bugs = []
        f = open('expected_findings')
        for l in f:
            bugs.append(l.replace('\n', ''))
        f.close()
        return bugs

        # common_causes = set(self.runs[0].get_causes())
        # for run in self.runs:
        #     # print(run.folder + " " + str(common_causes))
        #     common_causes = common_causes.intersection(set(run.get_causes()))
        # return common_causes

    def print_stats(self):
        bugs = list(self.common_bugs())
        bugs.sort()
        for bug in bugs:

            print("\n\n" + bug)
            data = self.get_iters_for(bug)
            data.print()

            data = self.get_secs_for(bug)
            data.print()

    def print_stats_single(self, id):
        bugs = list(self.common_bugs())
        bugs.sort()
        for bug in bugs:
            data = self.get_secs_for(bug)
            print(f"{bug:<35} TTE: {data.print_single(id)}")

    def print_iters_single(self, id):
        bugs = list(self.common_bugs())
        bugs.sort()
        for bug in bugs:
            data = self.get_iters_for(bug)
            print(f"{bug:<35} Iters: {data.print_single(id)}")

class Analyzer:
    def __init__(self):
        self.stats_by_mode = {}

    def getResultObjForMode(self, mode):
        if mode in self.stats_by_mode.keys():
            return self.stats_by_mode[mode]
        self.stats_by_mode[mode] = RunList(mode)
        return self.getResultObjForMode(mode)

    def getModeForFolder(self, path):
        with open(os.path.join(path, "run-info", "coverage-mode"), "r") as f:
            return f.read().strip()

    def common_bugs(self):
        common_causes = set(list(self.stats_by_mode.values())[0].common_bugs())
        for mode in self.stats_by_mode.values():
            common_causes = common_causes.intersection(set(mode.common_bugs()))
        filtered = []
        for bug in common_causes:
            # Skip the "OOO" bugs because we don't seem to care about them
            # in the eval.
            if not "OOO" in bug:
                filtered.append(bug)
        filtered.sort()
        return filtered


    def process_folders(self, folders):
        for folder in folders:
            if os.path.isfile(folder):
                print(f"Warning: Skipping input directory {folder} because it's a file")
                continue
            mode = self.getModeForFolder(folder) # type: str
            stats = self.getResultObjForMode(mode) # type: RunList
            stats.scan(folder)

    def process_single(self, folder):
            if os.path.isfile(folder):
                print(f"Error: expected input directory {folder}, got a file")
                return
            stats = RunList("manual")
            stats.scan_one(folder)
            self.stats_by_mode["manual"] = stats

    def getCleanName(self, bug_name):
        names = {
            "SplitCriticalEdges,BasicBlock" : "Only BB",
            "Toggle,SplitCriticalEdges,BasicBlock" : "BB + Toggle",
            "Toggle,Edge" : "Toggle+Edge",
            "Toggle" : "Only Toggle"
        }
        if bug_name in names:
            return names[bug_name]
        return bug_name

    def getSortedModeNames(self):
        keys = list(self.stats_by_mode.keys())
        keys.sort()
        if "Taint" in keys:
            keys.remove("Taint")
            keys.append("Taint")

        return keys

    def stats(self):
        for key in self.stats_by_mode.keys():
            self.stats_by_mode[key].print_stats()

    def stats_single(self, id):
        for key in self.stats_by_mode.keys():
            self.stats_by_mode[key].print_stats_single(id)

    def iters_single(self, id):
        for key in self.stats_by_mode.keys():
            self.stats_by_mode[key].print_iters_single(id)

    def plot(self):
        import matplotlib.pyplot as plt
        import numpy as np

        font = {'weight' : 'normal',
                'size'   : 7}

        plt.rc('font', **font)

        units = [["secs", "TTE"],
                 ["iters", "Iterations to exposure"]]

        colors = {"Taint": "#55d147",
                  "Toggle,SplitCriticalEdges,BasicBlock": "#d85beb",
                  "Toggle": "#5b5eeb",
                  "SplitCriticalEdges,BasicBlock": "#eb5b60",
                  "Edge": "#235b60",
                  "Toggle,Edge": "#555b60",}

        for unit in units:
            unit_abbrev = unit[0]
            unit_label = unit[1]

            all_bugs = list(self.common_bugs())
            print("Found bugs: " + " ".join(all_bugs))
            modes_to_skip = ["Edge", "Toggle,Edge"]

            fig, axes = plt.subplots(nrows=1, ncols=len(all_bugs),
                                     figsize=(17, 5),
                                     sharey=False)

            for i in range(0, len(all_bugs)):
                bug = all_bugs[i]
                ax2 = axes[i]
                raw_mode_names = []
                mode_names = []
                data = []
                for mode_key in self.getSortedModeNames():
                    mode = self.stats_by_mode[mode_key]
                    if mode.name in modes_to_skip:
                        continue
                    raw_mode_names.append(mode.name)
                    mode_names.append(self.getCleanName(mode.name))
                    if unit_abbrev == "secs":
                        data.append(mode.get_secs_for(bug).data)
                    elif unit_abbrev == "iters":
                        data.append(mode.get_iters_for(bug).data)
                    else:
                        print(f"Unknown unit: {unit_abbrev}")
                        sys.exit(1)


                ax2.set_title(unit_label + ' for ' + bug)
                ax2.grid(axis='y')
                parts = ax2.violinplot(
                        data, showmeans=True, showmedians=False,
                        showextrema=True)


                part_index = 0
                for pc in parts['bodies']:
                    mode_key = raw_mode_names[part_index]
                    pc.set_facecolor(colors[mode_key])
                    part_index += 1

                labels = mode_names

                if True:
                    scatter_x = []
                    scatter_y = []
                    x = 1
                    for row in data:
                        for col in row:
                            scatter_y.append(col)
                            scatter_x.append(x)
                        x += 1

                    ax2.scatter(scatter_x, scatter_y, marker=0, color='black', s=20, zorder=3)

                # set style for the axes
                ax2.set_xticks(np.arange(1, len(labels) + 1), labels=labels)
                ax2.set_xlabel('Mode')

            plt.subplots_adjust(top=0.92, bottom=0.07, left=0.1, right=0.97, hspace=0.24,
                        wspace=0.23)
            format = args.format
            plt.savefig(unit_abbrev + "." + format, dpi=300, format=format, orientation='portrait')
            plt.close()

# Single run
if args.stats_single or args.iters_single:
  analyzer = Analyzer()
  analyzer.process_single(args.folders[0])

  if args.stats_single:
    analyzer.stats_single(0)
  if args.iters_single:
    analyzer.iters_single(0)

# Stats for a batch of runs.
else:
  analyzer = Analyzer()
  analyzer.process_folders(args.folders)

  if args.stats:
    analyzer.stats()
  elif args.plot:
    analyzer.plot()
