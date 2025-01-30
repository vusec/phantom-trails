#!/bin/sh

cd /chipyard && AFL_PASSTHROUGH=1 ./scripts/build-toolchain-extra.sh riscv-tools
cd /chipyard/tools/DRAMSim2 && CXX=g++ make
cd /chipyard/sims/verilator && make -f stockboom-Makefile -j20 VERILATOR="verilator --cc --exe --no-timing" CONFIG=MediumBoomConfig
