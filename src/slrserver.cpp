#include "slr.h"
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
  ROT::SysInit();

  int N,d;
  double s_time,e_time;
  string pubFile = "../data/slrpub.dat";
  string datasets = "../data/encdata.dat";
  string cparam = "../comm/server/param.dat";
  string processed = "../comm/server/pdata.dat";

  int opt;
  int port = 12345;
  while((opt = getopt(argc, argv, "p:")) != -1){
    switch(opt){
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
  int sock = prepSSock(port);
  sock = acceptSSock(sock);

  SLR::Server s;


  recvFile(sock,(char *)cparam.c_str());
  s_time = get_wall_time();
  s.setParam(cparam);
  s.readPublicKey(pubFile);
  cerr << "init @server : " << get_wall_time() - s_time << "s" << endl;

  //ここまで前処理
  s_time = get_wall_time();
  s.dataProcessing(datasets,processed);
  cerr << "procData @server : " << get_wall_time() - s_time << "s" << endl;

  sendFile(sock,(char *)processed.c_str());

  return 0;
}
