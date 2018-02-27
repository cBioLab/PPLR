#include "slr.h"

#define BUFSIZE 256

//#define DEBUG

void SLR::Save(const std::string& name,const std::string& s){
  std::ofstream ofs(name.c_str(), std::ios::binary);
  size_t size = s.size();
  ofs.write((char *) &size,sizeof(size));
  ofs.write(s.c_str(),size*sizeof(char));	    
}

void SLR::Load(std::string& s,const std::string& name){
  std::ifstream ifs(name.c_str(), std::ios::binary);
  char* c;
  size_t size;
  ifs.read((char *) &size,sizeof(size));
  c = (char *)malloc(size*sizeof(char));
  ifs.read(c,size*sizeof(char));
  std::string str(c,size);
  s = str;
  free(c);
}

void SLR::dataEncryption(std::string& samplefile,int *N,int *d,std::string& datasets,std::string& pubFile){
  Elgamal::PublicKey pub;
  Elgamal::CipherText c;
  std::string key;
  SLR::Load(key, pubFile);
  pub.setStr(key,mcl::IoArray);
  std::ifstream ifs(samplefile.c_str(),std::ios::binary);
  std::ofstream ofs(datasets.c_str(),std::ios::binary);
  std::vector<int> n;
  size_t size;
  std::string ct;
  ifs >> (*N);
  ifs >> (*d);
  std::cerr << "N:" << (*N) << " d:" << (*d) << std::endl;
  n.resize(*d);
  //data: {y,x_1,...,x_(d-1)}
  for(int i=0;i<(*N);i++){
    for(int j=0;j<(*d);j++){
      ifs >> n[j];
      if(j==0){
	n[j] = 2 * n[j] - 1;
      }else{
	n[j] = n[0] * n[j];
      }
      for(int k=0;k<3;k++){
	if(k-1 == n[j]){
	  pub.enc(c,1,rg);
	}else{
	  pub.enc(c,0,rg);
	}
	ct = c.getStr(mcl::IoArray);
	size = ct.size();
	ofs.write((char *) &size,sizeof(size));
	ofs.write(ct.c_str(),size);
      }
    }
  }
  std::cerr << "encrypted data" << std::endl;
}

void SLR::Server::setParam(std::string& cparam){
  std::ifstream ifs(cparam.c_str(),std::ios::binary);
  ifs.read((char *) &datasize,sizeof(datasize));
  ifs.read((char *) &dimension,sizeof(dimension));
  fd = (Elgamal::CipherText*)malloc(datasize*dimension*3*sizeof(Elgamal::CipherText));
  sd = (Elgamal::CipherText*)malloc(datasize*dimension*2*sizeof(Elgamal::CipherText));
  td = (Elgamal::CipherText*)malloc(dimension*2*sizeof(Elgamal::CipherText));
  if(td == NULL || sd == NULL || td == NULL) std::cerr << "error malloc @setParam()" << std::endl;
  std::cerr << "server set param" << std::endl;
}

void SLR::Server::readPublicKey(std::string& pubfile){
  std::string key; 
  SLR::Load(key, pubfile);
  pub.setStr(key,mcl::IoArray);
  std::cerr << "Server received pubKey" << std::endl;
}

void SLR::Server::dataProcessing(std::string& datasets,std::string& processed){
  std::ifstream ifs(datasets.c_str(),std::ios::binary);
  size_t size;
  char *c;
  c = (char *)malloc(BUFSIZE * sizeof(char));
  for(int i=0;i<datasize*dimension*3;i++){
    ifs.read((char *) &size,sizeof(size));
    if(BUFSIZE < size){
      free(c);
      c = (char *)malloc(size * sizeof(char));
    }
    ifs.read(c,size * sizeof(char));
    std::string ct(c,size);
    fd[i].setStr(ct,mcl::IoArray);
  }
  free(c);
  fd2sd();
  sd2td();
  std::ofstream ofs(processed.c_str(),std::ios::binary);
  std::string str;
  for(int i=0;i<dimension*2;i++){
    str = td[i].getStr(mcl::IoArray);
    size = str.size();
    ofs.write((char *) &size,sizeof(size));
    ofs.write(str.c_str(),sizeof(char) * size);
  }
  std::cerr << "fin processing data" << std::endl;
}

//内積計算で値を変換する
void SLR::Server::fd2sd(){
  for(int i=0;i<datasize;i++){
    for(int j=0;j<dimension;j++){
      // sd[i][j][1]:i人目j次元目x
      sd[i*dimension*2+j*2+1] = fd[i*dimension*3+j*3];
      sd[i*dimension*2+j*2+1].add(fd[i*dimension*3+j*3+2]);
      // sd[i][j][0]:i人目j次元目(2y-1)x
      fd[i*dimension*3+j*3].neg();
      sd[i*dimension*2+j*2] = fd[i*dimension*3+j*3];
      sd[i*dimension*2+j*2].add(fd[i*dimension*3+j*3+2]);
    }
  }
  std::cerr << "fin fd2sd" << std::endl;
}

//総和を計算する
void SLR::Server::sd2td(){
  for(int i=0;i<dimension;i++){
    td[i*2] = sd[i*2];
    td[i*2+1] = sd[i*2+1];
    // td[d][0]:sum{(2y-1)x} , td[d][1]:sum{x}
    for(int j=1;j<datasize;j++){
      td[i*2].add(sd[j*dimension*2+i*2]);
      td[i*2+1].add(sd[j*dimension*2+i*2+1]);
    }
  }
  std::cerr << "fin sd2td" << std::endl;
}

