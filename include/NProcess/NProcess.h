/*
Hugon Christophe 2011.
Under GPL. See "COPYING.txt"
 */


#ifndef __ICEDCODE__NPROCESS_H__
#define __ICEDCODE__NPROCESS_H__

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
    /**
     * Object to be used by inheritence for the multithread processes.
     *
     */
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
      /**
       * Process method correspond to the method that will be executed in parrallel
       * The user have to implement it, even with something void if needed.
       * It can be executed more than once, the user have to use his own mutexes to
       * wait for it.
       */
      virtual void Process () = 0;
      virtual void PostProcess () {}
      /**
       * Run method correspond to the method that will be executed in parrallel
       * The user have to implement it, even with something void if needed.
       * It can be executed only once and the user can use the WaitForIt method to wait it.
       */
      virtual void Run () = 0;
      /**
       * Easy to use internal mutex method to be used in Run and Process for multi-theading safety.
       * Slower than direct mutex usage, it uses a dictionary to classify the mutexes by user names.
       * It's the dictionary from the singleton NProcess.
       * This method lock the mutex pointed by key_.
       * @param key_ string naming the mutex.
       */
      void Lock (const std::string& key_) {__sgt->Lock (key_);}
      /**
       * Easy to use internal mutex method to be used in Run and Process for multi-theading safety.
       * Slower than direct mutex usage, it uses a dictionary to classify the mutexes by user names.
       * It's the dictionary from the singleton NProcess.
       * This method unlock the mutex pointed by key_, or just create the mutex if it does not exist.
       * @param key_ string naming the mutex.
       */
      void Unlock (const std::string& key_) {__sgt->Unlock (key_);}

    private:
      static std::atomic <size_t> __processes_id__;
      size_t __current_process_id;
      std::atomic <size_t> __step__;
    };

  public:
    /**
     * Remove all user objects from the running list without deleting them.
     */
    void Flush ();
    void SetDebug (bool);
    void SetNbUsedProc(int nb_=-1);
    int GetNbProc();
    /**
     * @return the threads currently running the Run methods
     */
    int GetNbRunning() {return __nb_running;}
    /**
     * Prepare the mutex with the given name.
     * @return mutex directly usable by the user.
     */
    std::mutex* PrepareMutex(const std::string&);
    /**
     * Easy to use internal mutex method to be used in Run and Process for multi-theading safety.
     * Slower than direct mutex usage, it uses a dictionary to classify the mutexes by user names.
     * This method lock the mutex pointed by key_.
     * @param key_ string naming the mutex.
     */
    void Lock(const std::string& key_);
    /**
     * Easy to use internal mutex method to be used in Run and Process for multi-theading safety.
     * Slower than direct mutex usage, it uses a dictionary to classify the mutexes by user names.
     * This method unlock the mutex pointed by key_, or just create the mutex if it does not exist.
     * @param key_ string naming the mutex.
     */
    void Unlock(const std::string& key_);
    /**
     * Launch all the object Process method in parallel (with a maximum of simultaneous number of used process). It can run only the object with given step number, which is a kind of user id. It waits the end of all the processes
     *
     * @param step_ id number to differentiate the object to processes
     */
    void ProcessAll (unsigned int step_=0);
    void ForStep (bool &next_, std::list<unsigned int> step_list_);
    void ForStep (bool &next_, unsigned int start_, unsigned int stop_);
    /**
     * Wait for all the running objects
     */
    void WaitRunning ();
    /**
     * Wait only for the given object running to finish.
     */
    void WaitThis (Object*);
    /**
     * Stop the running object, without deleting it.
     */
    void KillThis (Object*);
    const std::list<Object*>& GetObjects() const {return __obj_lst;}
    size_t GetNbObject () const { return __obj_lst.size ();}

    /**
     * Process only one object.
     *
     * @param obj_
     */
    static void ProcessThis(Object* obj_);
    /**
     * launch a thread for an object.
     * @param obj_ to be run.
     */
    void AssynchRunThis (Object* obj_);
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
