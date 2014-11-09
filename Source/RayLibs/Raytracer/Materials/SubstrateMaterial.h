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

#ifndef SUBSTRATE_MATERIAL_H
#define SUBSTRATE_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* Reflective Material that models a diffuse underlying surface with a glossy specular surface above it.
* The glossiness of the specular surface is defined by a roughness parameter which can be different for U and V directions (in this case the material is anisotropic).
* The material uses FresnelBlend BxDF implementation with Blinn or Anisotropic microfacet distribution. 
*/
class SubstrateMaterial: public Material
  {
  public:
    /**
    * Creates SubstrateMaterial instance with the specified textures defining the reflectance and roughness.
    * The roughness is constant for U and V directions and thus the material is isotropic.
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse surface. Each spectrum component should be in [0;1] range.
    * @param ip_specular_reflectance The texture defining the reflectance of the glossy layer at normal incidence. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    SubstrateMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_diffuse_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_specular_reflectance, 
      intrusive_ptr<const Texture<double>> ip_roughness);

    /**
    * Creates SubstrateMaterial instance with the specified textures defining the reflectance and roughness.
    * The roughness can be different for U and V directions and thus the material is anisotropic.
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse surface. Each spectrum component should be in [0;1] range..
    * @param ip_specular_reflectance The texture defining the reflectance of the glossy layer at normal incidence. Each spectrum component should be in [0;1] range.
    * @param ip_u_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    * @param ip_v_roughness The texture defining the roughness of the surface in V direction. Values should be in [0;1] range.
    */
    SubstrateMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_diffuse_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d>> ip_specular_reflectance, 
      intrusive_ptr<const Texture<double>> ip_u_roughness, intrusive_ptr<const Texture<double>> ip_v_roughness);

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
    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_diffuse_reflectance, mp_specular_reflectance;

    intrusive_ptr<const Texture<double>> mp_roughness, mp_u_roughness, mp_v_roughness;
  };

#endif // SUBSTRATE_MATERIAL_H