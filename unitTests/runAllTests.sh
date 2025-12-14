#!/usr/bin/env bash
set -e

# Configure
cmake -S . -B build

# Build Debug
cmake --build build --config Debug

# Run tests
ctest --test-dir build -C Debug --output-on-failure

read -p "Press Enter to continue..."
