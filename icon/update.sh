#!/bin/bash

cd $(dirname $0)/..

echo "// auto generated file, do not edit!" > logo.h
echo "#include <Arduino.h>" >> logo.h
echo >> logo.h

./icon/convert.py icon/icon.png 1 >> logo.h

echo >> logo.h

