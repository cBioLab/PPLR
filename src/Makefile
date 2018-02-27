TOPDIR:=$(realpath $(dir $(lastword $(MAKEFILE_LIST))))/
CFLAGS= -O3 -g
CFLAGS+= -I$(TOPDIR)include -I$(TOPDIR)../../cybozulib/include/ -I$(TOPDIR)../../mcl/include -I$(TOPDIR)../../xbyak/ -I$(TOPDIR)../../PBWT-sec/include -I$(TOPDIR)../include/
LDFLAGS+= -lmcl -lgmp -lgmpxx  -L../../mcl/lib/ -lcrypto

ROT_OBJ=../../PBWT-sec/src/rot.o
COMM_OBJ=../../PBWT-sec/src/comm.o

CLR_SERVER = $(ROT_OBJ) $(COMM_OBJ) clr.o clrserver.o
CLR_CLIENT = $(ROT_OBJ) $(COMM_OBJ) clr.o clrclient.o
CLR_ENC = $(ROT_OBJ) clr.o encdata.o
CLR_TABLE = $(ROT_OBJ) clr.o maketable.o
MCL_LIB=../../mcl/lib/libmcl.a
SLR_SERVER = $(ROT_OBJ) $(COMM_OBJ) slr.o slrserver.o
SLR_CLIENT = $(ROT_OBJ) $(COMM_OBJ) slr.o slrclient.o
SLR_ENC = $(ROT_OBJ) slr.o slrdatap.o

all: $(MCL_LIB) pbwt_sec clrclient clrserver encdata maketable slrclient slrserver slrdatap

../../mcl/lib/libmcl.a:
	make -C ../../mcl USE_LLVM=0

pbwt_sec:
	make -C ../../PBWT-sec/src/

clean:
	rm -rf ../bin/clrserver ../bin/clrclient *~ $(CLR_SERVER) $(CLR_CLIENT) ../bin/encdata $(CLR_ENC) ../bin/maketable $(CLR_TABLE) $(SLR_SERVER) ../bin/slrclient ../bin/slrserver $(SLR_CLIENT) ../bin/slrdatap $(SLR_ENC)

.SUFFIXES: .cpp

.cpp.o:
	$(CXX) -c $< -o $@ $(CFLAGS) -fopenmp

clrserver: $(CLR_SERVER) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(CLR_SERVER) -o $@ $(LDFLAGS) -fopenmp
	mv clrserver $(TOPDIR)../bin/

clrclient: $(CLR_CLIENT) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(CLR_CLIENT) -o $@ $(LDFLAGS) -fopenmp
	mv clrclient $(TOPDIR)../bin/

encdata: $(CLR_ENC) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(CLR_ENC) -o $@ $(LDFLAGS) -fopenmp
	mv encdata $(TOPDIR)../bin/

maketable: $(CLR_TABLE) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(CLR_TABLE) -o $@ $(LDFLAGS) -fopenmp
	mv maketable $(TOPDIR)../bin/

slrserver: $(SLR_SERVER) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(SLR_SERVER) -o $@ $(LDFLAGS) -fopenmp
	mv slrserver $(TOPDIR)../bin/

slrclient: $(SLR_CLIENT) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(SLR_CLIENT) -o $@ $(LDFLAGS) -fopenmp
	mv slrclient $(TOPDIR)../bin/

slrdatap: $(SLR_ENC) $(MCL_LIB)
	mkdir -p ../bin/
	mkdir -p ../comm/
	$(CXX) $(SLR_ENC) -o $@ $(LDFLAGS) -fopenmp
	mv slrdatap $(TOPDIR)../bin/
