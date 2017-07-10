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
    pthread_attr_init(&__attr);
    pthread_attr_setdetachstate(&__attr, PTHREAD_CREATE_JOINABLE);
    pthread_cond_init (&__count_threshold_cv, NULL);
    pthread_mutex_init(&__main_mutex, NULL);
    pthread_mutex_init(&__count_mutex, NULL);
    pthread_mutex_init(&__launcher_mutex, NULL);
    pthread_mutex_init(&__map_mutex, NULL);
    pthread_mutex_init(&__join_mutex, NULL);
  }

  void NProcess::Flush () {
    cout << "destroying" << endl;
    pthread_attr_destroy(&__attr);
    pthread_cond_destroy(&__count_threshold_cv);
    pthread_mutex_destroy(&__main_mutex);
    pthread_mutex_destroy(&__count_mutex);
    pthread_mutex_destroy(&__launcher_mutex);
    pthread_mutex_destroy(&__map_mutex);
    pthread_mutex_destroy(&__join_mutex);

    for(map<Object*,pthread_t>::iterator it=__run_thread.begin(); it!=__run_thread.end();it++)
      {
	clog << "NProcess::Flush:WARNING: Running objects are about to be killed. Try to stop them before flushing or destroying NProcess.\n";
	KillThis(it->first);
      }
    __obj_lst.clear();
    __threads.clear();
  }

  void NProcess::SetNbProc (int nb_) {
    if (nb_<=0) __nb_thread=get_nprocs ();
    else __nb_thread=nb_;
  }
  int NProcess::GetNbProc () {
    return __nb_thread;
  }
  void NProcess::Add(Object* obj_){
    pthread_t a_thread;
    __obj_lst.push_back(obj_);
    __obj_lst.sort();
    __obj_lst.unique();
    while (__obj_lst.size() != __threads.size())
      __threads.push_back(a_thread);
  }

  void NProcess::SetDebug (bool debug_) {
    __debug=debug_;
  }

  void* NProcess::ProcessThis(void *obj_){

    pthread_mutex_trylock (&__sgt->__join_mutex);
    pthread_mutex_lock(&__sgt->__count_mutex);
    __sgt->__count++;
    if (__sgt->__count<__sgt->GetNbProc())
        pthread_mutex_unlock(&__sgt->__launcher_mutex);//pthread_cond_signal(&__sgt->__count_threshold_cv);

    pthread_mutex_unlock(&__sgt->__count_mutex);

    ((NProcess::Object*)(obj_))->Process();

    pthread_mutex_lock(&__sgt->__count_mutex);
    __sgt->__count--;
    if (__sgt->__count<__sgt->GetNbProc())
        pthread_mutex_unlock(&__sgt->__launcher_mutex);//pthread_cond_signal(&__sgt->__count_threshold_cv);
    if (__sgt->__count == 0)
        pthread_mutex_unlock (&__sgt->__join_mutex);
    pthread_mutex_unlock(&__sgt->__count_mutex);
    pthread_exit(NULL);
  }

  void NProcess::KillThis(Object* obj_){
    if (!__run_thread.count(obj_)){
      clog << "WARNING: Object isn't running.\n";
      return;
    }

    pthread_cancel(__run_thread[obj_]);

    pthread_mutex_lock(&__sgt->__main_mutex);
    __run_thread.erase(obj_);
    __nb_running--;
    pthread_mutex_unlock(&__sgt->__main_mutex);
  }

  void NProcess::RunThis(Object* obj_){
    if (__run_thread.count(obj_)){
      clog << "WARNING: Object already running.\n";
      return;
    }

    pthread_mutex_lock(&__sgt->__main_mutex);
    __nb_running++;
    pthread_create(&__run_thread[obj_], &__attr, RunThis, (void*)obj_);
    pthread_mutex_unlock(&__sgt->__main_mutex);



  }

  void* NProcess::RunThis(void* obj_){
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS,NULL);
    ((NProcess::Object*)(obj_))->Run();
    pthread_mutex_lock(&__sgt->__main_mutex);
    __run_thread.erase((Object*)obj_);
    __nb_running--;
    pthread_mutex_unlock(&__sgt->__main_mutex);

    return NULL;
  }

  void NProcess::Lock(string key_){

    pthread_mutex_lock(&__map_mutex);
    if(!__mutex_lst.count(key_)){
      pthread_mutex_t a_mutex;
      __mutex_lst[key_]=a_mutex;
      pthread_mutex_init(&__mutex_lst[key_], NULL);
    }
    pthread_mutex_t *to_mutex=&__mutex_lst[key_];
    pthread_mutex_lock(to_mutex);
    pthread_mutex_unlock(&__map_mutex);

  }

  void NProcess::Unlock (string key_){
    pthread_mutex_lock(&__map_mutex);
    if(__mutex_lst.count(key_))
      pthread_mutex_unlock(&__mutex_lst[key_]);
    else
      cerr << "NProcess::Unlock ERROR: " << key_ << " does not exist among mutexes" << endl;
    pthread_mutex_unlock(&__map_mutex);
  }

  void NProcess::ProcessAll (unsigned int step_){
    unsigned int i=0;

    for (list<Object*>::iterator it=__obj_lst.begin(); it!=__obj_lst.end();it++){

      if (step_ != (*it)->GetStep()) continue;

      pthread_mutex_lock(&__launcher_mutex);
      pthread_create(&__threads[i], &__attr, ProcessThis, (void*)(*it));
    }
    pthread_mutex_lock (&__join_mutex);
    pthread_mutex_unlock (&__join_mutex);
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

  void NProcess::WaitRunning (float sec_) {
    long start=time(0);
    long stop=start+sec_;

    while ((start<=stop || sec_<=0) && __run_thread.size())
      usleep (100000);

  }

  void NProcess::WaitThis (Object* obj_, float sec_) {
    long start=time(0);
    long stop=start+sec_;

    while ((start<=stop || sec_<=0) && __run_thread.count(obj_))
      usleep (100000);

  }

  unsigned int NProcess::Object::__process_id__=0;
  map<NProcess::Object*,pthread_t> NProcess::__run_thread;
  int NProcess::__nb_running=0;


  NProcess::Object::Object (){
    icedcode::NProcess::GetIt()->Add(this);
    __process_id__++;
    __step__=0;
  }
  NProcess::Object::~Object (){
    list<Object*>::iterator pos=find (__sgt->__obj_lst.begin(), __sgt->__obj_lst.end(),this);
    __sgt->__obj_lst.erase(pos);
    while (__sgt->__obj_lst.size() < __sgt->__threads.size())
      __sgt->__threads.pop_back();
  }

  NProcess::Object::Object (const Object& obj_){
    *this = obj_;
    __sgt->Add(this);
    __process_id__++;
  }
  NProcess::Object& NProcess::Object::operator= (const Object& obj_){
    *this = obj_;
    __sgt->Add(this);
    __process_id__++;
    return *this;
  }

}
