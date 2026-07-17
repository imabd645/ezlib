@echo off
REM ============================================================================
REM Build the ezauth native crypto accelerator.
REM
REM   usage:  build.bat
REM
REM NOTE: mingw64\bin MUST be on PATH, not just referenced by absolute path.
REM Invoking g++.exe by full path alone leaves cc1plus.exe unable to find its own
REM dependency DLLs (mpfr/gmp/isl/...), and it then fails with exit code 1 and NO
REM error message at all.
REM ============================================================================

REM Sources and %OUT% are relative, so run from THIS script's directory no matter
REM where it was invoked from.
cd /d "%~dp0"

set MINGW=C:\msys64\mingw64
set PATH=%MINGW%\bin;%PATH%

set OUT=..\dlls

REM -lbcrypt = Windows CNG: PBKDF2, HMAC, and the system CSPRNG. It ships with
REM Windows, so this adds no redistributable dependency.
REM
REM -static (not just -static-libgcc/-static-libstdc++) matters: without it the
REM DLL imports libwinpthread-1.dll, and os_load_lib then fails with nothing more
REM informative than a nil handle unless that sidecar sits next to it. Linking it
REM in keeps this package a single self-contained file.
echo [1/1] auth_accel.dll
g++ -shared -O2 -o %OUT%\auth_accel.dll auth_accel.cpp -lbcrypt -static -static-libgcc -static-libstdc++
if errorlevel 1 goto :failed

echo.
echo Build OK -^> %OUT%
goto :eof

:failed
echo.
echo BUILD FAILED
exit /b 1
