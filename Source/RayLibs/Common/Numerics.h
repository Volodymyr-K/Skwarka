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