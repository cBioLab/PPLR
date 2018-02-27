#include "clr.h"
#include "comm.h"
#include <fstream>

//#define DEBUG
//#define RANDV
//#define RANDI

#define G1SIZE 64
#define G2SIZE 128
#define GTSIZE 1536

#ifdef DEBUG
SHE::SecretKey master;
#endif

void CLR::Save(const std::string& name,const std::string& s){
  std::ofstream ofs(name.c_str(), std::ios::binary);
  size_t size = s.size();
  ofs.write((char *) &size,sizeof(size));
  ofs.write(s.c_str(),size*sizeof(char));	    
}

void CLR::Load(std::string& s,const std::string& name){
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

void CLR::dataEncryption(std::string& samplefile,int *N,int *d,std::string& datasets,std::string& pubFile){
  std::cerr << "start dataEncryption" << std::endl;
  SHE::PublicKey pub;
  SHE::CipherTextG1 c;
  std::string key;
  Load(key,pubFile);
  pub.setStr(key,mcl::IoFixedSizeByteSeq);
  std::ifstream ifs(samplefile.c_str(),std::ios::binary);
  std::ofstream ofs(datasets.c_str(),std::ios::binary);
  std::vector<int> a;
  size_t size;
  std::string ct;
  ifs >> (*N);
  ifs >> (*d);
  std::cerr << "N:" << (*N) << " d:" << (*d) << std::endl;
  a.resize(*d);
  //data: {y,x_1,...,x_(d-1)}
  for(int i=0;i<(*N);i++){
    for(int j=0;j<(*d);j++){
      ifs >> a[j];
#ifdef DEBUG
      std::cerr << a[j] << " ,";
#endif
    }
#ifdef DEBUG
    std::cerr << std::endl;
#endif    
    for(int j=0;j<(*d);j++){
      if(j==0){
	pub.enc(c,2*a[0]-1,rg);
      }else{
	pub.enc(c,a[j]*(2*a[0]-1),rg);
      }
      ct = c.getStr(mcl::IoFixedSizeByteSeq);
      size = ct.size();
      ofs.write((char *) &size,sizeof(size));
      ofs.write(ct.c_str(),size);
    }
    a[0] = 1;
    for(int j=0;j<(*d);j++){
      for(int k=j;k<(*d);k++){
	pub.enc(c,a[j]*a[k],rg);
	ct = c.getStr(mcl::IoFixedSizeByteSeq);
	size = ct.size();
	ofs.write((char *) &size,sizeof(size));
	ofs.write(ct.c_str(),size);
      }
    }
  }
  std::cerr << "encrypted data" << std::endl;
}

void CLR::Server::setParam(std::string& cparam){
  std::ifstream ifs(cparam.c_str(),std::ios::binary);
  ifs.read((char *) &datasize,sizeof(datasize));
  ifs.read((char *) &dimension,sizeof(dimension));
  ifs.read((char *) &itrmax,sizeof(itrmax));
  ifs.read((char *) &lambda,sizeof(lambda));
  ifs.read((char *) &ac,sizeof(ac));
  ifs.read((char *) &lpara,sizeof(lpara));
  A_raw.resize(datasize*dimension);
  B_raw.resize(datasize*dimension*(dimension+1)/2);
  A.resize(dimension);
  B.resize(dimension*dimension);
  theta.resize(dimension);
  gradient.resize(dimension);
  randvec.resize(dimension);
  std::cerr << "server set param" << std::endl;
}

void CLR::Server::readPublicKey(std::string& pubfile){
  std::string key; 
  CLR::Load(key, pubfile);
  pub.setStr(key,mcl::IoFixedSizeByteSeq);
  std::cerr << "Server received pubKey" << std::endl;
}

void CLR::Server::dataProcessing(std::string& datasets){
  std::ifstream ifs(datasets.c_str(),std::ios::binary);
  size_t size;
  char *c;
  c = (char *)malloc(G1SIZE * sizeof(char));
  for(int n=0;n<datasize;n++){
    for(int i=0;i<dimension;i++){
      ifs.read((char *) &size,sizeof(size));
      if(G1SIZE < size){
	free(c);
	std::cerr << "realloc" << std::endl;
	c = (char *)malloc(size * sizeof(char));
      }
      ifs.read(c,size * sizeof(char));
      std::string ct(c,size);
      A_raw[i+n*dimension].setStr(ct,mcl::IoFixedSizeByteSeq);
    }
    for(int i=0;i<dimension*(dimension+1)/2;i++){
      ifs.read((char *) &size,sizeof(size));
      if(G1SIZE < size){
	free(c);
	std::cerr << "realloc" << std::endl;
	c = (char *)malloc(size * sizeof(char));
      }
      ifs.read(c,size * sizeof(char));
      std::string ct(c,size);
      B_raw[n*dimension*(dimension+1)/2+i].setStr(ct,mcl::IoFixedSizeByteSeq);
    }
  }
  free(c);
  for(int i=0;i<dimension;i++){
    A[i] = A_raw[i];
    for(int n=1;n<datasize;n++){
      A[i].add(A_raw[i+n*dimension]);
    }
    //整数化
    SHE::CipherTextG1::mul(A[i],A[i],(-4)*ac);
  }
#ifdef DEBUG
  for(int i=0;i<dimension;i++){
    std::cout << "A[" << i <<  "] : " << master.dec(A[i]) << std::endl;
  }
#endif
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<dimension;i++){
    for(int j=i;j<dimension;j++){
      B[j+i*dimension] = B_raw[i*dimension+j-i*(i+1)/2];
      for(int n=1;n<datasize;n++){
	B[i*dimension+j].add(B_raw[i*dimension+j-i*(i+1)/2+n*dimension*(dimension+1)/2]);
      }
      if(i != j) B[j*dimension+i] = B[j+i*dimension];
    }
  }
#ifdef DEBUG
  for(int i=0;i<dimension;i++){
    for(int j=0;j<dimension;j++){
      std::cout << "B[" << i << "," << j << "] : " << master.dec(B[i*dimension+j]) << std::endl;
    }
  }
#endif

  std::cerr << "fin processing data" << std::endl;
}

