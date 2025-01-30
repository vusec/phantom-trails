import sys
import os
import subprocess
from multiprocessing import Pool

def process(bin_name):
    print(f"===== {bin_name} =====")
    print_flags = "PRINT_CYCLES=1 PRINT_COVERAGE=1 PRINT_ROB=1 CRASH_REPORT=1 VERBOSE_SIM=1"
    command = f"{print_flags} phantom-trails run --init /boom-wrapper/init/init --rom /boom-wrapper/boot/bootrom.rv64.img \"{bin_name}\""
    outfile = open(bin_name + ".cov")
    subprocess.run(command, shell=True, stdout=outfile)
    outfile.close()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print(f"Usage {sys.argv[0]} <FOLDER>")
        exit(-1)

    directory = sys.argv[1]
    files = []

    for filename in os.listdir(directory):
        if "." in filename:
            continue

        files.append(os.path.join(directory, filename))

    p = Pool()
    p.map(process, files)
    p.close()
