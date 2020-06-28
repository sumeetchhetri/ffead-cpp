#!/bin/sh

git clone https://github.com/TechEmpower/FrameworkBenchmarks
cd FrameworkBenchmarks
./tfb --mode verify --test-dir C++/ffead-cpp
