if not exist build mkdir build

rem Search for AVR toolchain
set AVR_GCC=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr_gcc.exe
set AVR_LD=F:\Dev_Tools\avr8-gnu-toolchain-win32_x86\bin\avr_ld.exe
rem report if i cannot be found
rem Search for AVRDUDE to flash to board
set AVRDUDE=F:\avrdude\...exe
rem report if i cannot be found

set AVR_GCC_FLAGS=
set AVR_LD_FLAGS=

set SOURCES=temeratureHard.c

pushd build

AVR_GCC AVR_GCC_FLAGS SOURCES

popd
goto EOF