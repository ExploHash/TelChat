#!/bin/bash
cmake -DCMAKE_BUILD_TYPE=Debug .
cmake --build . -- -j4
./build/TelChat