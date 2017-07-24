#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <cmath>
#include <fstream>
#include <NProcess.h>
#include <list>

pthread_mutex_t a_thread[3];

#define nproc icedcode::NProcess::GetIt()

//fourth exemple

using namespace std;

typedef struct {
  double val1;
  double val2;
} data_struct;

class event {
public:
  event() { }
  ~event () { };

  list<data_struct> data_set;
  list<data_struct> ana_set;
};

class reader : public icedcode::NProcess::Object {
public:
  reader (event *evt_=NULL, string path_="resources/datafile.dat") {

    if (evt_ == NULL)
      exit (1);
    __evt=evt_;

    __input.open(path_.data());
    if (!__input.is_open())
      {
	cerr << "ERROR: resources/datafile.dat does not exist.\n";
	exit (1);
      }
  }
  ~reader () {}

  bool has_next ()
  {
    return !__input.eof();
  }


  void Run () {}
  void Process () {
    int nb_it=0;
    data_struct val;
    list<data_struct> raw_set;
    while(__input >> val.val1 >> val.val2 && nb_it < 10)
      {
	nb_it++;
	raw_set.push_back(val);
      }
    nproc->Lock ("data_set");
    __evt->data_set.clear ();
    __evt->data_set=raw_set;
    nproc->Unlock("data_set");

  }

private:
  fstream __input;
  event *__evt;
};


class analyser : public icedcode::NProcess::Object {
public:
  analyser (event *evt_=NULL) {
    if (evt_ == NULL)
      exit (1);
    __evt=evt_;
  }
  ~analyser () {}

  void Run () {}
  void Process () {

    nproc->Lock ("data_set");
    for (list<data_struct>::iterator it=__evt->data_set.begin(); it!= __evt->data_set.end(); it++)
      {
	(*it).val1=sqrt((*it).val1);
	(*it).val2=sqrt((*it).val2);
      }
    nproc->Unlock("data_set");



    nproc->Lock("ana_set");
    __evt->ana_set.clear();
    __evt->ana_set=__evt->data_set;
    nproc->Unlock("ana_set");
  }

private:
  fstream __input;
  event *__evt;

};


class writer : public icedcode::NProcess::Object {
public:
  writer (event *evt_=NULL, string path_="resources/anafile.dat") {

    if (evt_ == NULL)
      exit (1);
    __evt=evt_;

    __output.open(path_.data(),fstream::out);
    if (!__output.is_open())
      {
	cerr << "ERROR: resources/anafile.dat is not accessible.\n";
	exit (1);
      }
  }
  ~writer () {}

public:
  void Run () {}
  void Process () {

    nproc->Lock ("ana_set");
    for (list<data_struct>::iterator it=__evt->ana_set.begin(); it!= __evt->ana_set.end(); it++)
      {
	__output << (*it).val1 << ' ' << (*it).val2 << endl;
      }
    nproc->Unlock("ana_set");

  }


private:
  fstream __output;
  event *__evt;

};


int main()
{
  event evt;
  reader r(&evt);
  analyser a(&evt);
  writer w(&evt);

  int nb=0;

  nproc->SetNbUsedProc(1);

  pthread_mutex_init(&a_thread[0], NULL);
  pthread_mutex_init(&a_thread[1], NULL);
  pthread_mutex_init(&a_thread[2], NULL);

  while (r.has_next ())
    {
      if (nb%1000 == 0)
        {
          cout << "\r"<<nb;cout.flush ();
        }

      //r.Process();
      //a.Process();
      //w.Process();
      nproc->ProcessAll();//ForStep(has_next,0,0);
      nb++;
    }

  cout << endl;

}
