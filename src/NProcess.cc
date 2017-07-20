#include <NProcess.h>
#include <algorithm>
#include <sys/sysinfo.h>
#include <unistd.h>


namespace icedcode {

  using namespace std;

  NProcess::NProcess () {
  }

  NProcess::~NProcess () {
  }

  void NProcess::Init () {
    __nb_thread=get_nprocs ();
    __count=0;
  }

  void NProcess::Flush () {
    cout << "destroying" << endl;

    for(auto& it: __run_thread)
      {
	clog << "NProcess::Flush:WARNING: Running objects are about to be killed. Try to stop them before flushing or destroying NProcess.\n";
	KillThis(it.first);
      }
    ProcessGarbageCollector ();
    __obj_lst.clear();
  }

  void NProcess::SetNbProc (int nb_) {
    if (nb_<=0) __nb_thread=get_nprocs ();
    else __nb_thread=nb_;
  }
  int NProcess::GetNbProc () {
    return __nb_thread;
  }
  void NProcess::Add(Object* obj_){
    __obj_lst.push_back(obj_);
    __obj_lst.sort();
    __obj_lst.unique();
  }

  void NProcess::SetDebug (bool debug_) {
    __debug=debug_;
  }

  void NProcess::ProcessThis(Object *obj_){

    __sgt->__count++;

    if (__sgt->__count<__sgt->GetNbProc())
      __sgt->__nb_thread_counter_mtx.unlock ();

    obj_->Process();

    __sgt->__count--;


    if (__sgt->__count<__sgt->GetNbProc())
      __sgt->__nb_thread_counter_mtx.unlock ();

  }

  void NProcess::KillThis(Object* obj_){
    if (!__run_thread.count(obj_)){
      clog << "WARNING: Object isn't running.\n";
      return;
    }

    __run_thread.erase (obj_);
    __nb_running--;
  }

  void NProcess::RunThis(Object* obj_){
    if (__sgt->__run_thread.count(obj_)){
      clog << "WARNING: Object already running.\n";
      return;
    }

    __sgt->__nb_running++;
    __sgt->__run_thread[obj_]=new thread(RunThis, obj_);
  }

  void NProcess::PrepareLock(const std::string& key_)
  {
    __locker_mtx.lock ();
    if(!__mutex_lst.count(key_))
      {
        __mutex_lst[key_]=new mutex;
      }
    __locker_mtx.unlock ();
  }

  void NProcess::Lock(const string& key_){
    PrepareLock (key_);
    __mutex_lst[key_]->lock ();
  }

  void NProcess::Unlock (const string& key_){
    __locker_mtx.lock ();
    if(!__mutex_lst.count(key_))
      {
        cerr << "NProcess::Unlock ERROR: " << key_ << " does not exist among mutexes, nothing unlock." << endl;
        return;
      }
    __locker_mtx.unlock ();
    __mutex_lst[key_]->unlock ();
  }

  void NProcess::ProcessAll (unsigned int step_){
    for (auto oit: __obj_lst)
      {
        if (step_ != oit->GetStep()) continue;
        __threads.push_back(new thread(ProcessThis, oit));

        if (__sgt->__count >= __sgt->__nb_thread)
          __nb_thread_counter_mtx.lock ();
      }

    for (auto tit: __threads)
      tit->join ();
    ProcessGarbageCollector ();
  }


  void NProcess::ForStep (bool &next_, unsigned int start_, unsigned stop_)
  {
    while (next_)
      {
	for (unsigned int it=start_; it <=stop_; it++)
	  ProcessAll(it);
      }
  }

  void NProcess::ForStep (bool &next_, list<unsigned int> list_)
  {
    while (next_)
      {
	for (list<unsigned int>::iterator it=list_.begin(); it != list_.end(); it++)
	  ProcessAll(*it);
      }
  }

  void NProcess::WaitRunning () {
    for (auto& thit:__run_thread)
      thit.second->join ();
  }

  void NProcess::WaitThis (Object* obj_) {
    __run_thread[obj_]->join ();
  }

  void NProcess::ProcessGarbageCollector ()
  {
    for (auto thit: __threads)
      {
        delete (thit);
      }
    __threads.clear ();
  }

  atomic<size_t> NProcess::Object::__processes_id__(0);


  NProcess::Object::Object (){
    icedcode::NProcess::GetIt()->Add(this);
    __processes_id__++;
    __current_process_id=__processes_id__;
    __step__=0;
  }
  NProcess::Object::~Object (){
    list<Object*>::iterator pos=find (__sgt->__obj_lst.begin(), __sgt->__obj_lst.end(),this);
    __sgt->__obj_lst.erase(pos);
  }

  NProcess::Object::Object (const Object& obj_){
    *this = obj_;
    __sgt->Add(this);
    __processes_id__++;
    __current_process_id=__processes_id__;
  }
  NProcess::Object& NProcess::Object::operator= (const Object& obj_){
    *this = obj_;
    __sgt->Add(this);
    __processes_id__++;
    __current_process_id=__processes_id__;
    return *this;
  }

}
