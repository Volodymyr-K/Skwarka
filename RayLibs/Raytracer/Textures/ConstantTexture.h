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

    T GetTextureValue() const;

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
T ConstantTexture<T>::GetTextureValue() const
  {
  return m_value;
  }

template<typename T>
T ConstantTexture<T>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  return m_value;
  }

/**
* Saves the data which is needed to construct ConstantTexture to the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void save_construct_data(Archive &i_ar, const ConstantTexture<T> *ip_texture, const unsigned int i_version)
  {
  T value = ip_texture->GetTextureValue();

  i_ar << value;
  }

/**
* Constructs ConstantTexture with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void load_construct_data(Archive &i_ar, ConstantTexture<T> *ip_texture, const unsigned int i_version)
  {
  T value;
  i_ar >> value;
  ::new(ip_texture)ConstantTexture<T>(value);
  }

/**
* Serializes ConstantTexture to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, ConstantTexture<T> &i_texture, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Texture<T> >(i_texture);
  }

// The following code exports different specializations of the ConstantTexture template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef ConstantTexture<Spectrum_f> ConstantTexture_Spectrum_f;
typedef ConstantTexture<Spectrum_d> ConstantTexture_Spectrum_d;
typedef ConstantTexture<float> ConstantTexture_float;
typedef ConstantTexture<double> ConstantTexture_double;

BOOST_CLASS_EXPORT(ConstantTexture_Spectrum_f)
BOOST_CLASS_EXPORT(ConstantTexture_Spectrum_d)
BOOST_CLASS_EXPORT(ConstantTexture_float)
BOOST_CLASS_EXPORT(ConstantTexture_double)

#endif // CONSTANT_TEXTURE_H