void SLR::Client::setParam(int N,int d){
  datasize = N;
  dimension = d;
  td = (Elgamal::CipherText*)malloc(dimension*2*sizeof(Elgamal::CipherText));
  A.resize(dimension);
  B.resize(dimension*dimension);
  dectd.resize(dimension*2);
  if(td == NULL) std::cerr << "error malloc @Client()" << std::endl;
  prv.setCache(-datasize, datasize);
}

void SLR::Client::makeParam(std::string& cparam){
  std::ofstream ofs(cparam.c_str(),std::ios::binary);
  ofs.write((char *) &datasize,sizeof(datasize));
  ofs.write((char *) &dimension,sizeof(dimension));
  std::cerr << "client set param" << std::endl;
}

void SLR::Client::setKeys(std::string& prvFile,std::string& pubFile){
  const mcl::EcParam& para = mcl::ecparam::secp192k1;
  //const mcl::EcParam& para = mcl::ecparam::secp256k1;
  const Fp x0(para.gx);
  const Fp y0(para.gy);
  const Ec P(x0, y0);
  const size_t bitLen = para.bitSize;
  
  std::string key;

  SLR::Load(key, pubFile);
  pub.setStr(key,mcl::IoArray);
  SLR::Load(key, prvFile);
  prv.setStr(key,mcl::IoArray);

  std::cerr << "Client loaded prv and pub" << std::endl; 
}

void SLR::Client::setData(std::string& processed,std::string& Afile,std::string& Bfile){
  bool b;
  Elgamal::CipherText ct;
  size_t size;
  char *c;
  c = (char *)malloc(BUFSIZE * sizeof(char));
  std::ifstream ifs(processed.c_str(),std::ios::binary);
  for(int i=0;i<dimension*2;i++){
    ifs.read((char *) &size,sizeof(size));
    if(size > BUFSIZE){
      free(c);
      c = (char *)malloc(size * sizeof(char));
    }
    ifs.read(c,sizeof(char)*size);
    std::string str(c,size);
    ct.setStr(str,mcl::IoArray);
    dectd[i] = prv.dec(ct,&b);
    if(!b) std::cerr << "dec error" << std::endl;
  }
  free(c);
#ifdef DEBUG
  std::string samplefile = "../data/test.dat";
  checkSample(samplefile,dectd);
#endif
  setA(Afile);
  setB(Bfile);
}

void SLR::Client::setA(std::string& Afile){
  std::ofstream ofs(Afile.c_str(),std::ios::binary);
  for(int i=0;i<dimension;i++){
    A[i] = dectd[i*2];
    ofs << A[i];
    if(i != dimension -1) ofs << ",";
  }
  ofs << "\n";
  #ifdef DEBUG
  std::cerr << "A" << std::endl;
  for(int i=0;i<dimension;i++){
    std::cerr << A[i] << " ";
  }
  std::cerr << std::endl;
  #endif
}

void SLR::Client::setB(std::string& Bfile){
  std::ofstream ofs(Bfile.c_str(),std::ios::binary);
  for(int i=0;i<dimension;i++){
    for(int j=0;j<=i;j++){
      if(i==j){
	B[i*dimension+j] = (-1) * dectd[i*2+1];
      }else{
	B[i*dimension+j] = B[j*dimension+i] = (double)dectd[i*2+1] * dectd[j*2+1] * (-1) / datasize;
      }
    }
  }
  for(int i=0;i<dimension;i++){
    for(int j=0;j<dimension;j++){
      ofs << B[i*dimension+j];
      if(j != dimension-1) ofs << ",";
    }
    ofs << "\n";
  }
  #ifdef DEBUG
  std::cerr << "B" << std::endl;
  for(int i=0;i<dimension;i++){
    for(int j=0;j<dimension;j++){
    std::cerr << B[i*dimension+j] << " ";
    }
    std::cerr << "\n";
  }
  std::cerr << std::endl;
  #endif
}

void SLR::checkSample(std::string& samplefile,std::vector<int> dectd){
  std::ifstream ifs(samplefile.c_str(),std::ios::binary);
  int N,d;
  std::vector<int> rawdata;
  ifs >> N;
  ifs >> d;
  rawdata.resize(N*d);
  for(int i=0;i<N;i++){
    for(int j=0;j<d;j++){
      ifs >> rawdata[i*d+j];
      std::cerr << rawdata[i*d+j];
    }
    std::cerr << std::endl;
  }
  int a,b;
  //(2y-1)x,x
  for(int i=0;i<d;i++){
    a = 0;
    b = 0;
    for(int j=0;j<N;j++){
      if(i==0){
	a += (2*rawdata[j*d] -1);
	b += 1;
      }else{
	a += (2*rawdata[j*d]-1) * rawdata[j*d+i];
	b += rawdata[j*d+i];
      }
    }
    if(a != dectd[2*i] || b != dectd[2*i+1]) std::cerr << "NG" << std::endl;
    //std::cerr << i << std::endl;
    //std::cerr << a << ":" << dectd[2*i] << " , " << b << ":" << dectd[2*i+1] << std::endl;
  }      
}
