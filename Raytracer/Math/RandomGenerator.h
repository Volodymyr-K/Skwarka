#ifndef RANDOM_GENERATOR_H
#define RANDOM_GENERATOR_H

#include <limits>
#include <boost\random\mersenne_twister.hpp>

/**
* Random generator class template.
* The class simply wraps the underlying implementation and provides convenient methods for generating random values in a specified range.
* ResultType template parameter corresponds to the type of the random values produced.
* UnderlyingRandomGenerator template parameter corresponds to the type of the underlying random generator implementation.
* The random generator is thread-safe if and only if the underlying random generator implementation is thread-safe (which is not the case for at least boost::mt19937).
* For the completely thread-safe random generator implementation see MultiThreadedRandomGenerator.
* @sa MultiThreadedRandomGenerator
*/
template<typename ResultType, typename UnderlyingRandomGenerator = boost::mt19937>
class RandomGenerator
  {
  public:
    RandomGenerator();

    /**
    * Constructs random generator with the specified seed value.
    */
    RandomGenerator(typename UnderlyingRandomGenerator::result_type i_seed);

    /**
    * Produces random value in [0;i_max_value) range.
    * @param i_max_value Upper bound on the range of random values to be produced. Can be negative.
    */
    ResultType operator()(ResultType i_max_value);

    /**
    * Produces random value in [i_min_value;i_max_value) range.
    * @param i_min_value Lower bound on the range of random values to be produced. Can be negative.
    * @param i_max_value Upper bound on the range of random values to be produced. Can be negative.
    */
    ResultType operator()(ResultType i_min_value, ResultType i_max_value);

    /**
    * Sets seed value for the random generator.
    */
    void SetSeed(typename UnderlyingRandomGenerator::result_type i_seed);

  private:
    UnderlyingRandomGenerator m_generator;

    /**
    * Cached value used to normalize random values produced by the underlying random generator.
    */
    const double m_inv_max;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ResultType, typename UnderlyingRandomGenerator>
RandomGenerator<ResultType,UnderlyingRandomGenerator>::RandomGenerator():
m_inv_max(1.0/std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max())
  {
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
RandomGenerator<ResultType,UnderlyingRandomGenerator>::RandomGenerator(typename UnderlyingRandomGenerator::result_type i_seed):
m_generator(i_seed), m_inv_max(1.0/std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max())
  {
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
ResultType RandomGenerator<ResultType,UnderlyingRandomGenerator>::operator()(ResultType i_max_value)
  {
  return (ResultType) ( m_generator()*(i_max_value*m_inv_max) );
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
ResultType RandomGenerator<ResultType,UnderlyingRandomGenerator>::operator()(ResultType i_min_value, ResultType i_max_value)
  {
  return i_min_value + this->operator()(i_max_value-i_min_value);
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
void RandomGenerator<ResultType,UnderlyingRandomGenerator>::SetSeed(typename UnderlyingRandomGenerator::result_type i_seed)
  {
  m_generator.seed(i_seed);
  }

#endif // RANDOM_GENERATOR_H