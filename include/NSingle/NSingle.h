/**
 * @file   NSingle.h
 * @author Christophe Hugon <hugon@cppm.in2p3.fr>
 * @date   Mon Jul 24 17:04:17 2017
 *
 * @brief  Singleton generalistic class
 *         Make easy to create singletons by inheritence
 *
 */


#ifndef __ICEDCODE_SINGLE_H__
#define __ICEDCODE_SINGLE_H__

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
    /**
     * Main end-user function.
     * The singleton is used in such a way: ClassName::GetIt ()
     * Take a look at the NProcess class as example to get how it works.
     * @return Object static address
     */
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

    /**
     * Remove the current singleton
     * The next GetIt will return a new one, and this one will be deleted.
     */
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
