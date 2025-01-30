#!/bin/bash

mkdir -p in_sim
cp /riscv-samples/tests/seed.bin in_sim/empty
mkdir -p out

date
sim-fuzzer -i in_sim -o out/ ./run-FuzzConfig @@ /boom-wrapper/boot/bootrom.rv64.img 0 20
date
