@echo off

if not exist build mkdir build

rem TODOS
rem Search for AVR toolchain
rem report if i cannot be found
rem Search for AVRDUDE to flash to board
rem report if i cannot be found

set SOURCES= ../src/temperatureHard.c

rem BUILD TOOLS
rem ************************************************************
set AVR_GCC=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr-gcc.exe
set AVR_LD=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr_ld.exe

rem ************************************************************
rem COMPILER(AVRGCC) OPTIONS
rem ************************************************************
set AVR_GCC_FLAGS=
set AVR_LD_FLAGS=


rem ************************************************************
rem START BUILD
rem ************************************************************
set path="F:\Dev\Embedded\arduino\build";path

pushd build

echo Compiling...
%AVR_GCC% %SOURCES%

popd
goto EOF

:EOF