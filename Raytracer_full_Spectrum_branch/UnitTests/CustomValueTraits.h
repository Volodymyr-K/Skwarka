#ifndef CUSTOM_VALUE_TRAITS_H
#define CUSTOM_VALUE_TRAITS_H

#include <Math/Point2D.h>
#include <Math/Point3D.h>
#include <Math/Vector2D.h>
#include <Math/Vector3D.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Color.h>
#include <sstream>

/*
This file defines ValueTraits specializations for some of the custom classes classes.
This is needed to make TS_ASSERT_EQUALS() support these types.
It also contains assert functions for point and vector delta assertions.

This file should be included into every test header.
*/

namespace CxxTest
  {
  template<class T>
  class ValueTraits< Point3D<T> >
    {
    public:
      ValueTraits(const Point3D<T> &i_point)
        {
        std::stringstream m_sstream;
        m_sstream << "( " << i_point << " )";
        m_sstream.getline(m_buffer,256);
        }
      const char *asString() const
        {
        return m_buffer;
        }
    private:
      char m_buffer[256];
    };

  template<class T>
  class ValueTraits< Point2D<T> >
    {
    public:
      ValueTraits(const Point2D<T> &i_point)
        {
        std::stringstream m_sstream;
        m_sstream << "( " << i_point << " )";
        m_sstream.getline(m_buffer,256);
        }
      const char *asString() const
        {
        return m_buffer;
        }
    private:
      char m_buffer[256];
    };

  template<class T>
  class ValueTraits< Vector3D<T> >
    {
    public:
      ValueTraits(const Vector3D<T> &i_vector)
        {
        std::stringstream m_sstream;
        m_sstream << "( " << i_vector << " )";
        m_sstream.getline(m_buffer,256);
        }
      const char *asString() const
        {
        return m_buffer;
        }
    private:
      char m_buffer[256];
    };

  template<class T>
  class ValueTraits< Vector2D<T> >
    {
    public:
      ValueTraits(const Vector2D<T> &i_vector)
        {
        std::stringstream m_sstream;
        m_sstream << "( " << i_vector << " )";
        m_sstream.getline(m_buffer,256);
        }
      const char *asString() const
        {
        return m_buffer;
        }
    private:
      char m_buffer[256];
    };

  template<int ColorSystem, class T>
  class ValueTraits< Color<ColorSystem,T> >
    {
    public:
      ValueTraits(const Color<ColorSystem,T> &i_color)
        {
        std::stringstream m_sstream;
        m_sstream << "( " << i_color << " )";
        m_sstream.getline(m_buffer,256);
        }
      const char *asString() const
        {
        return m_buffer;
        }
    private:
      char m_buffer[256];
    };

  template<class T>
  class ValueTraits< Spectrum<T> >
    {
    public:
      ValueTraits(const Spectrum<T> &i_spectrum)
        {
        std::stringstream m_sstream;
        m_sstream << "( " << i_spectrum << " )";
        m_sstream.getline(m_buffer,256);
        }
      const char *asString() const
        {
        return m_buffer;
        }
    private:
      char m_buffer[256];
    };
  };

template<typename T>
void CustomAssertDelta(const Point3D<T> &i_point1, const Point3D<T> &i_point2, T i_delta)
  {
  TS_ASSERT_DELTA(i_point1[0],i_point2[0],i_delta);
  TS_ASSERT_DELTA(i_point1[1],i_point2[1],i_delta);
  TS_ASSERT_DELTA(i_point1[2],i_point2[2],i_delta);
  }

template<typename T>
void CustomAssertDelta(const Point2D<T> &i_point1, const Point2D<T> &i_point2, T i_delta)
  {
  TS_ASSERT_DELTA(i_point1[0],i_point2[0],i_delta);
  TS_ASSERT_DELTA(i_point1[1],i_point2[1],i_delta);
  }

template<typename T>
void CustomAssertDelta(const Vector3D<T> &i_vector1, const Vector3D<T> &i_vector2, T i_delta)
  {
  TS_ASSERT_DELTA(i_vector1[0],i_vector2[0],i_delta);
  TS_ASSERT_DELTA(i_vector1[1],i_vector2[1],i_delta);
  TS_ASSERT_DELTA(i_vector1[2],i_vector2[2],i_delta);
  }

template<typename T>
void CustomAssertDelta(const Vector2D<T> &i_vector1, const Vector2D<T> &i_vector2, T i_delta)
  {
  TS_ASSERT_DELTA(i_vector1[0],i_vector2[0],i_delta);
  TS_ASSERT_DELTA(i_vector1[1],i_vector2[1],i_delta);
  }

template<typename T>
void CustomAssertDelta(const Spectrum<T> &i_spectrum1, const Spectrum<T> &i_spectrum2, T i_delta)
  {
  TS_ASSERT_DELTA(i_spectrum1[0],i_spectrum2[0],i_delta);
  TS_ASSERT_DELTA(i_spectrum1[1],i_spectrum2[1],i_delta);
  TS_ASSERT_DELTA(i_spectrum1[2],i_spectrum2[2],i_delta);
  }

template<int ColorSystem, class T>
void CustomAssertDelta(const Color<ColorSystem,T> &i_color1, const Color<ColorSystem,T> &i_color2, T i_delta)
  {
  TS_ASSERT_DELTA(i_color1[0],i_color2[0],i_delta);
  TS_ASSERT_DELTA(i_color1[1],i_color2[1],i_delta);
  TS_ASSERT_DELTA(i_color1[2],i_color2[2],i_delta);
  }


#endif // CUSTOM_VALUE_TRAITS_H