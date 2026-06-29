#! /bin/sh
set -e

conan install . -s build_type=Debug --build=missing
cmake --preset conan-debug
cmake --build build/Debug

