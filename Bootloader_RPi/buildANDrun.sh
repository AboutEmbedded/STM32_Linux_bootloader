#!/bin/bash
sudo rm -r ./build
mkdir ./build
cd ./build
cmake ../
make
#dir for rpi
sudo rm -r ../HexLoaderApp
mkdir ../HexLoaderApp
#copy files
cp ../TestHexes/Firmware_0_14_9_powerDelete.hex ../HexLoaderApp/Firmware.hex
cp ./HexLoader ../HexLoaderApp/HexLoader
cd ../HexLoaderApp
#./HexLoader
