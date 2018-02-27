#include "clr.h"
#include <sys/time.h>
#include <fstream>

extern cybozu::RandomGenerator rg;

using namespace std;

double get_wall_time(){
  struct timeval time;
  if (gettimeofday(&time, NULL)){
    return 0;
  }
  return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

int main(int argc,char* argv[]){
  if(argc != 2){
    cerr << "error : input datafile" << endl;
    return -1;
  }

  double time;
  string prvFile,pubFile;

  prvFile = "../data/prvsample.dat";
  pubFile = "../data/pubsample.dat";

  SHE::init();

  SHE::SecretKey sec;
  sec.setByCSPRNG(rg);
  
  SHE::PublicKey pub;
  sec.getPublicKey(pub);

  CLR::Save(prvFile,sec.getStr(mcl::IoFixedSizeByteSeq));
  CLR::Save(pubFile,pub.getStr(mcl::IoFixedSizeByteSeq));

  int N,d;
  string a = argv[1];
  string b = "../data/encdata.dat";
  CLR::dataEncryption(a,&N,&d,b,pubFile);

  ofstream ofs("../data/datapara.dat",std::ios::binary);
  ofs << N << endl;
  ofs << d << endl;

  return 0;
}
