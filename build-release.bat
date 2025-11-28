@echo off
cls
cmake --build .\build\ --config Release
echo -- Running program
build\Release\pesec.exe