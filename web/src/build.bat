@echo off
REM ============================================================================
REM Build the ezweb native accelerators.
REM
REM  usage:  build.bat            (build http_accel.dll)
REM          build.bat all        (build http_accel.dll and web_accel.dll)
REM
REM NOTE: mingw64\bin MUST be on PATH, not just referenced by absolute path.
REM Invoking g++.exe by full path alone leaves cc1plus.exe unable to find its own
REM dependency DLLs (mpfr/gmp/isl/...), and it then fails with exit code 1 and NO
REM error message at all -- which is a thoroughly confusing way to lose an hour.
REM ============================================================================

REM Both the sources and %OUT% below are relative, so run from THIS script's
REM directory no matter where it was invoked from. Without this, calling it by
REM absolute path from elsewhere fails with "http_accel.cpp: No such file".
cd /d "%~dp0"

set MINGW=C:\msys64\mingw64
set PATH=%MINGW%\bin;%PATH%

set CXXFLAGS=-shared -O2 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread
set OUT=..\dlls

REM -lz is linked STATICALLY (libz.a) for gzip, so no zlib1.dll is needed at runtime.
echo [1/1] http_accel.dll
g++ -shared -O2 -o %OUT%\http_accel.dll http_accel.cpp -lws2_32 -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread -lz
if errorlevel 1 goto :failed

if "%1"=="all" (
    echo [2/2] web_accel.dll
    g++ -shared -O2 -o %OUT%\web_accel.dll web_accel.cpp -static-libgcc -static-libstdc++ -Wl,-Bstatic -lstdc++ -lwinpthread
    if errorlevel 1 goto :failed
)

echo.
echo Build OK -^> %OUT%
echo Reminder: the EZCallback typedef in http_accel.cpp and the
echo os_ffi_create_callback type list in ..\main.ez must match exactly.
goto :eof

:failed
echo.
echo BUILD FAILED
exit /b 1
