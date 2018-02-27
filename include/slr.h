#include "rot.h"

extern cybozu::RandomGenerator rg;

namespace SLR{

  void Save(const std::string& name,const std::string& s);
  void Load(std::string& s ,const std::string& name);

  //実験用データ暗号化関数
  void dataEncryption(std::string& samplefile,int *N, int *d,std::string& datasets,std::string& pubFile);
  void checkSample(std::string& samplefile,std::vector<int> dected);

  class Server{
    Elgamal::PublicKey pub;
    Elgamal::CipherText *fd; //datasize * dimension * 3
    Elgamal::CipherText *sd; //datasize * dimension * 2
    Elgamal::CipherText *td; //dimension * 2
  public:
    int datasize;
    int dimension;
    void setParam(std::string& cparam); //recv:datasize,dimension
    void readPublicKey(std::string& pubFile);
    //暗号化された値を読み込みclientに送信するファイルを作成する
    void dataProcessing(std::string& datasets,std::string& processed);
    Server(){
      std::cerr << "make server" << std::endl;
    }
  private:
    void fd2sd();
    void sd2td();
  };

  class Client{
    Elgamal::PublicKey pub;
    Elgamal::PrivateKey prv;
    Elgamal::CipherText *td; //dimension * 2
    std::vector<int> dectd; //dimension * 2
    std::vector<int> A; //dimension
    std::vector<double> B; //dimension * dimension
    std::vector<double> theta;
  public:
    int datasize;
    int dimension;
    void setParam(int N,int d);
    void makeParam(std::string& cparam);
    void setKeys(std::string& prvFile, std::string& pubFile);
    void setData(std::string& processesd,std::string& Afile,std::string& Bfile);
    Client(){
      std::cerr << "make client" << std::endl;
    }
  private:
    void setA(std::string& Afile);
    void setB(std::string& Bfile);
  };
}
