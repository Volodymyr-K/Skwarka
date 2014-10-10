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

#ifndef MATTE_MATERIAL_H
#define MATTE_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* The diffuse-reflecting material implementation.
* The scattering properties at a surface point is controlled by the overall reflectivity and roughness parameter.
*/
class MatteMaterial: public Material
  {
  public:
    /**
    * Creates MatteMaterial instance with the specified textures defining the reflectivity and roughness of the material.
    * @param ip_reflectance The texture defining the total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_sigma The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    MatteMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_reflectance, intrusive_ptr<const Texture<double>> ip_sigma);

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
    MatteMaterial() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_reflectance;

    intrusive_ptr<const Texture<double>> mp_sigma;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void MatteMaterial::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(*this);
  i_ar & mp_reflectance;
  i_ar & mp_sigma;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(MatteMaterial)

#endif // MATTE_MATERIAL_H