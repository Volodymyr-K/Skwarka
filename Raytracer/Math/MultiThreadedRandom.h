#ifndef MULTI_THREADED_RANDOM_H
#define MULTI_THREADED_RANDOM_H

#include <limits>
#include <boost\random\mersenne_twister.hpp>
#include <tbb\enumerable_thread_specific.h>
#include <tbb\cache_aligned_allocator.h>

template<typename UnderlyingRandomGenerator>
class MultiThreadedRandomGenerator
  {
  private:
    typedef tbb::enumerable_thread_specific< UnderlyingRandomGenerator, tbb::cache_aligned_allocator<UnderlyingRandomGenerator>, tbb::ets_key_per_instance> ThreadedRandomGenerator;

  public:
    MultiThreadedRandomGenerator(bool i_decorrelate_thread_generators = true);

    double GenerateNormalizedRandom();

  private:
    ThreadedRandomGenerator m_thread_generators;

    bool m_decorrelate_thread_generators;
    const double m_inv_max;
  };

double RandomDouble(double i_max_value);
double RandomDouble(double i_min_value, double i_max_value);

unsigned int RandomUInt(unsigned int i_max_value);
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
typename double MultiThreadedRandomGenerator<UnderlyingRandomGenerator>::GenerateNormalizedRandom()
  {  
  bool exists;
  UnderlyingRandomGenerator &thread_local_generator = m_thread_generators.local(exists);

  // Decorrelate random generators for different threads by seeding the thread ID.
  if (exists==false && m_decorrelate_thread_generators)
    thread_local_generator.seed(GetCurrentThreadId());

  return thread_local_generator()*m_inv_max;
  }

// Global functions

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
  return (unsigned int) (global_multi_threaded_random_generator.GenerateNormalizedRandom() * i_max_value);
  }

inline unsigned int RandomUInt(unsigned int i_min_value, unsigned int i_max_value)
  {
  return i_min_value + RandomUInt(i_max_value - i_min_value);
  }

#endif // MULTI_THREADED_RANDOM_H