#include "clr.h"
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
  double time;

  SHE::init();

  int N,d;
  double s_time,e_time;
  string pubFile = "../data/pubsample.dat";
  string prvFile = "../data/prvsample.dat";
  string samplefile = "../data/test.dat";
  string datasets = "../comm/client/encdata.dat";
  string cparam = "../comm/client/param.dat";
  string thetaFile = "../comm/client/theta.dat";
  string gradientFile = "../comm/client/gradient.dat";
  string resultFile = "../comm/client/result.dat";
  string tableFile = "../data/tableForDec.dat";

  //-f datafile -a accuracy -l learning rate -r coefficient of regularization -i number of iterations
  int opt;
  int ac,itr;
  double cr,lr;
  ac = 3; itr = 10; cr = 0.5; lr = 0.01;
  string hostname = "localhost";
  int sock,port;
  int sendsize,recvsize;
  sendsize = recvsize = 0;
  port = 12345;
  N = d = 0;
  while((opt = getopt(argc, argv, "a:l:r:i:h:p:")) != -1){
    switch(opt){
    case 'a':
      {
	ac = atoi(optarg);
	break;
      }
    case 'l':
      {
	lr = atof(optarg);
	break;
      }
    case 'r':
      {
	cr = atof(optarg);
	break;
      }
    case 'i':
      {
	itr = atoi(optarg);
	break;
      }
    case 'h':
      {
	hostname = optarg;
	break;
      }
    case 'p':
      {
	port = atoi(optarg);
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

  ifstream ifs("../data/datapara.dat",ios::binary);
  ifs >> N;
  ifs >> d;

  sock = prepCSock((char *)hostname.c_str(),port);

  double initt;
  time = get_wall_time();
  CLR::Client c(ac,lr,cr,itr);
  c.loadKeys(prvFile,pubFile);
  c.setTable(tableFile);
  c.setParam(N,d);
  c.makeParam(cparam);
  c.initTheta(thetaFile);
  initt = get_wall_time()-time;

  sendsize+=sendFile(sock,(char *)cparam.c_str());

  cout << "start secure gradient decent" << endl;
  double sgdt;
  time = get_wall_time();
  for(int i=0;i<c.itrmax;i++){
    cout << "client : iteration(" << i << ")" << endl;
    sendsize+=sendFile(sock,(char *)thetaFile.c_str());
    recvsize+=recvFile(sock,(char *)gradientFile.c_str());
    c.calcTheta(gradientFile,thetaFile,resultFile,i);
  }
  sgdt = get_wall_time()-time;
  cout << "init @client : " << initt << "s" << endl;
  cout << "secure gradient descent : " << sgdt << "s" << endl;
  cout << "communication size : " << (double)(sendsize+recvsize)/(1024*1024) << "MB" << std::endl;


  return 0;
}
