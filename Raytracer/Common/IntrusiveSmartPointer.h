#ifndef INTRUSIVE_SMART_POINTER_H
#define INTRUSIVE_SMART_POINTER_H

#include "Assert.h"
#include <boost/intrusive_ptr.hpp>
#include <tbb/atomic.h>
using boost::intrusive_ptr;

/**
* This is the base class for all classes that need to be references-counted by the intrusive smart pointer strategy.
* The class has a single member field for the references counter.
* Intrusive pointers from the boost library based on this class are thread-safe.
*/
class ReferenceCounted
  {
  public:
    ReferenceCounted();

    /**
    * Increments the counter by one and returns the incremented value.
    */
    size_t IncRef();

    /**
    * Decrements the counter by one and returns the decremented value.
    */
    size_t DecRef();

    virtual ~ReferenceCounted();

  private:
    tbb::atomic<size_t> m_references;
    //size_t m_references;

    // Not implemented. All classes inheriting ReferenceCounted should only be passed by pointer or reference.
    ReferenceCounted(const ReferenceCounted &);
    ReferenceCounted &operator=(const ReferenceCounted &);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// For some reason TBB does not initialize m_references with 0 if it is listed in the constructor's initializer list.
// So, we have to initialize it manually by assigning 0 to it.
inline ReferenceCounted::ReferenceCounted()//: m_references()
  {
  m_references=0;
  }

inline ReferenceCounted::~ReferenceCounted()
  {
  }

inline size_t ReferenceCounted::IncRef()
  {
  return ++m_references;
  }

inline size_t ReferenceCounted::DecRef()
  {
  ASSERT(m_references>0);
  return --m_references;
  }

/**
* This function is called by boost library when a new intrusive_ptr instance is created.
*/
template<typename T>
void intrusive_ptr_add_ref(T *i_ptr)
  {
  i_ptr->IncRef();
  }

/**
* This function is called by boost library when a an intrusive_ptr instance is deleted.
*/
template<typename T>
void intrusive_ptr_release(T *i_ptr)
  {
  if (i_ptr->DecRef()==0)
    delete i_ptr;
  }

#endif // INTRUSIVE_SMART_POINTER_H