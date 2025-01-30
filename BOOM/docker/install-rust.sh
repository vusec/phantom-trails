#!/bin/bash

set -e

# Install Rust by piping some website into our shell like a boss.
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh -s -- -y --no-modify-path
source "$HOME/.cargo/env"
rustup default nightly-2023-06-01
