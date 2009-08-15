#ifndef SPECTRUM_H
#define SPECTRUM_H

#include "Common\Common.h"

template<typename T>
class Spectrum
  {
  public:
    Spectrum();
    Spectrum(T i_r, T i_g, T i_b);

    Spectrum<T> operator+(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator+=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator-(const Spectrum<T> &i_spectrum) const;
    Spectrum<T> &operator-=(const Spectrum<T> &i_spectrum);

    Spectrum<T> operator*(T i_value) const;
    Spectrum<T> &operator*=(T i_value);

    Spectrum<T> operator/(T i_value) const;
    Spectrum<T> &operator/=(T i_value);

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);

    void AddWeighted(const Spectrum &i_spectrum, T i_weight);
    bool IsZero() const;
    void XYZ(T o_xyz[3]) const;
    T Luminance() const;

  private:
    T m_rgb[3];
  };

template<typename T>
Spectrum<T> operator*(T i_value, const Spectrum<T> &i_spectrum);

template<typename T2, typename T>
Spectrum<T2> Convert(const Spectrum<T> &i_spectrum);

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
Spectrum<T>::Spectrum(T i_r, T i_g, T i_b)
  {
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
Spectrum<T> Spectrum<T>::operator*(T i_value) const
  {
  return Spectrum<T>(m_rgb[0]*i_value, m_rgb[1]*i_value, m_rgb[2]*i_value);
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator*=(T i_value)
  {
  m_rgb[0]*=i_value;
  m_rgb[1]*=i_value;
  m_rgb[2]*=i_value;

  return *this;
  }

template<typename T>
Spectrum<T> Spectrum<T>::operator/(T i_value) const
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  return (*this) * inv;
  }

template<typename T>
Spectrum<T> &Spectrum<T>::operator/=(T i_value)
  {
  ASSERT(fabs(i_value) > (T)0);

  T inv = (T)1.0 / i_value;
  (*this)*=inv;
  return *this;
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
void Spectrum<T>::AddWeighted(const Spectrum &i_spectrum, T i_weight)
  {
  m_rgb[0] += i_weight * i_spectrum.m_rgb[0];
  m_rgb[1] += i_weight * i_spectrum.m_rgb[1];
  m_rgb[2] += i_weight * i_spectrum.m_rgb[2];
  }

template<typename T>
bool Spectrum<T>::IsZero() const
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
  o_xyz[0] += XWeight[0] * m_rgb[0];
  o_xyz[1] += YWeight[1] * m_rgb[1];
  o_xyz[2] += ZWeight[2] * m_rgb[2];
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
  return i_spectrum*i_value;
  }

template<typename T2, typename T>
Spectrum<T2> Convert(const Spectrum<T> &i_spectrum)
  {
  return Spectrum<T2>((T2)i_spectrum[0], (T2)i_spectrum[1], (T2)i_spectrum[2]);
  }

#endif // SPECTRUM_H