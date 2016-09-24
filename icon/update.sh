#!/bin/bash

cd $(dirname $0)/..

echo "// file auto-generated with icon/update.sh!" > logo.h
echo "// DO NOT EDIT BY HAND!" >> logo.h
echo >> logo.h
echo "#include <Arduino.h>" >> logo.h
echo >> logo.h

./icon/convert.py icon/icon.png 1 >> logo.h

echo >> logo.h

