#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "Common\Common.h"
#include <istream>
#include <Math/Util.h>

/**
* Represents the spectrum of light (i.e. color).
* The spectrum is represented by the RGB model.
* The template parameter corresponds to the type of the RGB components.
*/
template<typename T>
class Spectrum
  {
  public:
    Spectrum();
    explicit Spectrum(T i_value);
    Spectrum(T i_r, T i_g, T i_b);

    Spectrum<T> operator+(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator+=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator-(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator-=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator*(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator*=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator/(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator/=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator*(double i_value) const;
    Spectrum<T> &operator*=(double i_value);

    Spectrum<T> operator/(double i_value) const;
    Spectrum<T> &operator/=(double i_value);

    bool operator==(const Spectrum<T> &i_point) const;
    bool operator!=(const Spectrum<T> &i_point) const;

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);

    /**
    * Clamps the spectrum components to the specified range.
    */
    void Clamp(T i_low, T i_high);

    /**
    * Adds a weighted Spectrum to this one. The method is needed for the efficiency.
    */
    void AddWeighted(const Spectrum &i_spectrum, T i_weight);

    /**
    * Returns true if the spectrum represents black light (i.e. no light).
    */
    bool IsBlack() const;

    /**
    * Converts the Spectrum to the XYZ response values.
    */
    void XYZ(T o_xyz[3]) const;

    /**
    * Returns luminance value of the spectrum (Y value of the XYZ).
    */
    T Luminance() const;

  private:
    T m_rgb[3];
  };

template<typename T>
Spectrum<T> operator*(T i_value, const Spectrum<T> &i_spectrum);

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

typedef Spectrum<float> Spectrum_f;
typedef Spectrum<double> Spectrum_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
Spectrum<T>::Spectrum()
  {
  m_rgb[0]=0;
  m_rgb[1]=0;
  m_rgb[2]=0;
  }

template<typename T>
Spectrum<T>::Spectrum(T i_value)
  {
  ASSERT(IsNaN(i_value)==false);

  m_rgb[0]=i_value;
  m_rgb[1]=i_value;
  m_rgb[2]=i_value;
  }

template<typename T>
Spectrum<T>::Spectrum(T i_r, T i_g, T i_b)
  {
  ASSERT(IsNaN(i_r)==false && IsNaN(i_g)==false && IsNaN(i_b)==false);

  m_rgb[0]=i_r;
  m_rgb[1]=i_g;
  m_rgb[2]=i_b;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator+(const Spectrum<T> &i_spectrum) const
  {
  return Spectrum<T>(m_rgb[0]+i_spectrum.m_rgb[0], m_rgb[1]+i_spectrum.m_rgb[1], m_rgb[2]+i_spectrum.m_rgb[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator+=(const Spectrum<T> &i_spectrum)
  {
  m_rgb[0]+=i_spectrum.m_rgb[0];
  m_rgb[1]+=i_spectrum.m_rgb[1];
  m_rgb[2]+=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator-(const Spectrum<T> &i_spectrum) const
  {
  return Spectrum<T>(m_rgb[0]-i_spectrum.m_rgb[0], m_rgb[1]-i_spectrum.m_rgb[1], m_rgb[2]-i_spectrum.m_rgb[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator-=(const Spectrum<T> &i_spectrum)
  {
  m_rgb[0]-=i_spectrum.m_rgb[0];
  m_rgb[1]-=i_spectrum.m_rgb[1];
  m_rgb[2]-=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator*(const Spectrum<T> &i_spectrum) const
  {
  return Spectrum<T>(m_rgb[0]*i_spectrum.m_rgb[0], m_rgb[1]*i_spectrum.m_rgb[1], m_rgb[2]*i_spectrum.m_rgb[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator*=(const Spectrum<T> &i_spectrum)
  {
  m_rgb[0]*=i_spectrum.m_rgb[0];
  m_rgb[1]*=i_spectrum.m_rgb[1];
  m_rgb[2]*=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator/(const Spectrum<T> &i_spectrum) const
  {
  ASSERT(i_spectrum.m_rgb[0]!=0.0 && i_spectrum.m_rgb[1]!=0.0 && i_spectrum.m_rgb[2]!=0.0);

  return Spectrum<T>(m_rgb[0]/i_spectrum.m_rgb[0], m_rgb[1]/i_spectrum.m_rgb[1], m_rgb[2]/i_spectrum.m_rgb[2]);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator/=(const Spectrum<T> &i_spectrum)
  {
  ASSERT(i_spectrum.m_rgb[0]!=0.0 && i_spectrum.m_rgb[1]!=0.0 && i_spectrum.m_rgb[2]!=0.0);

  m_rgb[0]/=i_spectrum.m_rgb[0];
  m_rgb[1]/=i_spectrum.m_rgb[1];
  m_rgb[2]/=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator*(double i_value) const
  {
  ASSERT(IsNaN(i_value)==false);

  return Spectrum<T>(
    (T) (m_rgb[0]*i_value), 
    (T) (m_rgb[1]*i_value), 
    (T) (m_rgb[2]*i_value));
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator*=(double i_value)
  {
  ASSERT(IsNaN(i_value)==false);

  m_rgb[0]=(T)(m_rgb[0]*i_value);
  m_rgb[1]=(T)(m_rgb[1]*i_value);
  m_rgb[2]=(T)(m_rgb[2]*i_value);
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
bool Spectrum<T>::operator==(const Spectrum<T> &i_point) const
  {
  if (m_rgb[0] != i_point.m_rgb[0]) return false;
  if (m_rgb[1] != i_point.m_rgb[1]) return false;
  if (m_rgb[2] != i_point.m_rgb[2]) return false;
  return true;
  }

template<typename T>
bool Spectrum<T>::operator!=(const Spectrum<T> &i_point) const
  {
  return !operator==(i_point);
  }

template<typename T>
T Spectrum<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_rgb[i_index];
  }

template<typename T>
T &Spectrum<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_rgb[i_index];
  }

template<typename T>
void Spectrum<T>::Clamp(T i_low, T i_high)
  {
  ASSERT(i_low<=i_high);
  m_rgb[0]=MathRoutines::Clamp(m_rgb[0], i_low, i_high);
  m_rgb[1]=MathRoutines::Clamp(m_rgb[1], i_low, i_high);
  m_rgb[2]=MathRoutines::Clamp(m_rgb[2], i_low, i_high);
  }

template<typename T>
void Spectrum<T>::AddWeighted(const Spectrum &i_spectrum, T i_weight)
  {
  ASSERT(IsNaN(i_weight)==false);
  m_rgb[0] += i_weight * i_spectrum.m_rgb[0];
  m_rgb[1] += i_weight * i_spectrum.m_rgb[1];
  m_rgb[2] += i_weight * i_spectrum.m_rgb[2];
  }

template<typename T>
bool Spectrum<T>::IsBlack() const
  {  
  if (m_rgb[0] != (T)0.0) return false;
  if (m_rgb[1] != (T)0.0) return false;
  if (m_rgb[2] != (T)0.0) return false;
  return true;
  }

template<typename T>
void Spectrum<T>::XYZ(T o_xyz[3]) const
  {
  const T XWeight[3] = {(T)0.412453, (T)0.357580, (T)0.180423};
  const T YWeight[3] = {(T)0.212671, (T)0.715160, (T)0.072169};
  const T ZWeight[3] = {(T)0.019334, (T)0.119193, (T)0.950227};

  o_xyz[0] = o_xyz[1] = o_xyz[2] = (T)0.0;
  for(unsigned char i=0;i<3;++i)
    {
    o_xyz[0] += XWeight[i] * m_rgb[i];
    o_xyz[1] += YWeight[i] * m_rgb[i];
    o_xyz[2] += ZWeight[i] * m_rgb[i];
    }
  }

template<typename T>
T Spectrum<T>::Luminance() const
  {
  const T YWeight[3] = {(T)0.212671, (T)0.715160, (T)0.072169};

  return YWeight[0]*m_rgb[0] + YWeight[1]*m_rgb[1] + YWeight[2]*m_rgb[2];
  }

template<typename T>
Spectrum<T> operator*(T i_value, const Spectrum<T> &i_spectrum)
  {
  ASSERT(IsNaN(i_value)==false);
  return i_spectrum*i_value;
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

#endif // SPECTRUM_H