#ifndef SPECTRUM_COEF_H
#define SPECTRUM_COEF_H

#include <Common/Common.h>
#include <Math/MathRoutines.h>
#include <istream>

/**
* Represents the spectrum of a light multiplier.
* SpectrumCoef is a dimensionless quantity that can be used to define a reflectivity of a surface, volume transmittance etc.
* Don't mix it up with the Spectrum class that is used to represent real light values like radiance or flux.
* The data is represented by the RGB model.
* The template parameter corresponds to the type of the RGB components.
* @sa Spectrum
*/
template<typename T>
class SpectrumCoef
  {
  public:
    SpectrumCoef();
    explicit SpectrumCoef(T i_value);
    SpectrumCoef(T i_r, T i_g, T i_b);

    SpectrumCoef<T> operator+(const SpectrumCoef<T> &i_spectrum) const;
    SpectrumCoef<T> &operator+=(const SpectrumCoef<T> &i_spectrum);

    SpectrumCoef<T> operator-(const SpectrumCoef<T> &i_spectrum) const;
    SpectrumCoef<T> &operator-=(const SpectrumCoef<T> &i_spectrum);

    SpectrumCoef<T> operator*(const SpectrumCoef<T> &i_spectrum) const;
    SpectrumCoef<T> &operator*=(const SpectrumCoef<T> &i_spectrum);

    SpectrumCoef<T> operator/(const SpectrumCoef<T> &i_spectrum) const;
    SpectrumCoef<T> &operator/=(const SpectrumCoef<T> &i_spectrum);

    SpectrumCoef<T> operator*(double i_value) const;
    SpectrumCoef<T> &operator*=(double i_value);

    SpectrumCoef<T> operator/(double i_value) const;
    SpectrumCoef<T> &operator/=(double i_value);

    bool operator==(const SpectrumCoef<T> &i_point) const;
    bool operator!=(const SpectrumCoef<T> &i_point) const;

    T operator[](unsigned char i_index) const;
    T &operator[](unsigned char i_index);

    /**
    * Clamps the spectrum components to the specified range.
    */
    void Clamp(T i_low, T i_high);

    /**
    * Adds a weighted SpectrumCoef to this one. The method is more efficient than calling += and * operators.
    */
    void AddWeighted(const SpectrumCoef &i_spectrum, T i_weight);

    /**
    * Returns true if the spectrum coefficient is zero everywhere.
    */
    bool IsBlack() const;

    /**
    * Returns luminance value of the spectrum (Y value of the XYZ representation).
    */
    T Luminance() const;

  private:
    T m_rgb[3];
  };

template<typename T1, typename T2>
SpectrumCoef<T2> operator*(T1 i_value, const SpectrumCoef<T2> &i_spectrum);

/**
* Reads SpectrumCoef from the input stream.
*/
template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, SpectrumCoef<T> &o_spectrum);

/**
* Prints SpectrumCoef to the output stream.
*/
template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const SpectrumCoef<T> &i_spectrum);

/**
* Converts SpectrumCoef instance to a SpectrumCoef parameterized by a specified type.
*/
template<typename T2, typename T>
SpectrumCoef<T2> Convert(const SpectrumCoef<T> &i_spectrum);

/**
* Returns true if all spectrum components are in the specified range (inclusive).
*/
template<typename T>
bool InRange(const SpectrumCoef<T> &i_spectrum, T i_low, T i_high);

/**
* Returns true if any of the spectrum components is NaN.
*/
template<typename T>
bool IsNaN(const SpectrumCoef<T> &i_spectrum);

/**
* Returns true if any of the spectrum components is a positive or a negative infinity.
*/
template<typename T>
bool IsInf(const SpectrumCoef<T> &i_spectrum);

typedef SpectrumCoef<float> SpectrumCoef_f;
typedef SpectrumCoef<double> SpectrumCoef_d;

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
SpectrumCoef<T>::SpectrumCoef()
  {
  m_rgb[0]=0;
  m_rgb[1]=0;
  m_rgb[2]=0;
  }

template<typename T>
SpectrumCoef<T>::SpectrumCoef(T i_value)
  {
  ASSERT(IsNaN(i_value)==false);

  m_rgb[0]=i_value;
  m_rgb[1]=i_value;
  m_rgb[2]=i_value;
  }

template<typename T>
SpectrumCoef<T>::SpectrumCoef(T i_r, T i_g, T i_b)
  {
  ASSERT(IsNaN(i_r)==false && IsNaN(i_g)==false && IsNaN(i_b)==false);

  m_rgb[0]=i_r;
  m_rgb[1]=i_g;
  m_rgb[2]=i_b;
  }

