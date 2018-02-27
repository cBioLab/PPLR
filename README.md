# PPLR

C++ implementation of PPLR

# Summary
PPLR is a library for computing Logistic Regression in privacy-preserving manner with Homomorphic Encryption. 
It contains two applications CLR and SLR.
* CLR : Secure version (Primary submission)
* SLR : Fast version (Secondary submission)

# List of Supported CPUs and Operating Systems
* Intel 64-bit CPUs
* 64-bit Linux (tested on ubuntu 14.04 LTS and ubuntu 16.04 LTS)

# List of Supported Compilers and Prerequisite Tools
* gcc 4.8.4 + OpenSSL 1.0.1f + GMP 5.1.3

# Installation Requirements
Create a working directory (e.g., work) and clone the following repositories.

		cd ~
		mkdir work
		cd work
		git clone git://github.com/cBioLab/PPLR.git
		git clone git://github.com/iskana/PBWT-sec.git
		git clone git://github.com/herumi/xbyak.git
 		git clone git://github.com/herumi/mcl.git
		git clone git://github.com/herumi/cybozulib.git
		git clone git://github.com/herumi/cybozulib_ext.git
* Xbyak is a prerequisite for optimizing the operations in the finite field on Intel CPUs.
* OpenSSL and libgmp-dev are available via apt-get (or other similar commands).

# Installation
       cd mcl
       git checkout 61473db5730731327059fa42a445853d853c6a21
       cd ..
       cd cybozulib
       git checkout 94b914a1d93b3ecf7f7aa096006d8a72b39c7496
       cd ..
       cd cybozulib_ext
       git checkout fe3bfe014e72c779433e8b02bef9066e2bfec9db
       cd ..
       cd xbyak
       git checkout c5da3778e7f84013fe8c26fcf18a67881bd1e825
       cd ../PPLR/src
       make

# Prerequisite Files and Libraries for Running Your Application
	* OpenSSL
	* GMP (libgmp-dev)
	
# Running CLR
	precomputation
	./dataenc [training data file]

	@server
	./clrserver -p [port number]　-c [the number of threads]
	
	@client
	./clrclient -h [ip address of server] -p [port number] -i [the number of learning iterations] -l [learning rate] -r [coefficient of L2 norm] -a [accuracy]

* This demonstration shows the execution procedure of CLR after the server receives the encrypted data from the data providers. For convenience, data encryption is executed beforehand and all ciphertexts is on the server.
* Encrypted data is PPLR/data/encdata.dat. Before running the server, you put this file in PPLR/data/ of the server.
* If accuracy is *s*, the logistic regression model parameters are calculated with *s* digits after the decimal point.


# Running SLR
		./slrtest -f [training data file]
		python slr.py [learning rate] [coefficient of L2 norm]
		
* File exchange is done locally (server and client run in local machine).	

# Converting from iDASH-sample to training data file and test data file
		cd PPLR/data
		python convert.py [iDASH-sample e.g."LR_dataWith100SNP.txt"] [training data file] [test data file]

# Training data format
 	   	1st line   : The number of data providers
	   	2nd line   : Dimension of data 
	   	3rd line ~ : Data
		(i-th line : Data of (i-2)-th data providers [y x1 x2 ... xD])

# Copyright Notice
Copyright (C) 2017, Masanobu Jimbo 
All rights reserved.

# License
PPLR (files in this repository) is distributed under the [BSD 3-Clause License] (http://opensource.org/licenses/BSD-3-Clause "The BSD 3-Clause License").

# Licenses of External Libraries
Licenses of external libraries are listed as follows.

* PBWT-sec: BSD-3-Clause
* cybozulib: BSD-3-Clause
* mcl: BSD-3-Clause
* Xbyak: BSD-3-Clause
* MPIR: LGPL2
* OpenSSL: Apache License Version 1.0 + SSLeay License

Software including any of those libraries is allowed to be used for commercial purposes without releasing its source code as long as the regarding copyright statements described as follows are included in the software.

* This product includes software that was developed by an OpenSSL project in order to use OpenSSL toolkit.
* This product includes PBWT-sec.
* This product includes mcl, cybozulib, and Xbyak.
* This product includes MPIR.

# Contact Information
* Masanobu Jimbo (jimwase@asagi.waseda.jp)

# History

2017/August/16; initial version
