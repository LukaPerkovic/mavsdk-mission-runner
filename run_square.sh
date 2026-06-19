#!/bin/bash
set -e
cmake --build build
./build/fly_a_square udpin://0.0.0.0:14540