template<typename T>
SpectrumCoef<T> SpectrumCoef<T>::operator+(const SpectrumCoef<T> &i_spectrum) const
  {
  return SpectrumCoef<T>(m_rgb[0]+i_spectrum.m_rgb[0], m_rgb[1]+i_spectrum.m_rgb[1], m_rgb[2]+i_spectrum.m_rgb[2]);
  }

template<typename T>
SpectrumCoef<T> &SpectrumCoef<T>::operator+=(const SpectrumCoef<T> &i_spectrum)
  {
  m_rgb[0]+=i_spectrum.m_rgb[0];
  m_rgb[1]+=i_spectrum.m_rgb[1];
  m_rgb[2]+=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
SpectrumCoef<T> SpectrumCoef<T>::operator-(const SpectrumCoef<T> &i_spectrum) const
  {
  return SpectrumCoef<T>(m_rgb[0]-i_spectrum.m_rgb[0], m_rgb[1]-i_spectrum.m_rgb[1], m_rgb[2]-i_spectrum.m_rgb[2]);
  }

template<typename T>
SpectrumCoef<T> &SpectrumCoef<T>::operator-=(const SpectrumCoef<T> &i_spectrum)
  {
  m_rgb[0]-=i_spectrum.m_rgb[0];
  m_rgb[1]-=i_spectrum.m_rgb[1];
  m_rgb[2]-=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
SpectrumCoef<T> SpectrumCoef<T>::operator*(const SpectrumCoef<T> &i_spectrum) const
  {
  return SpectrumCoef<T>(m_rgb[0]*i_spectrum.m_rgb[0], m_rgb[1]*i_spectrum.m_rgb[1], m_rgb[2]*i_spectrum.m_rgb[2]);
  }

template<typename T>
SpectrumCoef<T> &SpectrumCoef<T>::operator*=(const SpectrumCoef<T> &i_spectrum)
  {
  m_rgb[0]*=i_spectrum.m_rgb[0];
  m_rgb[1]*=i_spectrum.m_rgb[1];
  m_rgb[2]*=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
SpectrumCoef<T> SpectrumCoef<T>::operator/(const SpectrumCoef<T> &i_spectrum) const
  {
  ASSERT(i_spectrum.m_rgb[0]!=0.0 && i_spectrum.m_rgb[1]!=0.0 && i_spectrum.m_rgb[2]!=0.0);

  return SpectrumCoef<T>(m_rgb[0]/i_spectrum.m_rgb[0], m_rgb[1]/i_spectrum.m_rgb[1], m_rgb[2]/i_spectrum.m_rgb[2]);
  }

template<typename T>
SpectrumCoef<T> &SpectrumCoef<T>::operator/=(const SpectrumCoef<T> &i_spectrum)
  {
  ASSERT(i_spectrum.m_rgb[0]!=0.0 && i_spectrum.m_rgb[1]!=0.0 && i_spectrum.m_rgb[2]!=0.0);

  m_rgb[0]/=i_spectrum.m_rgb[0];
  m_rgb[1]/=i_spectrum.m_rgb[1];
  m_rgb[2]/=i_spectrum.m_rgb[2];

  return *this;
  }

template<typename T>
SpectrumCoef<T> SpectrumCoef<T>::operator*(double i_value) const
  {
  ASSERT(IsNaN(i_value)==false);

  return SpectrumCoef<T>(
    (T) (m_rgb[0]*i_value), 
    (T) (m_rgb[1]*i_value), 
    (T) (m_rgb[2]*i_value));
  }

template<typename T>
SpectrumCoef<T> &SpectrumCoef<T>::operator*=(double i_value)
  {
  ASSERT(IsNaN(i_value)==false);

  m_rgb[0]=(T)(m_rgb[0]*i_value);
  m_rgb[1]=(T)(m_rgb[1]*i_value);
  m_rgb[2]=(T)(m_rgb[2]*i_value);
  return *this;
  }

template<typename T>
SpectrumCoef<T> SpectrumCoef<T>::operator/(double i_value) const
  {
  ASSERT(IsNaN(i_value)==false);
  ASSERT(i_value != 0.0);
  return SpectrumCoef<T>(
    (T) (m_rgb[0]/i_value), 
    (T) (m_rgb[1]/i_value), 
    (T) (m_rgb[2]/i_value));
  }

template<typename T>
SpectrumCoef<T> &SpectrumCoef<T>::operator/=(double i_value)
  {
  ASSERT(IsNaN(i_value)==false);
  ASSERT(i_value != 0.0);

  m_rgb[0]=(T)(m_rgb[0]/i_value);
  m_rgb[1]=(T)(m_rgb[1]/i_value);
  m_rgb[2]=(T)(m_rgb[2]/i_value);
  return *this;
  }

template<typename T>
bool SpectrumCoef<T>::operator==(const SpectrumCoef<T> &i_point) const
  {
  if (m_rgb[0] != i_point.m_rgb[0]) return false;
  if (m_rgb[1] != i_point.m_rgb[1]) return false;
  if (m_rgb[2] != i_point.m_rgb[2]) return false;
  return true;
  }

template<typename T>
bool SpectrumCoef<T>::operator!=(const SpectrumCoef<T> &i_point) const
  {
  return !operator==(i_point);
  }

template<typename T>
T SpectrumCoef<T>::operator[](unsigned char i_index) const
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_rgb[i_index];
  }

template<typename T>
T &SpectrumCoef<T>::operator[](unsigned char i_index)
  {
  ASSERT(i_index>=0 && i_index<3);
  return m_rgb[i_index];
  }

template<typename T>
void SpectrumCoef<T>::Clamp(T i_low, T i_high)
  {
  ASSERT(i_low<=i_high);
  m_rgb[0]=MathRoutines::Clamp(m_rgb[0], i_low, i_high);
  m_rgb[1]=MathRoutines::Clamp(m_rgb[1], i_low, i_high);
  m_rgb[2]=MathRoutines::Clamp(m_rgb[2], i_low, i_high);
  }

template<typename T>
void SpectrumCoef<T>::AddWeighted(const SpectrumCoef &i_spectrum, T i_weight)
  {
  ASSERT(IsNaN(i_weight)==false);
  m_rgb[0] += i_weight * i_spectrum.m_rgb[0];
  m_rgb[1] += i_weight * i_spectrum.m_rgb[1];
  m_rgb[2] += i_weight * i_spectrum.m_rgb[2];
  }

template<typename T>
bool SpectrumCoef<T>::IsBlack() const
  {  
  return m_rgb[0] == (T)0.0 && m_rgb[1] == (T)0.0 && m_rgb[2] == (T)0.0;
  }

template<typename T>
T SpectrumCoef<T>::Luminance() const
  {
  const T YWeight[3] = {(T)0.212671, (T)0.715160, (T)0.072169};

  return YWeight[0]*m_rgb[0] + YWeight[1]*m_rgb[1] + YWeight[2]*m_rgb[2];
  }
template<typename T1, typename T2>
SpectrumCoef<T2> operator*(T1 i_value, const SpectrumCoef<T2> &i_spectrum)
  {
  ASSERT(IsNaN(i_value)==false);
  return i_spectrum*i_value;
  }

template <class charT, class traits, typename T>
std::basic_istream<charT,traits>& operator >> (std::basic_istream<charT,traits>& i_stream, SpectrumCoef<T> &o_spectrum)
  {
  i_stream >> o_spectrum[0] >> o_spectrum[1] >> o_spectrum[2];
  return i_stream;
  }

template <class charT, class traits, typename T>
std::basic_ostream<charT,traits>& operator << (std::basic_ostream<charT,traits>& o_stream, const SpectrumCoef<T> &i_spectrum)
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
SpectrumCoef<T2> Convert(const SpectrumCoef<T> &i_spectrum)
  {
  return SpectrumCoef<T2>((T2)i_spectrum[0], (T2)i_spectrum[1], (T2)i_spectrum[2]);
  }

template<typename T>
bool InRange(const SpectrumCoef<T> &i_spectrum, T i_low, T i_high)
  {
  return
    i_spectrum[0]>=i_low && i_spectrum[0]<=i_high &&
    i_spectrum[1]>=i_low && i_spectrum[1]<=i_high &&
    i_spectrum[2]>=i_low && i_spectrum[2]<=i_high;
  }

template<typename T>
bool IsNaN(const SpectrumCoef<T> &i_spectrum)
  {
  return IsNaN(i_spectrum[0]) || IsNaN(i_spectrum[1]) || IsNaN(i_spectrum[2]);
  }

template<typename T>
bool IsInf(const SpectrumCoef<T> &i_spectrum)
  {
  return IsInf(i_spectrum[0]) || IsInf(i_spectrum[1]) || IsInf(i_spectrum[2]);
  }

/**
* Serializes SpectrumCoef to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, SpectrumCoef<T> &i_spectrum, const unsigned int i_version)
  {
  i_ar & i_spectrum[0];
  i_ar & i_spectrum[1];
  i_ar & i_spectrum[2];
  }

// Don't store class info for SpectrumCoef.
BOOST_CLASS_IMPLEMENTATION(SpectrumCoef_f, boost::serialization::object_serializable)
BOOST_CLASS_IMPLEMENTATION(SpectrumCoef_d, boost::serialization::object_serializable)

#endif // SPECTRUM_COEF_H