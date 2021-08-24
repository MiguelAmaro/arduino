@echo off

set PROJECT_NAME=blinkyHard

set AVRDUDE=F:\Dev_Tools\avrdude\bin\avrdude.exe
set CONFIG_FILE=F:\Dev_Tools\arduino-1.8.15\hardware\tools\avr\etc\avrdude.conf

set BINARY=".\build\blinkyHard.hex"
set PART=m328p
set PORT=COM5
set BAUD_RATE=115200

set AVRDUDE_FLAGS=^
-F ^
-v ^
-carduino ^
-p%PART% ^
-C%CONFIG_FILE% ^
-P%PORT% ^
-b%BAUD_RATE% ^
-D ^
-Uflash:w:%BINARY%:i

%AVRDUDE% %AVRDUDE_FLAGS%