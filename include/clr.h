#include <iostream>
#include <cmath>
#include <mcl/she.hpp>
#include <cybozu/random_generator.hpp>
#include <rot.h>

typedef mcl::she::SHET<mcl::bn256::BN, mcl::bn256::Fr> SHE;

extern cybozu::RandomGenerator rg;

namespace CLR{

  void Save(const std::string& name,const std::string& s);
  void Load(std::string& s ,const std::string& name);

  void dataEncryption(std::string& samplefile,int *N, int *d,std::string& datasets,std::string& pubFile);

  class Server{
    SHE::PublicKey pub;
    std::vector<SHE::CipherTextG1> A_raw; //datasize*dimension
    std::vector<SHE::CipherTextG1> B_raw; //datasize*dimension*(dimension+1)/2 
    std::vector<SHE::CipherTextG1> A; //dimension
    std::vector<SHE::CipherTextG1> B; //dimension*dimension
    std::vector<SHE::CipherTextG2> theta; //dimension
    std::vector<SHE::CipherTextM> gradient; //dimension
    std::vector<int> randvec; //dimension
  public:
    int datasize;
    int dimension;
    int itrmax;
    int lambda; //x8
    int random; //theta初期値用
    int ac;
    int lpara;
    int core;
    void setParam(std::string& cparam); //datasize,dimension,itrmax,lambda
    void readPublicKey(std::string& pubFile);
    void dataProcessing(std::string& datasets);
    void calcGradient(std::string& thetaFile,std::string& gradientFile,int itrnum);
    Server(int c=1){
      core = c;
      std::cerr << "make server" << std::endl;
    }
  };

  class Client{
    SHE::PublicKey pub;
    SHE::SecretKey prv;
    std::vector<SHE::CipherTextG2> theta;
    std::vector<SHE::CipherTextM> gradient;
    std::vector<int> paras;
  public:
    int datasize;
    int dimension;
    int itrmax;
    int lambda;
    int decRange;
    int ac;
    int lpara;
    void setParam(int N,int d);
    void setTable(std::string& tableFile);
    void makeParam(std::string& cparam);
    void setKeys(std::string& prvFile,std::string& pubFile);
    void loadKeys(std::string& prvFile,std::string& pubFile);
    void initTheta(std::string& thetaFile);
    void calcTheta(std::string& gradientFile,std::string& thetaFile,std::string& resultFile,int itr);
    Client(int a,double l,double r,int i){
      std::cerr << "make client" << std::endl;
      itrmax = i;
      lambda = 8 * r; //正則化項係数
      ac = std::pow(10,a); //保証する桁数
      lpara = 1 / l; //学習係数
    }
  };
}