void CLR::Server::calcGradient(std::string& thetaFile,std::string& gradientFile,int itrnum){
  std::cerr << "calcGradient" << std::endl;
  std::ifstream ifs(thetaFile.c_str(),std::ios::binary);
  size_t size;
  char *c;
  c = (char *)malloc(G2SIZE * sizeof(char));
  for(int i=0;i<dimension;i++){
    ifs.read((char *) &size,sizeof(size));
    if(G2SIZE < size){
      free(c);
      std::cerr << "realloc" << std::endl;
      c = (char *)malloc(size * sizeof(char));
    }
    ifs.read(c,size * sizeof(char));
    std::string ct(c,size);
    theta[i].setStr(ct,mcl::IoFixedSizeByteSeq);
  }
  std::vector<SHE::CipherTextM> ctm(dimension);
  std::vector<SHE::CipherTextG2> ctg2(dimension);
  std::vector<SHE::CipherTextG1> ran(dimension);
  //乱数消去処理
  omp_set_num_threads(core);
#pragma omp parallel for
  for(int i=0;i<dimension;i++){
    pub.enc(ctg2[i],randvec[i]/8/lpara,rg);
    theta[i].add(ctg2[i]);
  }
  if(itrnum+1 != itrmax){
    std::cerr << "randomize" << std::endl;
  }
#pragma omp parallel for
  //勾配計算
  for(int i=0;i<dimension;i++){
    pub.convertToCipherTextM(gradient[i],A[i]);
    for(int j=0;j<dimension;j++){
      SHE::CipherTextM::mul(ctm[i],B[i*dimension+j],theta[j]);
      gradient[i].add(ctm[i]);
    }
    SHE::CipherTextG2::mul(ctg2[i],theta[i],lambda);
    pub.convertToCipherTextM(ctm[i],ctg2[i]);
    gradient[i].add(ctm[i]);
#ifdef DEBUG
    std::cerr << "gradient[" << i << "] : " << master.dec(gradient[i]) << std::endl;
#endif
    
    if(itrnum+1 != itrmax){
      int r = randvec[i];
      randvec[i] = rg.get32()%(5*datasize*dimension*ac);
#ifdef RANDV
      randvec[i] = 0;
#endif
      pub.enc(ran[i],(randvec[i]-r),rg);
      pub.convertToCipherTextM(ctm[i],ran[i]);
      gradient[i].add(ctm[i]);
    }else{
      pub.enc(ran[i],randvec[i],rg);
      pub.convertToCipherTextM(ctm[i],ran[i]);
      gradient[i].sub(ctm[i]);
    }    
#ifdef DEBUG
    std::cerr << "gradient'[" << i << "] : " << master.dec(gradient[i]) << std::endl;
#endif
  }

  std::ofstream ofs(gradientFile.c_str(),std::ios::binary);
  std::string str;
  for(int i=0;i<dimension;i++){
    str = gradient[i].getStr(mcl::IoFixedSizeByteSeq);
    size = str.size();
    ofs.write((char *) &size,sizeof(size));
    ofs.write(str.c_str(),sizeof(char) * size);
  }
  free(c);
}

