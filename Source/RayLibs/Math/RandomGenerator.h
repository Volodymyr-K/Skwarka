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
* For the completely thread-safe random generator implementation see ThreadSafeRandomGenerator.
* @sa ThreadSafeRandomGenerator
*/
template<typename ResultType, typename UnderlyingRandomGenerator = boost::mt19937>
class RandomGenerator
  {
  public:
    RandomGenerator();

    /**
    * Constructs random generator with the specified seed value.
    */
    RandomGenerator(size_t i_seed);

    /**
    * Produces random value in [0;i_max) range.
    * @param i_max Upper bound on the range of random values to be produced. Can be negative.
    */
    ResultType operator()(ResultType i_max);

    /**
    * Produces random value in [i_min;i_max) range.
    * If i_max<i_min then random values are generated in (i_max;i_min] range.
    * @param i_min Lower bound on the range of random values to be produced. Can be negative.
    * @param i_max Upper bound on the range of random values to be produced. Can be negative.
    */
    ResultType operator()(ResultType i_min, ResultType i_max);

    /**
    * Sets seed value for the random generator.
    */
    void SetSeed(typename UnderlyingRandomGenerator::result_type i_seed);

  public:
    // Not implemented, not a value type.
    RandomGenerator(const RandomGenerator<ResultType,UnderlyingRandomGenerator>&);
    RandomGenerator &operator=(const RandomGenerator<ResultType,UnderlyingRandomGenerator>&);

  private:
    UnderlyingRandomGenerator m_generator;

    /**
    * Cached value used to normalize random values produced by the underlying random generator.
    */
    double m_inv_max;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename ResultType, typename UnderlyingRandomGenerator>
RandomGenerator<ResultType,UnderlyingRandomGenerator>::RandomGenerator()
  {
  // We add 1.0 to the divider to prevent the random generator from returning upper bound of the input range in operator() methods.
  m_inv_max = 1.0 / (1.0+std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max());
  ASSERT(m_inv_max < 1.0/std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max());
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
RandomGenerator<ResultType,UnderlyingRandomGenerator>::RandomGenerator(size_t i_seed):
m_generator(static_cast<typename UnderlyingRandomGenerator::result_type>(i_seed))
  {
  // We add 1.0 to the divider to prevent the random generator from returning upper bound of the input range in operator() methods.
  m_inv_max = 1.0 / (1.0+std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max());
  ASSERT(m_inv_max < 1.0/std::numeric_limits<typename UnderlyingRandomGenerator::result_type>::max());
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
ResultType RandomGenerator<ResultType,UnderlyingRandomGenerator>::operator()(ResultType i_max)
  {
  return (ResultType) ( m_generator()*(i_max*m_inv_max) );
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
ResultType RandomGenerator<ResultType,UnderlyingRandomGenerator>::operator()(ResultType i_min, ResultType i_max)
  {
  return i_min + this->operator()(i_max-i_min);
  }

template<typename ResultType, typename UnderlyingRandomGenerator>
void RandomGenerator<ResultType,UnderlyingRandomGenerator>::SetSeed(typename UnderlyingRandomGenerator::result_type i_seed)
  {
  m_generator.seed(i_seed);
  }

#endif // RANDOM_GENERATOR_H