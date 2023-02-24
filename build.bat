rem @echo off

rem set "path=c:\compilers\mingw+msys+mips-rtems4.10\opt\mips-rtems4.10\mips-rtems4.10\bin\;c:\compilers\mingw+msys+mips-rtems4.10\opt\mips-rtems4.10\bin\;c:\compilers\mingw+msys+mips-rtems4.10\bin\;C:\Program Files\CMake\bin"

set "source=.\"
set "project=..\burkProject"

cmake -DCMAKE_TOOLCHAIN_FILE=%source%toolchain-mips-rtems4.10.cmake -B%project% -G "Eclipse CDT4 - MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug -DCMAKE_ECLIPSE_VERSION=4.11 -DCMAKE_ECLIPSE_MAKE_ARGUMENTS=-j8 -H%source%

pause
