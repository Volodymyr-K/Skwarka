#ifndef CORE_COMMON_H
#define CORE_COMMON_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/RandomGenerator.h>

/**
* This structure encapsulates objects that are passed through the pipeline of the raytracer and which are specific to each thread.
*/
struct ThreadSpecifics
  {
  MemoryPool *mp_pool;

  RandomGenerator<double> *mp_random_generator;
  };

#endif // CORE_COMMON_H