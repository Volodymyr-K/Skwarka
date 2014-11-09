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

#ifndef UBER_MATERIAL_H
#define UBER_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* A "kitchen sink" material representing the union of many other materials.
* This is a highly parameterized material that is useful for importing different materials from other formats.
*/
class UberMaterial: public Material
  {
  public:

    /**
    * Creates UberMaterial instance with the specified textures defining the various of the material.
    * @param ip_Kd The texture defining the total diffuse reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_Ks The texture defining the total glossy reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_Kr The texture defining the total specular reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface (for the glossy component). Values should be in [0;1] range.
    * @param ip_opacity The texture defining the opacity of the surface. Each spectrum component should be in [0;1] range.
    * @param i_refractive_index The texture defining the opacity of the surface. Each spectrum component should be in [0;1] range.
    */
    UberMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kd,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Ks,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kr,
      intrusive_ptr<const Texture<double>> ip_roughness,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_opacity,
      double i_refractive_index);

    /**
    * Returns a pointer to BSDF describing local scattering properties at the specified surface point.
    * The BSDF object and all its BxDFs is allocated using the provided MemoryPool.
    * @param i_dg DifferentialGeometry instance defining the surface point.
    * @param i_triangle_index Index of the triangle in the corresponding TriangleMesh.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return A pointer to the resulting BSDF object. The object is allocated in the specified memory pool.
    */
    virtual const BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const;

  private:
    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_Kd, mp_Ks, mp_Kr, mp_opacity;
    intrusive_ptr<const Texture<double>> mp_roughness;
    double m_refractive_index;
  };

#endif // UBER_MATERIAL_H