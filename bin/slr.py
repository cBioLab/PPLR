# -*- coding: utf-8 -*-
import numpy as np
import sys
import csv
    
def sigmoid(theta,x):
    u = theta.dot(x)
    if u > -100:
        return 1/(1 + np.exp((-1) * u))
    else:
        return 0

def update(theta,A,B,Lambda):
    dim = theta.size
    a = np.array([-0.714761,-0.5,-0.0976419])
    n = a[2] * B.dot(theta) + a[1] * A + Lambda * theta
    return n

def loadData():
    Afile = "../comm/client/A.csv"
    Bfile = "../comm/client/B.csv"
    A = np.loadtxt(Afile,delimiter=",", skiprows=0)
    B = np.loadtxt(Bfile,delimiter=",", skiprows=0)
    #print A
    #print B
    return A,B

def learn(alpha,Lambda):
    #thetaの初期値設定
    A,B = loadData()
    
    #最急降下法
    old = (np.random.rand(B.shape[0])-0.5)*2 #-1~1
    for i in range(1000):
        new = old - alpha * update(old,A,B,Lambda)
        if(np.linalg.norm(new-old) < 0.0001):
            break
        old = new
    return new

if __name__ == "__main__":
    argvs = sys.argv
    argc = len(argvs)

    if (argc != 3):
        print "error"
        quit()

    alpha = float(argvs[1])
    Lambda = float(argvs[2])

    theta = learn(alpha,Lambda)

    f = open('../comm/result.dat', 'w')
    writer = csv.writer(f, lineterminator='\n')
    writer.writerow(theta)
    f.close()
