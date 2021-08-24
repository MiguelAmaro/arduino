@echo off

if not exist build mkdir build

rem TODOS
rem Search for AVR toolchain
rem report if i cannot be found
rem Search for AVRDUDE to flash to board
rem report if i cannot be found

set PROJECT_NAME=blinkyHard
set SOURCES= ../src/%PROJECT_NAME%.c

rem BUILD TOOLS
rem ************************************************************
set AVR_GCC=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr-gcc.exe
set AVR_LD=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr_ld.exe
set AVR_OBJCOPY=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr-objcopy.exe

rem ************************************************************
rem COMPILER(AVRGCC) OPTIONS
rem ************************************************************
set AVR_GCC_FLAGS=-mmcu=atmega328p -DF_CPU=1000000UL -Wall -Os -o %PROJECT_NAME%.elf

set AVR_LD_FLAGS=


rem ************************************************************
rem START BUILD
rem ************************************************************
set path="F:\Dev\Embedded\arduino\build";path

pushd build

echo Compiling...
%AVR_GCC% %AVR_GCC_FLAGS% %SOURCES%

echo Converting Binary to Intel Hex format...
%AVR_OBJCOPY% -j .text -j .data -O ihex %PROJECT_NAME%.elf %PROJECT_NAME%.hex

popd
goto EOF

:EOF