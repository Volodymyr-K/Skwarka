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

#ifndef PLASTIC_MATERIAL_H
#define PLASTIC_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* Reflective Material that models plastic as a mixture of diffuse and glossy scattering functions.
* The glossiness of the glossy part is defined by a roughness parameter. The material is isotropic.
*/
class PlasticMaterial: public Material
  {
  public:
    /**
    * Creates PlasticMaterial instance with the specified textures defining the reflectances and roughness.
    * The sum of diffuse and glossy reflectances should not be greater than 1.0 (for all spectrum components).
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse part. Each spectrum component should be in [0;1] range.
    * @param ip_glossy_reflectance The texture defining the reflectance of the glossy layer. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    PlasticMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_diffuse_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_glossy_reflectance,
      intrusive_ptr<const Texture<double>> ip_roughness);

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
    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_diffuse_reflectance, mp_glossy_reflectance;

    intrusive_ptr<const Texture<double>> mp_roughness;
  };

#endif // PLASTIC_MATERIAL_H