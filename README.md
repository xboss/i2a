# i2a
A tool for converting images and C arrays to each other.
i2a is implemented by ANSI C, it can be compiled and used on Linux, OSX, Windows, *nix. 

## Features
* Convert BMP file to C array.
* Convert C array to BMP file.
* Currently, only the RGB16(RRRRRGGGGGGBBBBB (2byte/pixel)) is supported.
* Currently, only the BMP(24 bit full color) is supported.

## Build

```
cd i2a
mkdir build
cd build
cmake ..
make
```

## Usage examples
Convert C array to BMP file:
```
$ i2a a2b your_c_array.h rgb16 800 600 24

$ write BMP file ok. your_c_array.h.bmp

```

Convert BMP file to C array:
```
$ i2a b2a your.bmp rgb16 > your_c_array.h

```

help:
```
$ i2a

$ Usage: i2a <mode> <file path> <color mode> <width> <height> <color bit count>
$     <mode>: a2b or b2a
$         a2b: Convert C array file to BMP file.
$         b2a: Convert BMP file to C array file.
$     <file path>: C array file or BMP file.
$     <color mode>: The color mode of each pixel in the array. Now only supports the following formats:
$         rgb16: RRRRRGGGGGGBBBBB (2byte/pixel).
$     <width>: Image's width. Mandatory in a2b mode.
$     <height>: Image's height. Mandatory in a2b mode.
$     <color bit count>: Color depth of BMP file. Mandatory in a2b mode. Now only supports the following formats:
$         24: 24 bit full color.

```
Only one array is allowed in "c_array.h".
The format of "c_array.h" example:
```
static const uint16_t bmp_arr[] = {
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
    0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF}
```
