#ifndef NUMERICS_H
#define NUMERICS_H

/*
* This file defines the numeric-related stuff like IsNaN and IsPositiveInf functions.
*/

#include <float.h>
#include <limits>

/**
* Returns true if the specified value is NaN.
*/
template<typename T>
bool IsNaN(T i_value);

/**
* Returns true if the specified value represents positive infinity.
*/
template<typename T>
bool IsPositiveInf(T i_value);

/**
* Returns true if the specified value represents negative infinity.
*/
template<typename T>
bool IsNegativeInf(T i_value);

/**
* Returns true if the specified value represents negative or positive infinity.
*/
template<typename T>
bool IsInf(T i_value);

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
bool IsNaN(T i_value)
  {
  // The generic implementation always returns false.
  
  ASSERT(0 && "IsNaN() is not implemented for this type.");
  return false;
  }

template<>
inline bool IsNaN<float>(float i_value)
  {
  return _isnan(i_value) != 0;
  }

template<>
inline bool IsNaN<double>(double i_value)
  {
  return _isnan(i_value) != 0;
  }

template<typename T>
bool IsPositiveInf(T i_value)
  {
  return i_value == std::numeric_limits<T>::infinity();
  }

template<typename T>
bool IsNegativeInf(T i_value)
  {
  return i_value == -std::numeric_limits<T>::infinity();
  }

template<typename T>
bool IsInf(T i_value)
  {
  return i_value == std::numeric_limits<T>::infinity() || i_value == -std::numeric_limits<T>::infinity();
  }

#endif // NUMERICS_H