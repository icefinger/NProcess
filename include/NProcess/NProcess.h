/*
Hugon Christophe 2011.
Under GPL. See "COPYING.txt"
 */


#ifndef NALC_PROC
#define NALC_PROC

#include <list>
#include <vector>
#include <map>
#include <string>
#include <pthread.h>

#include <NSingle.h>


namespace icedcode {

  class NProcess : public NSingle<NProcess>{
  public:
    friend class NSingle<NProcess>;

  public:

    class Object {
    public:
      Object ();
      virtual ~Object ();

    public:
      Object (const Object&);
      Object& operator= (const Object&);
      unsigned int GetProcessId() {return __process_id__;}
      unsigned int GetStep() {return __step__;}

    public:
      virtual void PreProcess () {}
      virtual void Process () = 0;
      virtual void PostProcess () {}
      virtual void Run () = 0;

    private:
      static unsigned int __process_id__;
      unsigned int __step__;
    };

  public:
    void Flush ();
    void SetDebug (bool);
    void SetNbProc(int nb_=-1);
    int GetNbProc();
    int GetNbRunning() {return __nb_running;}
    void Lock(std::string);
    void Unlock(std::string);
    void ProcessAll (unsigned int step_=0);
    void ForStep (bool &next_, std::list<unsigned int> step_list_);
    void ForStep (bool &next_, unsigned int start_, unsigned int stop_);
    void WaitRunning (float sec_=-1);
    void WaitThis (Object*,float sec_=-1);
    void RunThis (Object*);
    void KillThis (Object*);
    const std::list<Object*>& GetObjects() {return __obj_lst;}

  private:
    void Init ();
    void Add(Object*);
    static void* ProcessThis(void* obj);
    static void* RunThis(void* obj);
    NProcess();
    ~NProcess();

  private:
    int __count;
    bool __debug;
    bool __init;
    int __nb_thread;
    static int __nb_running;

    pthread_mutex_t __main_mutex;
    pthread_mutex_t __map_mutex;
    pthread_mutex_t __count_mutex;
    pthread_mutex_t __launcher_mutex;
    pthread_mutex_t __join_mutex;
    pthread_cond_t __count_threshold_cv;
    pthread_attr_t __attr;

    std::map<std::string, pthread_mutex_t> __mutex_lst;

    std::list<Object*> __obj_lst;
    static std::map <Object*,pthread_t> __run_thread;
    std::vector<pthread_t> __threads;
  };


}
#endif
