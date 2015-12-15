#!/usr/bin/env bash

echo "clean workspace"
rm -rf ./mosquitto
rm -rf ./libs/mosquitto
mkdir -p ./libs/mosquitto/src

echo "clone mosquitto library"
git clone http://git.eclipse.org/gitroot/mosquitto/org.eclipse.mosquitto.git ./mosquitto

echo "copy mosquitto files"
cp ./mosquitto/config.h ./libs/mosquitto/src/config.h
cp ./mosquitto/lib/*.h ./libs/mosquitto/src/
cp ./mosquitto/lib/*.c ./libs/mosquitto/src/

echo "remove temporary files"
rm -rf ./mosquitto
