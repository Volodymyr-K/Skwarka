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

#ifndef TRANSLUCENT_MATERIAL_H
#define TRANSLUCENT_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* A material that describes diffuse and glossy reflection and transmission through surface.
*/
class TranslucentMaterial : public Material
  {
  public:

    /**
    * Creates UberMaterial instance with the specified textures defining the various of the material.
    * @param ip_Kd The texture defining the total diffuse reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_Ks The texture defining the total glossy reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface (for the glossy component). Values should be in (0;1] range.
    * @param ip_reflectance The texture defining the reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_transmittance The texture defining the transmittance. Each spectrum component should be in [0;1] range.
    * @param i_refractive_index Refractive index of the object. Should be positive.
    */
    TranslucentMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kd,
                        intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Ks,
                        intrusive_ptr<const Texture<double>> ip_roughness,
                        intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance,
                        intrusive_ptr<const Texture<SpectrumCoef_d>> ip_transmittance,
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
    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_Kd, mp_Ks, mp_reflectance, mp_transmittance;
    intrusive_ptr<const Texture<double>> mp_roughness;
    double m_refractive_index;
  };

#endif // TRANSLUCENT_MATERIAL_H