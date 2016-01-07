#!/bin/bash

# Build the project
rm -rf build
mkdir -p build
cd build
cmake .. && make

rc=$?; if [[ $rc != 0 ]]; then echo "Build failed"; exit $rc; fi




