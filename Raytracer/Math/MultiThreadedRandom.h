#ifndef MULTI_THREADED_RANDOM_H
#define MULTI_THREADED_RANDOM_H

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
class MultiThreadedRandomGenerator
  {
  private:
    /**
    * tbb::enumerable_thread_specific is the thread local storage used to keep separate generators for different threads.
    */
    typedef tbb::enumerable_thread_specific<UnderlyingRandomGenerator, tbb::cache_aligned_allocator<UnderlyingRandomGenerator>, tbb::ets_key_per_instance> ThreadRandomGenerators;

  public:
    /**
    * Constructs an object of the thread-safe random generator.
    * @param i_decorrelate_thread_generators If the value is false the thread-local generators may all produce the same sequence of random values (depending on the UnderlyingRandomGenerator,
    * at least it holds for boost::mt19937). If the value is true all the values will be decorrelated by seeding the thread-local generators the thread ID.
    */
    MultiThreadedRandomGenerator(bool i_decorrelate_thread_generators = true);

    /**
    * Generates random value in [0;UINT_MAX) range.
    * The method is thread-safe.
    */
    unsigned int GenerateUIntRandom();

    /**
    * Generates random value in [0;1.0) range.
    * The method is thread-safe.
    */
    double GenerateNormalizedRandom();

  private:
    ThreadRandomGenerators m_thread_generators;
    bool m_decorrelate_thread_generators;

    /**
    * Cached value used to normalize random values produced by the underlying random generator.
    */
    const double m_inv_max;
  };

/**
* Global thread-safe function generating double random values in [0;i_max_value) range.
* @param i_max_value Upper bound on the range of random values to be produced. Can be negative.
*/
double RandomDouble(double i_max_value);

/**
* Global thread-safe function generating double random values in [i_min_value;i_max_value) range.
* @param i_min_value Lower bound on the range of random values to be produced. Can be negative.
* @param i_max_value Upper bound on the range of random values to be produced. Can be negative.
*/
double RandomDouble(double i_min_value, double i_max_value);

/**
* Global thread-safe function generating unsigned int random values in [0;i_max_value) range.
* @param i_max_value Upper bound on the range of random values to be produced. Can be negative.
*/
unsigned int RandomUInt(unsigned int i_max_value);

/**
* Global thread-safe function generating unsigned int random values in [i_min_value;i_max_value) range.
* @param i_min_value Lower bound on the range of random values to be produced. Can be negative.
* @param i_max_value Upper bound on the range of random values to be produced. Can be negative.
*/
unsigned int RandomUInt(unsigned int i_min_value, unsigned int i_max_value);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename UnderlyingRandomGenerator>
MultiThreadedRandomGenerator<UnderlyingRandomGenerator>::MultiThreadedRandomGenerator(bool i_decorrelate_thread_generators):
m_decorrelate_thread_generators(i_decorrelate_thread_generators),
m_inv_max(1.0/std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max())
  {
  }

template<typename UnderlyingRandomGenerator>
double MultiThreadedRandomGenerator<UnderlyingRandomGenerator>::GenerateNormalizedRandom()
  {  
  return GenerateUIntRandom()*m_inv_max;
  }

template<typename UnderlyingRandomGenerator>
unsigned int MultiThreadedRandomGenerator<UnderlyingRandomGenerator>::GenerateUIntRandom()
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
extern MultiThreadedRandomGenerator<boost::mt19937> global_multi_threaded_random_generator;

inline double RandomDouble(double i_max_value)
  {
  return global_multi_threaded_random_generator.GenerateNormalizedRandom() * i_max_value;
  }

inline double RandomDouble(double i_min_value, double i_max_value)
  {
  return i_min_value + RandomDouble(i_max_value-i_min_value);
  }

inline unsigned int RandomUInt(unsigned int i_max_value)
  {
  return global_multi_threaded_random_generator.GenerateUIntRandom() % i_max_value;
  }

inline unsigned int RandomUInt(unsigned int i_min_value, unsigned int i_max_value)
  {
  return i_min_value + RandomUInt(i_max_value - i_min_value);
  }

#endif // MULTI_THREADED_RANDOM_H