void CLR::Client::setTable(std::string& tableFile){
  std::ifstream ifs(tableFile.c_str(),std::ios::binary);
  SHE::gtHashTbl.load(ifs);
}

void CLR::Client::setParam(int N,int d){
  datasize = N;
  dimension = d;
  theta.resize(dimension);
  gradient.resize(dimension);
  paras.resize(dimension);
  decRange = N * d * ac * 10;
  //SHE::setRangeForGTDLP(decRange); 
#ifdef DEBUG
  master = prv;
  SHE::setRangeForG1DLP(decRange); 
#endif

}

void CLR::Client::makeParam(std::string& cparam){
  std::ofstream ofs(cparam.c_str(),std::ios::binary);
  ofs.write((char *) &datasize,sizeof(datasize));
  ofs.write((char *) &dimension,sizeof(dimension));
  ofs.write((char *) &itrmax,sizeof(itrmax));
  ofs.write((char *) &lambda,sizeof(lambda));
  ofs.write((char *) &ac,sizeof(ac));
  ofs.write((char *) &lpara,sizeof(lpara));
  std::cerr << "client set param" << std::endl;
}

void CLR::Client::setKeys(std::string& prvFile,std::string& pubFile){
  prv.setByCSPRNG(rg);
  prv.getPublicKey(pub);
  CLR::Save(prvFile, prv.getStr(mcl::IoFixedSizeByteSeq));
  CLR::Save(pubFile, pub.getStr(mcl::IoFixedSizeByteSeq));
  std::cerr << "Client created prv and pub" << std::endl; 
}

void CLR::Client::loadKeys(std::string& prvFile,std::string& pubFile){
  std::string key; 
  CLR::Load(key, pubFile);
  pub.setStr(key,mcl::IoFixedSizeByteSeq);

  CLR::Load(key, prvFile);
  prv.setStr(key,mcl::IoFixedSizeByteSeq);

  std::cerr << "Client sets prv and pub" << std::endl; 
}


void CLR::Client::initTheta(std::string& thetaFile){
  std::cerr << "initTheta" << std::endl;
  std::ofstream ofs(thetaFile.c_str(),std::ios::binary);
  size_t size;
  int random;
  for(int i=0;i<dimension;i++){
    random = rg.get32()%(2*ac-1)-(ac-1);
#ifdef RANDI
    random = 0;
#endif
    paras[i] = random;
    std::cerr << (double)paras[i]/ac << " , ";
    pub.enc(theta[i],random,rg);
    std::string ct = theta[i].getStr(mcl::IoFixedSizeByteSeq);
    size = ct.size();
    ofs.write((char *) &size,sizeof(size));
    ofs.write(ct.c_str(),size);
  }
  std::cerr << std::endl;
}

void CLR::Client::calcTheta(std::string& gradientFile,std::string& thetaFile,std::string& resultFile,int itr){
  std::cerr << "calcTheta" << std::endl;
  std::ifstream ifs(gradientFile.c_str(),std::ios::binary);
  size_t size;
  char *c;
  c = (char *)malloc(GTSIZE * sizeof(char));
  for(int i=0;i<dimension;i++){
    ifs.read((char *) &size,sizeof(size));
    if(GTSIZE < size){
      free(c);
      c = (char *)malloc(size * sizeof(char));
    }
    ifs.read(c,size * sizeof(char));
    std::string ct(c,size);
    gradient[i].setStr(ct,mcl::IoFixedSizeByteSeq);
  }

  std::cerr << "start updating thetas" << std::endl;
  //パラメータの更新
  int g;
  std::ofstream ofs(thetaFile.c_str(),std::ios::binary);
  for(int i=0;i<dimension;i++){
    g = prv.dec(gradient[i]);
    g = g / 8 / lpara;
    paras[i] -= g;
    pub.enc(theta[i],paras[i],rg);
    std::cerr << (double)paras[i]/ac << " , ";
    std::string ct = theta[i].getStr(mcl::IoFixedSizeByteSeq);
    size = ct.size();
    ofs.write((char *) &size,sizeof(size));
    ofs.write(ct.c_str(),size);
  }
  std::cerr << std::endl;
  std::cerr << "end updating thetas" << std::endl;
  if(itr+1 == itrmax){
    std::ofstream ofs(resultFile.c_str(),std::ios::binary);
    for(int i=0;i<dimension;i++){
      ofs << (double)paras[i]/ac;
      if(i+1!=dimension) ofs << ",";
    }
    ofs << std::endl;
  }
}
