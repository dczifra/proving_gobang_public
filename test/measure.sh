#!/bin/bash
for col in 5 6 7 8 9 10 11 12 13 14 15 16
do
    sed -i 's/const unsigned int COL=.*/const unsigned int COL='"${col}"';/g' ../src/common.h
    make -j5
    time ./AMOBA "$@"
done
