#!/bin/bash
set -e
cmake --build build
./build/issue_sim udpin://0.0.0.0:14540 "$@"
