#!/bin/bash

#converting csv data format to training/test data format
python convert.py ../data/10-5.csv ../data/train.dat ../data/test.dat 3

./maketable 1000

#encrypting training data for data providers
L=`./encdata ../data/train.dat`

#starting server
./clrserver -p 22345 -c 4 &
sleep 2s
#starting analyst
./clrclient -i 10 -l $L -r 0.1 -a 2 -p 22345 -h localhost

#evaluating CLR
python calcAUC.py ../comm/client/result.dat ../data/test.dat
