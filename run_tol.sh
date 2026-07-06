#!/bin/bash
set -e
cmake --build build
./build/takeoff_and_land udpin://0.0.0.0:14540
