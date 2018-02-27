# -*- coding: utf-8 -*-
import numpy as np
import sys
from sklearn import metrics

def loadCSV(filename):
    l = np.loadtxt(filename,delimiter=",",skiprows=0)
    return l
    
def sigmoid(theta,x):
    u = theta.dot(x)
    if u > -100:
        return 1/(1 + np.exp((-1) * u))
    else:
        return 0

def check(theta,X,y,thr):
    size = y.size
    tcount = 0.0
    t = 0.0
    fcount = 0.0
    f = 0.0
    for i in range(size):
        est = sigmoid(theta,X[i,:])
        if y[i] == 1:
            t += 1
            if thr < est:
                tcount += 1
        elif y[i] == 0:
            f += 1
            if thr >= est:
                fcount += 1
    sens = tcount / t
    fp = (f-fcount) /f
    return sens,fp

if __name__ == "__main__":
    argvs = sys.argv
    argc = len(argvs)
    if(argc != 3):
        print "error : python calcAUC.py [LR parameters file] [test data file]"
        quit()
    print "evaluation of theta"
    itrmax = 1000
    theta = loadCSV(argvs[1])
    test = loadCSV(argvs[2])
    X = test[:,1:]
    y = test[:,0]
    X0 = np.ones(y.size)
    X = np.c_[X0.T,X]
    sen = np.zeros(itrmax)
    fp = np.zeros(itrmax)
    for i in range(itrmax):
        thr = 1 - float(i)/itrmax
        s,f = check(theta,X,y,thr)
        sen[i] = s
        fp[i] = f
    print "Parameters : " + str(theta)
    print "AUC : " + str(metrics.auc(fp,sen))
