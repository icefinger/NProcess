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
#include <thread>
#include <atomic>
#include <mutex>

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
      unsigned int GetProcessId() {return __current_process_id;}
      unsigned int GetStep() {return __step__;}

    public:
      virtual void PreProcess () {}
      virtual void Process () = 0;
      virtual void PostProcess () {}
      virtual void Run () = 0;
      void Lock (const std::string& key_) {__sgt->Lock (key_);}
      void Unlock (const std::string& key_) {__sgt->Unlock (key_);}

    private:
      static std::atomic <size_t> __processes_id__;
      size_t __current_process_id;
      std::atomic <size_t> __step__;
    };

  public:
    void Flush ();
    void SetDebug (bool);
    void SetNbProc(int nb_=-1);
    int GetNbProc();
    int GetNbRunning() {return __nb_running;}
    void PrepareLock(const std::string&);
    void Lock(const std::string&);
    void Unlock(const std::string&);
    void ProcessAll (unsigned int step_=0);
    void ForStep (bool &next_, std::list<unsigned int> step_list_);
    void ForStep (bool &next_, unsigned int start_, unsigned int stop_);
    void WaitRunning ();
    void WaitThis (Object*);
    void KillThis (Object*);
    const std::list<Object*>& GetObjects() const {return __obj_lst;}
    size_t GetNbObject () const { return __obj_lst.size ();}

    static void ProcessThis(Object* obj_);
    static void RunThis (Object* obj_);

  private:
    //static void RunGarbageCollector (Object*);
    void ProcessGarbageCollector ();
    void Init ();
    void Add(Object*);
    NProcess();
    virtual ~NProcess();

  private:
    std::atomic <int> __count;
    bool __debug;
    bool __init;
    std::atomic <int> __nb_thread;
    std::atomic<int> __nb_running;

    std::mutex __nb_thread_counter_mtx;
    std::mutex __locker_mtx;

    std::map<std::string, std::mutex*> __mutex_lst;
    std::list<Object*> __obj_lst;
    std::map <Object*,std::thread*> __run_thread;
    std::list<std::thread*> __threads;
  };


}
#endif
