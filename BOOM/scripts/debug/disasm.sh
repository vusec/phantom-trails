#!/bin/bash

riscv64-unknown-elf-objdump -b binary -m riscv:rv64 -M no-aliases -D $@
