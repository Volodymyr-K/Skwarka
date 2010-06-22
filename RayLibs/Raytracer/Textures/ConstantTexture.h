#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H

#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

/**
* The simplest texture implementation that returns a constant value everywhere.
*/
template<typename T>
class ConstantTexture: public Texture<T>
  {
  public:
    ConstantTexture(const T &i_value);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    * The implementation always returns a constant value.
    */
    virtual T Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    T m_value;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
ConstantTexture<T>::ConstantTexture(const T &i_value):
Texture(), m_value(i_value)
  {
  }

template<typename T>
T ConstantTexture<T>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  return m_value;
  }

#endif // CONSTANT_TEXTURE_H