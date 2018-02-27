#!/usr/bin/env python
# -*- coding: utf-8 -*-

import numpy as np
import sys
import csv

argvs = sys.argv
argc = len(argvs)

if (argc != 5):
    print "error : python convert.py [input-filename] [train-filename] [test-filename] [the number of covariates used for learning]"
    quit()

print "separating training data and test data"

inputfile = argvs[1]
trainfile = argvs[2]
testfile = argvs[3]
end = int(argvs[4])

Data = np.loadtxt(inputfile,delimiter=",",skiprows=1,dtype='int')

N = Data.shape[0]

train = Data[:int(N*0.8),:]
test = Data[int(N*0.8):,:]

train = train[:,0:(end+1)]
test = test[:,0:(end+1)]

print "training data size : " + str(train.shape)
print "test data size : " + str(test.shape)

fp = open(trainfile,'w')
fp.write(str(train.shape[0])+"\n")
fp.write(str(train.shape[1])+"\n")
for data in train:
    for d in data:
        fp.write(str(int(d))+" ")
    fp.write("\n")
fp.close()

f = open(testfile,'w')
writer = csv.writer(f, lineterminator='\n')
writer.writerows(test)
f.close()
