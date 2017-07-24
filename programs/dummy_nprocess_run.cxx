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

  string name;
protected:
  void Process () {}
  void Run () {
    cout << name<<endl;cout.flush();sleep(1);
    for (int i=0;i<10;i++){
      cout << "running the loop " << i << endl;
      usleep (1e7);
    }
  }

};

int main()
{

  cout<<  "you have "<<icedcode::NProcess::GetIt()->GetNbProc()<<" processor."<<endl;
  heritis h[2];
  h[0].name="num1 ";
  h[1].name="num2 ";
  icedcode::NProcess::GetIt()->RunThis(&h[0]);
  sleep (1);
  icedcode::NProcess::GetIt()->KillThis (&h[0]);
  //sleep (1);
  cout << endl << endl;
  //sleep (1);
  icedcode::NProcess::GetIt()->RunThis(&h[1]);
  icedcode::NProcess::GetIt()->WaitRunning();
  cout.flush();
  return 0;
}
