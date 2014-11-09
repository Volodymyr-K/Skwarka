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

#ifndef MIX_MATERIAL_H
#define MIX_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* Material implementation that combines two other pluggable Material implementations with user-specified weights.
* Used to "mix" two different materials based on some texture of weights.
* Can also be used with a single reference material (in this case it simply scales it).
*/
class MixMaterial: public Material
  {
  public:
    /**
    * Creates MixMaterial instance with the two specified reference materials and the texture defining the scaling weight.
    * @param ip_material1 First reference material. Should not be NULL.
    * @param ip_material2 Second reference material. Can be NULL.
    * @param ip_scale Scales texture. Each spectrum component should be in [0;1] range.
    * The first reference material will be scaled with these weights.
    * The second reference material will be scaled with weights equal to 1 minus these weights.
    */
    MixMaterial(intrusive_ptr<const Material> ip_material1, intrusive_ptr<const Material> ip_material2,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_scale);

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
    intrusive_ptr<const Material> mp_material1, mp_material2;

    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_scale;
  };

#endif // MIX_MATERIAL_H