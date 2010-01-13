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

#endif // MATERIAL_H