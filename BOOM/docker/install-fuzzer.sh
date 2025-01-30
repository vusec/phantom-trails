#!/bin/bash

set -e

source "$HOME/.cargo/env"
cargo build --release
cargo build
