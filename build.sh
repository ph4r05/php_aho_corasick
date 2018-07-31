#!/bin/bash
phpize --clean
phpize
./configure --enable-ahocorasick
make clean
make
