@echo off
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
set QTDIR=C:\Qt\6.5.3\msvc2019_64
cd /d "C:\Users\altay\OneDrive\Projects\etrek"
"C:\Program Files\CMake\bin\cmake.exe" --preset Qt-Debug

