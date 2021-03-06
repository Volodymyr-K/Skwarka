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

#ifndef SPECTRUM_H
#define SPECTRUM_H

#include <Common/Common.h>
#include <Math/MathRoutines.h>
#include <Math/HalfFloat.h>
#include "SpectrumCoef.h"
#include <istream>

/**
* Represents the light spectrum. Can define radiance, irradiance, flux and other light values.
* Don't mix it up with the SpectrumCoef class (which is a dimensionless quantity) that is used to represent properties of
* surface or volume media like surface reflectivity, volume transmittance.
* The spectrum data is represented by a piecewise-constant function with three segments on the wavelengths domain.
* The actual wavelength segments are not defined by this class, see SpectrumRoutines namespace for the details.
* The template parameter corresponds to the type of the spectrum values (e.g. float, double).
* @sa SpectrumCoef, SpectrumRoutines
*/
template<typename T>
class Spectrum
  {
  public:
    Spectrum();
    explicit Spectrum(T i_value);
    Spectrum(T i_value1, T i_value2, T i_value3);

    Spectrum<T> operator+(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator+=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator-(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator-=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator*(const SpectrumCoef<T> &i_spectrum_coef) const;
    Spectrum<T> &operator*=(const SpectrumCoef<T> &i_spectrum_coef);

    Spectrum<T> operator/(const SpectrumCoef<T> &i_spectrum_coef) const;
    Spectrum<T> &operator/=(const SpectrumCoef<T> &i_spectrum_coef);

    Spectrum<T> operator*(double i_value) const;
    Spectrum<T> &operator*=(double i_value);

    Spectrum<T> operator/(double i_value) const;
    Spectrum<T> &operator/=(double i_value);

    bool operator==(const Spectrum<T> &i_spectrum) const;
    bool operator!=(const Spectrum<T> &i_spectrum) const;

    T operator[](size_t i_index) const;
    T &operator[](size_t i_index);

    /**
    * Clamps the spectrum components to the specified range.
    */
    void Clamp(T i_low, T i_high);

    /**
    * Adds a weighted Spectrum to this one. The method is more efficient than calling += and * operators.
    */
    void AddWeighted(const Spectrum &i_spectrum, T i_weight);

    /**
    * Returns true if the spectrum represents black light (i.e. no light).
    */
    bool IsBlack() const;

  private:
    // Values of the piecewise-constant spectrum function.
    T m_values[3];
  };

template<typename T1, typename T2>
Spectrum<T2> operator*(T1 i_value, const Spectrum<T2> &i_spectrum);

template<typename T>
Spectrum<T> operator*(const SpectrumCoef<T> &i_spectrum_coef, const Spectrum<T> &i_spectrum);

/**
* Reads Spectrum from the input stream.
*/
template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Spectrum<T> &o_spectrum);

/**
* Prints Spectrum to the output stream.
*/
template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Spectrum<T> &i_spectrum);

/**
* Converts Spectrum instance to a Spectrum parameterized by a specified type.
*/
template<typename T2, typename T>
Spectrum<T2> Convert(const Spectrum<T> &i_spectrum);

/**
* Returns true if all spectrum components are in the specified range (inclusive).
*/
template<typename T>
bool InRange(const Spectrum<T> &i_spectrum, T i_low, T i_high);

/**
* Returns true if any of the spectrum components is NaN.
*/
template<typename T>
bool IsNaN(const Spectrum<T> &i_spectrum);

/**
* Returns true if any of the spectrum components is a positive or a negative infinity.
*/
template<typename T>
bool IsInf(const Spectrum<T> &i_spectrum);

typedef Spectrum<float> Spectrum_f;
typedef Spectrum<double> Spectrum_d;
typedef Spectrum<HalfFloat> Spectrum_h;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Spectrum<T>::Spectrum()
  {
  m_values[0]=0;
  m_values[1]=0;
  m_values[2]=0;
  }

template<typename T>
Spectrum<T>::Spectrum(T i_value)
  {
  ASSERT(IsNaN(i_value)==false);

  m_values[0]=i_value;
  m_values[1]=i_value;
  m_values[2]=i_value;
  }

