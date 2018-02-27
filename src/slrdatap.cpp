#include "slr.h"
#include "comm.h"
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
  ROT::SysInit();

  int N,d;
  double s_time,e_time;
  string pubFile = "../data/slrpub.dat";
  string prvFile = "../data/slrprv.dat";
  string samplefile = "../data/train.dat";
  string datasets = "../data/encdata.dat";

  int opt;
  while((opt = getopt(argc, argv, "f:")) != -1){
    switch(opt){
    case 'f':
      {
	samplefile = optarg;
	break;
      }
    default:
      {
	fprintf(stderr,"error! \'%c\' \'%c\'\n", opt, optopt);
	return -1;
	break;
      }
    }
  }

  const mcl::EcParam& para = mcl::ecparam::secp192k1;
  const Fp x0(para.gx);
  const Fp y0(para.gy);
  const Ec P(x0, y0);
  const size_t bitLen = para.bitSize;
  
  Elgamal::PrivateKey prvt;
  prvt.init(P, bitLen, rg);
  const Elgamal::PublicKey& pubt = prvt.getPublicKey();

  SLR::Save(prvFile, prvt.getStr(mcl::IoArray));
  SLR::Save(pubFile, pubt.getStr(mcl::IoArray));

  s_time = get_wall_time();
  SLR::dataEncryption(samplefile,&N,&d,datasets,pubFile);
  cerr << "dataEnc : " << get_wall_time() - s_time << "s" << endl;

  ofstream ofs("../data/datapara.dat",std::ios::binary);
  ofs << N << endl;
  ofs << d << endl;

  return 0;
}
