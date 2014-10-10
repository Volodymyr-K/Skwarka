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

#ifndef WINDY_TEXTURE_H
#define WINDY_TEXTURE_H

#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Mapping.h>
#include <Math/NoiseRoutines.h>

/**
* Texture implementation that generates windy-like noise values using Fractional Brownian noise function.
* Both the wind strength and wave amplitude are computed via the noise function.
* The texture uses pluggable Mapping3D implementation to map DifferentialGeometry to a 3D image point (needed mostly to scale the wind pattern).
*/
template<typename T>
class WindyTexture : public Texture<T>
  {
  public:
    /**
    * Crates WindyTexture with the specified 3D mapping.
    * @param ip_mapping 3D mapping used to map DifferentialGeometry to a 3D point. Should not be NULL.
    */
    WindyTexture(intrusive_ptr<const Mapping3D> ip_mapping);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    virtual T Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    /**
    * Helper private method that converts double value to the target type.
    * Specializations for Spectrum_f, and SpectrumCoef_f are provided.
    * The generic template implementation simply uses T constructor.
    * This method is needed only to avoid compiler warning when implicitly converting from double to float.
    */
    T _To_T(double i_value) const;

  private:
    WindyTexture() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Mapping3D> mp_mapping;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
WindyTexture<T>::WindyTexture(intrusive_ptr<const Mapping3D> ip_mapping) :
Texture(), mp_mapping(ip_mapping)
  {
  }

template<typename T>
T WindyTexture<T>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  Point3D_d point;
  Vector3D_d dp_dx, dp_dy;
  mp_mapping->Map(i_dg, i_triangle_index, point, dp_dx, dp_dy);

  double windStrength = NoiseRoutines::FBm(point*0.1, dp_dx*0.1, dp_dy*0.1, 0.5, 3);
  double waveHeight = NoiseRoutines::FBm(point, dp_dx, dp_dy, 0.5, 6);
  return _To_T(fabs(windStrength) * waveHeight);
  }

template<typename T>
T WindyTexture<T>::_To_T(double i_value) const
  {
  return T(i_value);
  }

template<>
inline Spectrum_f WindyTexture<Spectrum_f>::_To_T(double i_value) const
  {
  return Spectrum_f(static_cast<float>(i_value));
  }

template<>
inline SpectrumCoef_f WindyTexture<SpectrumCoef_f>::_To_T(double i_value) const
  {
  return SpectrumCoef_f(static_cast<float>(i_value));
  }

template<typename T>
template<class Archive>
void WindyTexture<T>::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Texture<T>>(*this);
  i_ar & mp_mapping;
  }

// The following code exports different specializations of the WindyTexture template in the boost serialization framework.
// If you need to serialize a new specialization you have to add it here.
typedef WindyTexture<Spectrum_f> WindyTexture_Spectrum_f;
typedef WindyTexture<Spectrum_d> WindyTexture_Spectrum_d;
typedef WindyTexture<SpectrumCoef_f> WindyTexture_SpectrumCoef_f;
typedef WindyTexture<SpectrumCoef_d> WindyTexture_SpectrumCoef_d;
typedef WindyTexture<float> WindyTexture_float;
typedef WindyTexture<double> WindyTexture_double;

BOOST_CLASS_EXPORT_KEY(WindyTexture_Spectrum_f)
BOOST_CLASS_EXPORT_KEY(WindyTexture_Spectrum_d)
BOOST_CLASS_EXPORT_KEY(WindyTexture_SpectrumCoef_f)
BOOST_CLASS_EXPORT_KEY(WindyTexture_SpectrumCoef_d)
BOOST_CLASS_EXPORT_KEY(WindyTexture_float)
BOOST_CLASS_EXPORT_KEY(WindyTexture_double)

#endif // WINDY_TEXTURE_H