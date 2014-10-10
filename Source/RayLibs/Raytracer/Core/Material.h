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

#ifndef MATERIAL_H
#define MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include "DifferentialGeometry.h"
#include "BSDF.h"

/**
* Base abstract class for object's material.
* The only method returns a BSDF object that describes the local scattering properties at the specified surface point.
*/
class Material: public ReferenceCounted
  {
  public:
    /**
    * Returns a pointer to BSDF describing local scattering properties at the specified surface point.
    * The BSDF object and all its BxDFs is allocated using the provided MemoryPool.
    * @param i_dg DifferentialGeometry instance defining the surface point.
    * @param i_triangle_index Index of the triangle in the corresponding TriangleMesh.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return A pointer to the resulting BSDF object. The object is allocated in the specified memory pool.
    */
    virtual const BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const = 0;

    virtual ~Material();

  protected:
    Material();

  private:
    // Not implemented, not a value type.
    Material(const Material&);
    Material &operator=(const Material&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Material::Material()
  {
  }

inline Material::~Material()
  {
  }

/**
* Serializes Material to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, Material &i_material, const unsigned int i_version)
  {
  // Nothing to do here, everything must be serialized by the derived classes.

  // Just call the serialization for the base ReferenceCounted class.
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_material);
  }


#endif // MATERIAL_H