#!/bin/bash

#converting csv file to training data and test data for PPLR
python convert.py ../data/10-5.csv ../data/train.dat ../data/test.dat 3

#data encryption
./slrdatap -f ../data/train.dat

#server start
./slrserver -p 32345 &

sleep 1s

#client start
./slrclient -p 32345 -h localhost
python slr.py 0.001 0.1

#evaluating SLR
python calcAUC.py ../comm/result.dat ../data/test.dat
