#include "clr.h"
#include <sys/time.h>
#include <fstream>

extern cybozu::RandomGenerator rg;

using namespace std;

int main(int argc,char* argv[]){
  SHE::init();
  if(argc == 2 || atoi(argv[1]) >= 0){
    SHE::setRangeForGTDLP(atoi(argv[1]),10);
    ofstream ofs("../data/tableForDec.dat",ios::binary);
    SHE::gtHashTbl.save(ofs);
    cout << "save table" << endl;
  }else{
    SHE::SecretKey sec;
    sec.setByCSPRNG(rg);
    SHE::PublicKey pub;
    sec.getPublicKey(pub);
    SHE::CipherTextG1 cg1;
    SHE::CipherTextG2 cg2;
    SHE::CipherTextM cgt;
    pub.enc(cg1,100,rg);
    pub.enc(cg2,-73,rg);
    SHE::CipherTextM::mul(cgt,cg1,cg2);
    ifstream ifs("../data/tableForDec.dat",ios::binary);
    SHE::gtHashTbl.load(ifs);
    cout << sec.dec(cgt) << endl;
  }
  return 0;
}
