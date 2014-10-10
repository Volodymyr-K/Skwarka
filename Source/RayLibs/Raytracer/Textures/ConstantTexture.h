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

#ifndef CONSTANT_TEXTURE_H
#define CONSTANT_TEXTURE_H

#include <Raytracer/Core/Spectrum.h>
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
    ConstantTexture() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

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

template<typename T>
template<class Archive>
void ConstantTexture<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Texture<T>>(*this);
  i_ar & m_value;
  }

// The following code exports different specializations of the ConstantTexture template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef ConstantTexture<Spectrum_f> ConstantTexture_Spectrum_f;
typedef ConstantTexture<Spectrum_d> ConstantTexture_Spectrum_d;
typedef ConstantTexture<SpectrumCoef_f> ConstantTexture_SpectrumCoef_f;
typedef ConstantTexture<SpectrumCoef_d> ConstantTexture_SpectrumCoef_d;
typedef ConstantTexture<float> ConstantTexture_float;
typedef ConstantTexture<double> ConstantTexture_double;

BOOST_CLASS_EXPORT_KEY(ConstantTexture_Spectrum_f)
BOOST_CLASS_EXPORT_KEY(ConstantTexture_Spectrum_d)
BOOST_CLASS_EXPORT_KEY(ConstantTexture_SpectrumCoef_f)
BOOST_CLASS_EXPORT_KEY(ConstantTexture_SpectrumCoef_d)
BOOST_CLASS_EXPORT_KEY(ConstantTexture_float)
BOOST_CLASS_EXPORT_KEY(ConstantTexture_double)

#endif // CONSTANT_TEXTURE_H