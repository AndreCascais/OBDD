#!/bin/bash

./obdd inputs/input.txt input.dot
dot -Tpng -o input.png input.dot
./obdd inputs/input0000.txt input0000.dot
dot -Tpng -o input0000.png input0000.dot
./obdd inputs/input4var.txt input4var.dot
dot -Tpng -o input4var.png input4var.dot
./obdd inputs/input01010101.txt input01010101.dot
dot -Tpng -o input01010101.png input01010101.dot
./obdd inputs/input10100011.txt input10100011.dot
dot -Tpng -o input10100011.png input10100011.dot
./obdd inputs/input5var.txt input5var.dot
dot -Tpng -o input5var.png input5var.dot

exit $?
