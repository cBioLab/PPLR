#include "clr.h"
#include "comm.h"
#include <sys/time.h>

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
  //string prvFile = "../data/prvsample.dat";
  //string samplefile = "../data/test.dat";
  string datasets = "../data/encdata.dat";
  string cparam = "../comm/server/param.dat";
  string thetaFile = "../comm/server/theta.dat";
  string gradientFile = "../comm/server/gradient.dat";

  //-f datafile -a accuracy -l learning rate -r coefficient of regularization -i number of iterations
  int opt;
  int sock,port;
  int core;
  core = 1;
  port = 12345;
  while((opt = getopt(argc, argv, "p:c:")) != -1){
    switch(opt){
    case 'p':
      {
	port = atoi(optarg);
	break;
      }
    case 'c':
      {
	core = atoi(optarg);
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

  sock = prepSSock(port);
  sock = acceptSSock(sock);

  recvFile(sock,(char *)cparam.c_str());

  double initt;
  time = get_wall_time();
  CLR::Server s(core);
  s.setParam(cparam);
  s.readPublicKey(pubFile);
  initt = get_wall_time()-time;

  double proct;
  time = get_wall_time();
  s.dataProcessing(datasets);
  proct = get_wall_time()-time;

  cout << "start secure gradient decent" << endl;
  for(int i=0;i<s.itrmax;i++){
    cout << "server : iteration(" << i << ")" << endl;
    recvFile(sock,(char *)thetaFile.c_str());
    s.calcGradient(thetaFile,gradientFile,i);
    sendFile(sock,(char *)gradientFile.c_str());
  }
  cout << "init @server : " << initt << "s" << endl;
  cout << "procData @server : " << proct << "s" << endl;
  return 0;
}
