// -*- mode: c++ ; -*-
// test_dummy_icedcode.cxx

#define nproc icedcode::NProcess::GetIt()

#include <iostream>
#include <unistd.h>
#include <cmath>

#include <chrono>
#include <random>

#include <NProcess.h>

//first exemple : how to launch nproc process from an User object.

using namespace std;

int value=15;

class heritis : public icedcode::NProcess::Object {
public:
  heritis () {}
  ~heritis () {}
protected:
  void Run () {}
  void Process () {
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
    Lock ("cout");
    cout << "running process " << GetProcessId()<< endl;
    Unlock ("cout");
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution(0.0,1.0);
    generator.seed (seed1);
    size_t max = 50000000*distribution(generator);
    for (size_t i=0;i<max;i++)
      {
        a_result+=generator ();
      }
  }

public:
  double a_result=0;
private:
  static mutex __locker_mtx;

};

mutex heritis::__locker_mtx;

int main()
{
  {
    cout<<  "you have "<<icedcode::NProcess::GetIt()->GetNbProc()<<" processor."<<endl;
    heritis h[100];
    icedcode::NProcess::GetIt()->ProcessAll();
    cout << "after processall" << endl;
    for (size_t i=0; i<100; i++)
      cout << h[i].GetProcessId () << " "<<h[i].a_result << endl;
  }
  return 0;
}
