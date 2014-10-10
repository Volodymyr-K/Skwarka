/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

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
    size_t IncRef() const;

    /**
    * Decrements the counter by one and returns the decremented value.
    */
    size_t DecRef() const;

    virtual ~ReferenceCounted();

  private:
    mutable tbb::atomic<size_t> m_references;
    //mutable size_t m_references;

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

inline size_t ReferenceCounted::IncRef() const
  {
  return ++m_references;
  }

inline size_t ReferenceCounted::DecRef() const
  {
  ASSERT(m_references>0);
  return --m_references;
  }

/**
* This function is called by boost library when a new intrusive_ptr instance is created.
*/
template<typename T>
void intrusive_ptr_add_ref(const T *i_ptr)
  {
  i_ptr->IncRef();
  }

/**
* This function is called by boost library when a an intrusive_ptr instance is deleted.
*/
template<typename T>
void intrusive_ptr_release(const T *i_ptr)
  {
  if (i_ptr->DecRef()==0)
    delete i_ptr;
  }

/**
* Serializes intrusive_ptr to the specified Archive. This method is used by the boost serialization framework.
* This method is (indirectly) called by the serialize() method below.
*/
template<typename T, class Archive>
inline void save(Archive &i_ar, const intrusive_ptr<T> &i_ptr, const unsigned int i_version)
  {
  T *p_raw_poniter = i_ptr.get();
  i_ar << p_raw_poniter;
  }

/**
* De-serializes intrusive_ptr from the specified Archive. This method is used by the boost serialization framework.
* This method is (indirectly) called by the serialize() method below.
*/
template<typename T, class Archive>
inline void load(Archive &i_ar, intrusive_ptr<T> &i_ptr, const unsigned int i_version)
  {
  T *p_referenced;
  i_ar >> p_referenced;
  i_ptr.reset(p_referenced);
  }

/**
* Serializes intrusive_ptr to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, intrusive_ptr<T> &i_ptr, const unsigned int i_version)
  {
  boost::serialization::split_free(i_ar, i_ptr, i_version);
  }

/**
* Serializes ReferenceCounted to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, ReferenceCounted &i_ref, const unsigned int i_version)
  {
  // Nothing to do here because we don't serialize the references counter. The counter value is dynamically updated during the de-serialization.
  }

#endif // INTRUSIVE_SMART_POINTER_H