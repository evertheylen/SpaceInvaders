#!/bin/bash

# Build the project
mkdir -p build
cd build
cmake .. && make

rc=$?; if [[ $rc != 0 ]]; then echo "Build failed"; exit $rc; fi

# Execute
cd ../src
../build/exec example.json SFML:VC




