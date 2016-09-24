#!/usr/bin/env python

import png
import sys
import array

f = open(sys.argv[1], 'rb')
r = png.Reader(file = f)
width, height, pixels, info = r.read()

if (width != 128) or (height != 64):
    print "Image needs to be 128x64!"
    f.close()
    sys.exit(1)

if not info['greyscale']:
    print "Image needs to be Greyscale!"
    f.close()
    sys.exit(1)

if info['bitdepth'] != 1:
    print "Image needs to be single-bit bitmap!"
    f.close()
    sys.exit(1)

if info['alpha']:
    print "Image can't have alpha channel!"
    f.close()
    sys.exit(1)

invertImage = False
if len(sys.argv) > 2 and sys.argv[2] == '1':
    invertImage = True

# Create output data buffer
out = array.array('B')
for i in range(1024):
    if invertImage:
        out.append(0xFF)
    else:
        out.append(0)

# Walk image pixels and fill buffer
l = list(pixels)
for x in range(128):
    for y in range(64):
        col = x
        row = y / 8
        bit = y % 8
        if invertImage:
            out[(row * 128) + col] &= ~(l[y][x] << bit)
        else:
            out[(row * 128) + col] |= (l[y][x] << bit)

f.close()

# Print as C data structure
print "const PROGMEM uint8_t bootLogo[1024] = {"
for y in range(64):
    s = "    "
    for x in range(16):
        s += "0x{:02X}".format(out[x + (y * 16)])
        if x < 15:
            s += ', '
        elif y < 63:
            s += ','
    print s
print "};"

