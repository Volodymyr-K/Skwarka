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

#ifndef THREAD_SAFE_RANDOM_H
#define THREAD_SAFE_RANDOM_H

#include <Common\Common.h>
#include <limits>
#include <boost\random\mersenne_twister.hpp>
#include <tbb\enumerable_thread_specific.h>
#include <tbb\cache_aligned_allocator.h>

/**
* Thread-safe random generator class template.
* The class simply wraps the underlying implementation.
* UnderlyingRandomGenerator template parameter corresponds to the type of the underlying random generator implementation.
* Each thread has its own thread-local copy of UnderlyingRandomGenerator thus ensuring there's no concurrent access to the generator's state.
* The class provides an option to decorrelate the values produced in different threads (default behavior).
* @sa RandomGenerator
*/
template<typename UnderlyingRandomGenerator>
class ThreadSafeRandomGenerator
  {
  private:
    /**
    * tbb::enumerable_thread_specific is the thread local storage used to keep separate generators for different threads.
    */
    typedef tbb::enumerable_thread_specific<UnderlyingRandomGenerator, tbb::cache_aligned_allocator<UnderlyingRandomGenerator>, tbb::ets_no_key> ThreadRandomGenerators;
    // TBD: it is more efficient to use ets_key_per_instance instead of ets_no_key here but the current version of TBB does not support it yet.
    // Consider switching to it when it does.

  public:
    /**
    * Constructs an object of the thread-safe random generator.
    * @param i_decorrelate_thread_generators If the value is false the thread-local generators may all produce the same sequence of random values (depending on the UnderlyingRandomGenerator,
    * at least it holds for boost::mt19937). If the value is true all the values will be decorrelated by seeding the thread-local generators the thread ID.
    */
    ThreadSafeRandomGenerator(bool i_decorrelate_thread_generators = true);

    /**
    * Generates random value in [0;INT_MAX) range.
    * The method is thread-safe.
    */
    int GenerateIntRandom();

    unsigned int GenerateUIntRandom();

    /**
    * Generates random value in [0;1.0) range.
    * The method is thread-safe.
    */
    double GenerateNormalizedRandom();

  private:
    // Not implemented, not a value type.
    ThreadSafeRandomGenerator(const ThreadSafeRandomGenerator<UnderlyingRandomGenerator>&);
    ThreadSafeRandomGenerator &operator=(const ThreadSafeRandomGenerator<UnderlyingRandomGenerator>&);

  private:
    ThreadRandomGenerators m_thread_generators;
    bool m_decorrelate_thread_generators;

    /**
    * Cached value used to normalize random values produced by the underlying random generator.
    */
    double m_inv_max;
  };

/**
* Global thread-safe function generating double random values in [0;i_max) range.
* If i_max is negative then random values are generated in (i_max;0] range.
* @param i_max Upper bound on the range of random values to be produced. Can be negative.
*/
double RandomDouble(double i_max);

/**
* Global thread-safe function generating double random values in [i_min;i_max) range.
* If i_max<i_min then random values are generated in (i_max;i_min] range.
* @param i_min Lower bound on the range of random values to be produced. Can be negative.
* @param i_max Upper bound on the range of random values to be produced. Can be negative.
*/
double RandomDouble(double i_min, double i_max);

/**
* Global thread-safe function generating int random values in [0;i_max) range.
* If i_max is negative then random values are generated in (i_max;0] range.
* @param i_max Upper bound on the range of random values to be produced. Should not be null. Can be negative.
*/
int RandomInt(int i_max);

/**
* Global thread-safe function generating int random values in [i_min;i_max) range.
* If i_max<i_min then random values are generated in (i_max;i_min] range.
* i_min should not be equal to i_max.
* @param i_min Lower bound on the range of random values to be produced. Can be negative.
* @param i_max Upper bound on the range of random values to be produced. Can be negative.
*/
int RandomInt(int i_min, int i_max);

/**
* Global thread-safe function generating unsigned int random values in [0;2^32) range.
*/
unsigned int RandomUInt();

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename UnderlyingRandomGenerator>
ThreadSafeRandomGenerator<UnderlyingRandomGenerator>::ThreadSafeRandomGenerator(bool i_decorrelate_thread_generators):
m_decorrelate_thread_generators(i_decorrelate_thread_generators)
  {
  // We add 1.0 to the divider to prevent the random generator from returning upper bound of the input range in operator() methods.
  m_inv_max = 1.0 / (1.0+INT_MAX);
  ASSERT(m_inv_max < 1.0/INT_MAX);
  }

template<typename UnderlyingRandomGenerator>
double ThreadSafeRandomGenerator<UnderlyingRandomGenerator>::GenerateNormalizedRandom()
  {  
  return GenerateIntRandom()*m_inv_max;
  }

template<typename UnderlyingRandomGenerator>
int ThreadSafeRandomGenerator<UnderlyingRandomGenerator>::GenerateIntRandom()
  {
  return GenerateUIntRandom()%INT_MAX;
  }

template<typename UnderlyingRandomGenerator>
unsigned int ThreadSafeRandomGenerator<UnderlyingRandomGenerator>::GenerateUIntRandom()
  {
  // Get thread-local copy of random generator (create if not exists).
  bool exists;
  UnderlyingRandomGenerator &thread_local_generator = m_thread_generators.local(exists);

  // Decorrelate random generators for different threads by seeding the thread ID.
  if (exists==false && m_decorrelate_thread_generators)
    thread_local_generator.seed(GetCurrentThreadId());

  return thread_local_generator();
  }

// This global thread-safe random generator is used in all the global random functions below.
extern ThreadSafeRandomGenerator<boost::mt19937> global_multi_threaded_random_generator;

inline double RandomDouble(double i_max)
  {
  return global_multi_threaded_random_generator.GenerateNormalizedRandom() * i_max;
  }

inline double RandomDouble(double i_min, double i_max)
  {
  return i_min + RandomDouble(i_max-i_min);
  }

inline int RandomInt(int i_max)
  {
  ASSERT(i_max!=0);

  if (i_max>0)
    return global_multi_threaded_random_generator.GenerateIntRandom() % i_max;
  else
    return - (global_multi_threaded_random_generator.GenerateIntRandom() % (-i_max));
  }

inline int RandomInt(int i_min, int i_max)
  {
  ASSERT(i_min!=i_max);

  // Assert for int overflow.
  ASSERT( (i_max>i_min && i_max-i_min>0) || (i_max<i_min && i_max-i_min<0) );
  return i_min + RandomInt(i_max - i_min);
  }

inline unsigned int RandomUInt()
  {
  return global_multi_threaded_random_generator.GenerateUIntRandom();
  }

#endif // THREAD_SAFE_RANDOM_H