template<typename T>
Spectrum<T>::Spectrum(T i_value1, T i_value2, T i_value3)
  {
  ASSERT(IsNaN(i_value1)==false && IsNaN(i_value2)==false && IsNaN(i_value3)==false);

  m_values[0]=i_value1;
  m_values[1]=i_value2;
  m_values[2]=i_value3;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator+(const Spectrum<T> &i_spectrum) const
  {
  return Spectrum<T>(m_values[0]+i_spectrum.m_values[0], m_values[1]+i_spectrum.m_values[1], m_values[2]+i_spectrum.m_values[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator+=(const Spectrum<T> &i_spectrum)
  {
  m_values[0]+=i_spectrum.m_values[0];
  m_values[1]+=i_spectrum.m_values[1];
  m_values[2]+=i_spectrum.m_values[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator-(const Spectrum<T> &i_spectrum) const
  {
  return Spectrum<T>(m_values[0]-i_spectrum.m_values[0], m_values[1]-i_spectrum.m_values[1], m_values[2]-i_spectrum.m_values[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator-=(const Spectrum<T> &i_spectrum)
  {
  m_values[0]-=i_spectrum.m_values[0];
  m_values[1]-=i_spectrum.m_values[1];
  m_values[2]-=i_spectrum.m_values[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator*(const SpectrumCoef<T> &i_spectrum_coef) const
  {
  return Spectrum<T>(m_values[0]*i_spectrum_coef[0], m_values[1]*i_spectrum_coef[1], m_values[2]*i_spectrum_coef[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator*=(const SpectrumCoef<T> &i_spectrum_coef)
  {
  m_values[0]*=i_spectrum_coef[0];
  m_values[1]*=i_spectrum_coef[1];
  m_values[2]*=i_spectrum_coef[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator/(const SpectrumCoef<T> &i_spectrum_coef) const
  {
  ASSERT(i_spectrum_coef[0]!=0.0 && i_spectrum_coef[1]!=0.0 && i_spectrum_coef[2]!=0.0);

  return Spectrum<T>(m_values[0]/i_spectrum_coef[0], m_values[1]/i_spectrum_coef[1], m_values[2]/i_spectrum_coef[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator/=(const SpectrumCoef<T> &i_spectrum_coef)
  {
  ASSERT(i_spectrum_coef[0]!=0.0 && i_spectrum_coef[1]!=0.0 && i_spectrum_coef[2]!=0.0);

  m_values[0]/=i_spectrum_coef[0];
  m_values[1]/=i_spectrum_coef[1];
  m_values[2]/=i_spectrum_coef[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator*(double i_value) const
  {
  ASSERT(IsNaN(i_value)==false);

  return Spectrum<T>(
    (T) (m_values[0]*i_value), 
    (T) (m_values[1]*i_value), 
    (T) (m_values[2]*i_value));
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator*=(double i_value)
  {
  ASSERT(IsNaN(i_value)==false);

  m_values[0]=(T)(m_values[0]*i_value);
  m_values[1]=(T)(m_values[1]*i_value);
  m_values[2]=(T)(m_values[2]*i_value);
  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator/(double i_value) const
  {
  ASSERT(IsNaN(i_value)==false);
  ASSERT(i_value != 0.0);

  double inv = 1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator/=(double i_value)
  {
  ASSERT(IsNaN(i_value)==false);
  ASSERT(i_value != 0.0);

  double inv = 1.0 / i_value;
  (*this)*=inv;
  return *this;
  }

template<typename T>
bool Spectrum<T>::operator==(const Spectrum<T> &i_spectrum) const
  {
  if (m_values[0] != i_spectrum.m_values[0]) return false;
  if (m_values[1] != i_spectrum.m_values[1]) return false;
  if (m_values[2] != i_spectrum.m_values[2]) return false;
  return true;
  }

template<typename T>
bool Spectrum<T>::operator!=(const Spectrum<T> &i_spectrum) const
  {
  return !operator==(i_spectrum);
  }

template<typename T>
T Spectrum<T>::operator[](size_t i_index) const
  {
  ASSERT(i_index<3);
  return m_values[i_index];
  }

template<typename T>
T &Spectrum<T>::operator[](size_t i_index)
  {
  ASSERT(i_index<3);
  return m_values[i_index];
  }

template<typename T>
void Spectrum<T>::Clamp(T i_low, T i_high)
  {
  ASSERT(i_low<=i_high);
  m_values[0]=MathRoutines::Clamp(m_values[0], i_low, i_high);
  m_values[1]=MathRoutines::Clamp(m_values[1], i_low, i_high);
  m_values[2]=MathRoutines::Clamp(m_values[2], i_low, i_high);
  }

template<typename T>
void Spectrum<T>::AddWeighted(const Spectrum &i_spectrum, T i_weight)
  {
  ASSERT(IsNaN(i_weight)==false);
  m_values[0] += i_weight * i_spectrum.m_values[0];
  m_values[1] += i_weight * i_spectrum.m_values[1];
  m_values[2] += i_weight * i_spectrum.m_values[2];
  }

template<typename T>
bool Spectrum<T>::IsBlack() const
  {  
  return m_values[0] == (T)0.0 && m_values[1] == (T)0.0 && m_values[2] == (T)0.0;
  }

template<typename T1, typename T2>
Spectrum<T2> operator*(T1 i_value, const Spectrum<T2> &i_spectrum)
  {
  ASSERT(IsNaN(i_value)==false);
  return i_spectrum*i_value;
  }

template<typename T>
Spectrum<T> operator*(const SpectrumCoef<T> &i_spectrum_coef, const Spectrum<T> &i_spectrum)
  {
  return i_spectrum*i_spectrum_coef;
  }

template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, Spectrum<T> &o_spectrum)
  {
  i_stream >> o_spectrum[0] >> o_spectrum[1] >> o_spectrum[2];
  return i_stream;
  }

template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const Spectrum<T> &i_spectrum)
  {
  /* string stream
  * - with same format
  * - without special field width
  */
  std::basic_ostringstream<charT,traits> s;
  s.copyfmt(o_stream);
  s.width(0);

  s << i_spectrum[0] << ' ' << i_spectrum[1] << ' ' << i_spectrum[2];
  o_stream << s.str();
  return o_stream;
  }

template<typename T2, typename T>
Spectrum<T2> Convert(const Spectrum<T> &i_spectrum)
  {
  return Spectrum<T2>((T2)i_spectrum[0], (T2)i_spectrum[1], (T2)i_spectrum[2]);
  }

template<typename T>
bool InRange(const Spectrum<T> &i_spectrum, T i_low, T i_high)
  {
  return
    i_spectrum[0]>=i_low && i_spectrum[0]<=i_high &&
    i_spectrum[1]>=i_low && i_spectrum[1]<=i_high &&
    i_spectrum[2]>=i_low && i_spectrum[2]<=i_high;
  }

template<typename T>
bool IsNaN(const Spectrum<T> &i_spectrum)
  {
  return IsNaN(i_spectrum[0]) || IsNaN(i_spectrum[1]) || IsNaN(i_spectrum[2]);
  }

template<typename T>
bool IsInf(const Spectrum<T> &i_spectrum)
  {
  return IsInf(i_spectrum[0]) || IsInf(i_spectrum[1]) || IsInf(i_spectrum[2]);
  }

#endif // SPECTRUM_H