# NProcess
A pthread base multi-threading library

## Principle
The idea of this library is to use the polymorphism of the object oriented c++ to make it easier and more coherent way to do multi-threading. The object to be multi-threaded has to inherit from the NProcess::Object and will benefit of two virtual methods Process and Run. The process is used for series calculus, as many thread as object and can be launched more than once. The run is use for asynchronous processes.

## Compile
The NProcess library does not have any dependencies, and use c++11 standards. So just do:

    mkdir build
    cd build
    cmake ..
    make

## Basic how to use
The examples in programs shows how to basically use. dummy_nprocess is the basic multi-process use, dummy_nprocess_run is for assynchronous and dummy_locker and dummy_share_memory_nprocess are for built in lock mutexes. The dummy_data_analyser is to show how to use it for speeding up an analysis because you have to present some results and the calculus center is not available, the true origin of this library :D

### Parallel basic process
First of all you have to inherit the class you want to be multithreaded:

    class heritis : public icedcode::NProcess::Object {
    public:
      heritis () {}
      ~heritis () {}
    protected:
      void Run () {} //useless, just keep it empty
      void Process () {
        unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();
        cout << "running process " << GetProcessId()<< endl;
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution(0.0,1.0);
        generator.seed (seed1);
        size_t max = 50000000*distribution(generator);
        for (size_t i=0;i<max;i++)
            a_result+=generator ();
      }

    public:
      double a_result=0;
    };

and then create as many processes as objects:

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

### Assynchronous process
To use the assynchronous continuous process, just use the Run () method instead. See documentation for more details.






