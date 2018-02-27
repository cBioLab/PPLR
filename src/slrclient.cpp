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
  string cparam = "../comm/client/param.dat";
  string processed = "../comm/client/pdata.dat";
  string Afile = "../comm/client/A.csv";
  string Bfile = "../comm/client/B.csv";

  string hostname = "localhost";
  int opt;
  int port = 12345;
  while((opt = getopt(argc, argv, "p:h:")) != -1){
    switch(opt){
    case 'p':
      {
	port = atoi(optarg);
	break;
      }
    case 'h':
      {
	hostname = optarg;
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

  int sock = prepCSock((char *)hostname.c_str(),port);

  SLR::Client c;
  c.setKeys(prvFile,pubFile);

  ifstream ifs("../data/datapara.dat",std::ios::binary);
  ifs >> N;
  ifs >> d;

  s_time = get_wall_time();
  c.setParam(N,d);
  c.makeParam(cparam);
  cerr << "init @client : " << get_wall_time() - s_time << "s" << endl;

  sendFile(sock,(char *)cparam.c_str());
  recvFile(sock,(char *)processed.c_str());

  s_time = get_wall_time();
  c.setData(processed,Afile,Bfile);
  cerr << "setData @client : " << get_wall_time() - s_time << "s" << endl;

  return 0;
}
