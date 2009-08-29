#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <limits>
#include <boost\random\mersenne_twister.hpp>

template<typename ResultType, typename UnderlyingRandomGenerator = boost::mt19937>
class RandomGenerator
  {
  public:
    RandomGenerator();
    RandomGenerator(typename UnderlyingRandomGenerator::result_type i_seed);

    ResultType operator()(ResultType i_max_value);
    ResultType operator()(ResultType i_min_value, ResultType i_max_value);

    void SetSeed(typename UnderlyingRandomGenerator::result_type i_seed);

  private:
    UnderlyingRandomGenerator m_generator;
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
  return m_generator()*(i_max_value*m_inv_max);
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
ResultType RandomGenerator<ResultType,UnderlyingRandomGenerator>::operator()(ResultType i_min_value, ResultType i_max_value)
  {
  return i_min_value + m_generator()*((i_max_value-i_min_value)*m_inv_max);
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
void RandomGenerator<ResultType,UnderlyingRandomGenerator>::SetSeed(typename UnderlyingRandomGenerator::result_type i_seed)
  {
  m_generator.seed(i_seed);
  }

#endif // RANDOMGENERATOR_H