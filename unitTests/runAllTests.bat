@echo off
REM Configure
cmake -S . -B build

REM Build Debug
cmake --build build --config Debug

REM Run tests
ctest --test-dir build -C Debug --output-on-failure

pause