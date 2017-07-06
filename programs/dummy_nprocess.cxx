// -*- mode: c++ ; -*-
// test_dummy_icedcode.cxx

#define nproc icedcode::NProcess::GetIt()

#include <iostream>
#include <unistd.h>
#include <cmath>

#include <NProcess.h>

//first exemple : how to launch nproc process from an User object.

int value=15;

using namespace std;

class heritis : public icedcode::NProcess::Object {
public:
  heritis () {}
  ~heritis () {}
protected:
  void Run () {}
  void Process () {
    double a_result;
    for (int i=0;i<1000000000;i++)
      for (int j=0;j<1000000000;j++)
	a_result=sin(cos(tan(value)));
    cout <<" sin(cos(tan("<< value <<")))="<<a_result<<" ";cout.flush();
  }

};

int main()
{

  cout<<  "you have "<<icedcode::NProcess::GetIt()->GetNbProc()<<" processor."<<endl;
  heritis h[100];
  icedcode::NProcess::GetIt()->ProcessAll();
  return 0;
}
