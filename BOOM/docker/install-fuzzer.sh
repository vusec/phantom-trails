#!/bin/bash

set -e

source "$HOME/.cargo/env"


# This is a total house of cards
#
# We currently use rust 1.72 nightly from 2023-05-31
#
# We cannot upgrade to the _actual_ Rust 1.72 release as it made a change to
# the TypeId type that breaks LibAFL:
# https://github.com/AFLplusplus/LibAFL/pull/1311
#
# Current versions of the packages pulled in by LibAFL dependencies, however,
# break with Rust 1.72 nightly, they explicitly require newer rust versions
# (1.82 and up or so as of 2025-07-02).
#
# The correct way to fix this would be to update our vendored LibAFL, but that
# is currently not possible due to time constraints, so we manually downgrade
# packages that require newer rust versions. This list will grow in the future
# unless we upgrade LibAFL and our Rust version.
cargo update -p postcard@1.1.3 --precise 1.1.1
#cargo update -p backtrace@0.3.75 --precise 0.3.68

cargo update -p uuid --precise 1.3.0
#cargo update -p getrandom@0.4.2 --precise 0.3.4


cargo build --release
cargo build
