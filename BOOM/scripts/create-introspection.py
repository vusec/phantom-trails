import sys
import os


if len(sys.argv) < 2:
    print(f"Usage: {sys.argv[0]} <PATH_TO_HEADER>")

header = open(sys.argv[1])
f = open("Introspection.h", "w")

f.write("""
#pragma once

#include <stdio.h>
#include <iostream>
#include <tuple>

#include <verilated.h>
#include <debug.h>
#include </clang/llvm/compiler-rt/include/tainting.h>

const std::tuple<uint64_t, uint64_t, const char *> TestHarnessNameMap[] = {

""")

separator = ""
for line in header:
    if " TestHarness__DOT" in line:
        member = line.split(" ")[-1].replace(';','').replace("\n","")
        pretty_name = member.replace("TestHarness__DOT__chiptop__DOT__system__DOT__","").replace("__DOT__",".")
        f.write(separator + "\n" + "{")
        f.write(f"offsetof(VTestHarness,{member}), sizeof(VTestHarness::{member}), \"{pretty_name}\"")
        f.write("}")
        separator = ","


f.write("""
};

bool testHarnessTaint[sizeof(VTestHarness)] = { 0 };

static void printTaint(const VTestHarness* top) {
    for (auto &[off, size, name] : TestHarnessNameMap) {
        uint8_t *rawByte = (uint8_t *)((uint64_t)(top) + off);
        bool tainted = Tainting::check(rawByte, size);

        if (tainted and not testHarnessTaint[off]) {
            TRACE("[T] %s (0x%x)\\n", name, off);
        }
        if (not tainted and testHarnessTaint[off]) {
            TRACE("[U] %s (0x%x)\\n", name, off);
        }

        testHarnessTaint[off] = tainted;
    }
}
""")

f.close()
