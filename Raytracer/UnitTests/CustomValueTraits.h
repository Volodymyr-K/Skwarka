#ifndef CUSTOM_VALUE_TRAITS_H
#define CUSTOM_VALUE_TRAITS_H

#include <Math/Point2D.h>
#include <Math/Point3D.h>
#include <Math/Vector2D.h>
#include <Math/Vector3D.h>
#include <sstream>

// This file defines ValueTraits specializations for point and vector 2D and 3D classes.
// This is needed to make TS_ASSERT_EQUALS() support these types.
// This file should be included into every test header that uses TS_ASSERT_EQUALS() for points or vectors.

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
      ValueTraits(const Vector3D<T> &i_point)
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
  class ValueTraits< Vector2D<T> >
    {
    public:
      ValueTraits(const Vector2D<T> &i_point)
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
  };

#endif // CUSTOM_VALUE_TRAITS_H