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

#ifndef MIX_TEXTURE_H
#define MIX_TEXTURE_H

#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

/**
* The texture implementation that linearly interpolates between two other pluggable textures. The interpolation weight is defined by a third texture.
* The template T1 is the value type of the two textures that are being interpolated, T2 is the value type of the interpolation weight.
*/
template<typename T1, typename T2>
class MixTexture : public Texture<T1>
  {
  public:
    /**
    * Crates MixTexture with given textures.
    */
    MixTexture(intrusive_ptr<const Texture<T1>> ip_texture1, intrusive_ptr<const Texture<T1>> ip_texture2, intrusive_ptr<const Texture<T2>> ip_weight);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    virtual T1 Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    intrusive_ptr<const Texture<T1>> mp_texture1, mp_texture2;
    intrusive_ptr<const Texture<T2>> mp_weight;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
MixTexture<T1, T2>::MixTexture(intrusive_ptr<const Texture<T1>> ip_texture1, intrusive_ptr<const Texture<T1>> ip_texture2, intrusive_ptr<const Texture<T2>> ip_weight) :
Texture(), mp_texture1(ip_texture1), mp_texture2(ip_texture2), mp_weight(ip_weight)
  {
  ASSERT(ip_texture1);
  ASSERT(ip_texture2);
  ASSERT(ip_weight);
  }

template<typename T1, typename T2>
T1 MixTexture<T1, T2>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  T2 weight = mp_weight->Evaluate(i_dg, i_triangle_index);
  return static_cast<T1>(mp_texture1->Evaluate(i_dg, i_triangle_index) * (T2(1.0)-weight) + mp_texture2->Evaluate(i_dg, i_triangle_index) * weight);
  }

#endif // MIX_TEXTURE_H