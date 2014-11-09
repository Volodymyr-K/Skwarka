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

#ifndef SCALE_TEXTURE_H
#define SCALE_TEXTURE_H

#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/DifferentialGeometry.h>

/**
* The texture implementation that multiplies two other pluggable textures.
* The template parameters T1 and T2 are the value types of the two pluggable textures. The multiplication of objects of these types should be an object of type T2.
*/
template<typename T1, typename T2>
class ScaleTexture: public Texture<T2>
  {
  public:
    /**
    * Crates ScaleTexture with the given textures.
    */
    ScaleTexture(intrusive_ptr<const Texture<T1>> ip_texture1, intrusive_ptr<const Texture<T2>> ip_texture2);

    /**
    * Returns the value corresponding to the specified DifferentialGeometry and triangle index.
    */
    virtual T2 Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const;

  private:
    intrusive_ptr<const Texture<T1>> mp_texture1;
    intrusive_ptr<const Texture<T2>> mp_texture2;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename T1, typename T2>
ScaleTexture<T1,T2>::ScaleTexture(intrusive_ptr<const Texture<T1>> ip_texture1, intrusive_ptr<const Texture<T2>> ip_texture2):
Texture(), mp_texture1(ip_texture1), mp_texture2(ip_texture2)
  {
  ASSERT(ip_texture1);
  ASSERT(ip_texture2);
  }

template<typename T1, typename T2>
T2 ScaleTexture<T1,T2>::Evaluate(const DifferentialGeometry &i_dg, size_t i_triangle_index) const
  {
  return static_cast<T2>( mp_texture1->Evaluate(i_dg, i_triangle_index) * mp_texture2->Evaluate(i_dg, i_triangle_index) );
  }

#endif // SCALE_TEXTURE_H