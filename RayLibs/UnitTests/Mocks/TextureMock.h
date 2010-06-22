#ifndef TEXTURE_MOCK_H
#define TEXTURE_MOCK_H

#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

/*
Texture mock implementation.
Just like the ConstantTexture it returns the same value everywhere.
*/
template<typename T>
class TextureMock: public Texture<T>
  {
  public:
    TextureMock(const T &i_value);

    virtual T Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    T m_value;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
TextureMock<T>::TextureMock(const T &i_value):
Texture(), m_value(i_value)
  {
  }

template<typename T>
T TextureMock<T>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  return m_value;
  }

#endif // TEXTURE_MOCK_H