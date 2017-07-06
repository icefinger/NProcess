// -*- mode: c++ ; -*-
// test_dummy_icedcode.cxx

#define nproc icedcode::NProcess::GetIt()

#include <iostream>
#include <unistd.h>
#include <cmath>

#include <NProcess.h>

//second exemple : how to share memory. A nproc->Lock method is under development.
int incr=1;
int value=13;
double result=2e+250;//this value is static to be shared between all of of the processes. It can be a pointer inside the object, with a common address for all of them.

using namespace std;

class heritis : public icedcode::NProcess::Object {
public:
  heritis () {
    a=incr++;
    pthread_mutex_init(&__mutex, NULL);
  }
  ~heritis () {}
protected:
  void Run () {}
  void Process () {
    double a_result=0.;
    for (int i=0;i<1000000000;i++)
      for (int j=0;j<100000000;j++)//just to slow down :p
      a_result=sin(cos(tan(result)));//value is only read by the threads, no need of Lock. a_result is a "local" temporary double.
    pthread_mutex_lock (&__mutex);
    //nproc->Lock("a_key");//result is under writting, so it should be locked with a "key"
    cout <<"in the thread"<<' '<<a<<' '<<" result was "<<result<<" before.\n";cout.flush();
    a_result=pow(result,0.9)+a_result*a_result;
    result=a_result;
    cout <<"in this thread result is "<<result<<" after.\n\n";cout.flush();
    //nproc->Unlock("a_key");//then unlocked with the same key, else all will be stuck after the first thread.
    pthread_mutex_unlock (&__mutex);
  }
private:
  int a;
  pthread_mutex_t __mutex;
};

int main()
{
  cout<<  "you have "<<nproc->GetNbProc()<<" processor."<<endl;//nproc->SetNbProc(n) to set n threads.
  nproc->SetNbProc(3);
  heritis h[1000];
  nproc->ProcessAll(); //short cut of NProcess::GetIt()->ProcessAll()
  return 0;
}
