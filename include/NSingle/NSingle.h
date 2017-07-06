/*
  Hugon Christophe 2011.
  Under GPL. See "COPYING.txt"
*/


#ifndef ICEDCODE_SINGLE
#define ICEDCODE_SINGLE

#include <typeinfo>
#include <iostream>
#include <cstring>

namespace icedcode
{

  template <typename T>
    class NSingle {
  protected:
    NSingle () {
      __debug=false;
    }
    virtual ~NSingle () { }

    virtual void Init () = 0;
    virtual void Flush () = 0;

  public:
    void SetDebug(bool debug_);

  public:
    static T *GetIt ()
    {
      if (NULL == __sgt)
        {
          __sgt = new T;
          const char* name=typeid(__sgt).name();
          int size_name=strlen(name);

          std::clog << "creating singleton ";
          for(int i=3; i<size_name-1; i++)
            if (name[i] > '9')
              std::clog<<name[i];
            else
              std::clog << "::";

          std::clog<<'.'<< std::endl;
          __sgt->Init();
        }

      return (static_cast<T*> (__sgt));
    }

    static void kill ()
    {
      if (NULL != __sgt)
        {
          delete __sgt;
          __sgt = NULL;
        }
    }

  protected:
    static T *__sgt;

    bool __debug;

  private:
    NSingle (const NSingle&);
    NSingle& operator= (const NSingle&);


  };

  template <typename T>
    T *NSingle<T>::__sgt = NULL;
}
#endif
