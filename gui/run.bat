@echo off

pushd build

if exist acc_arduino.exe (start acc_arduino.exe) else (echo ERROR: "acc_arduino.exe" does not exist!!!)

popd
