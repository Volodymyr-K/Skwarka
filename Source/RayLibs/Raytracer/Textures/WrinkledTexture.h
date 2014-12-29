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

#ifndef WRINKLED_TEXTURE_H
#define WRINKLED_TEXTURE_H

#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Mapping.h>
#include <Math/NoiseRoutines.h>

/**
* Texture implementation that generates windy-like noise values using the "turbulence" noise function.
* The texture uses pluggable Mapping3D implementation to map DifferentialGeometry to a 3D image point (needed mostly to scale the wind pattern).
*/
template<typename T>
class WrinkledTexture : public Texture<T>
  {
  public:
    /**
    * Crates WrinkledTexture with the specified 3D mapping and turbulence parameters.
    * @param i_max_octaves Max number of octaves to generate.
    * @param i_roughness The fading factor. Should be in [0;1] range.
    * @param ip_mapping 3D mapping used to map DifferentialGeometry to a 3D point. Should not be NULL.
    */
    WrinkledTexture(size_t i_max_octaves, double i_roughness, intrusive_ptr<const Mapping3D> ip_mapping);

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
    size_t m_max_octaves;
    double m_roughness;
    intrusive_ptr<const Mapping3D> mp_mapping;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T>
WrinkledTexture<T>::WrinkledTexture(size_t i_max_octaves, double i_roughness, intrusive_ptr<const Mapping3D> ip_mapping) :
Texture(), m_max_octaves(i_max_octaves), m_roughness(i_roughness), mp_mapping(ip_mapping)
  {
  }

template<typename T>
T WrinkledTexture<T>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  Point3D_d point;
  Vector3D_d dp_dx, dp_dy;
  mp_mapping->Map(i_dg, i_triangle_index, point, dp_dx, dp_dy);

  return _To_T(NoiseRoutines::Turbulence(point, dp_dx, dp_dy, m_roughness, m_max_octaves));
  }

template<typename T>
T WrinkledTexture<T>::_To_T(double i_value) const
  {
  return T(i_value);
  }

template<>
inline Spectrum_f WrinkledTexture<Spectrum_f>::_To_T(double i_value) const
  {
  return Spectrum_f(static_cast<float>(i_value));
  }

template<>
inline SpectrumCoef_f WrinkledTexture<SpectrumCoef_f>::_To_T(double i_value) const
  {
  return SpectrumCoef_f(static_cast<float>(i_value));
  }

#endif // WRINKLED_TEXTURE_H