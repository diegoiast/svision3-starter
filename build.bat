@echo off
setlocal

conan install . -s build_type=Debug --build=missing || exit /b 1
cmake --preset conan-debug || exit /b 1
cmake --build build\Debug --config Debug || exit /b 1
