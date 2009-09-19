#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include "TriangleMesh.h"
#include "Texture.h"
#include "Material.h"

/**
* Encapsulates geometrical and material properties of an object.
* The class is mostly used to map Material objects to the corresponding TriangleMesh objects.
* The class also contains a bump map that is used to perturb the shading normals before evaluating the BSDF.
*/
class Primitive: ReferenceCounted
  {
  public:
    /**
    * Creates Primitive instance with the specified TriangleMesh, Material and bump map.
    * ip_bump_map parameter can be NULL; in that case the bump mapping is not used.
    */
    Primitive(intrusive_ptr<TriangleMesh> ip_mesh, intrusive_ptr<Material> ip_material, intrusive_ptr<Texture<double> > ip_bump_map);

    intrusive_ptr<TriangleMesh> GetTriangleMesh() const;

    intrusive_ptr<Material> GetMaterial() const;

    /**
    * Returns a pointer to BSDF describing local scattering properties at the specified surface point.
    * The BSDF object and all its BxDFs is allocated using the provided MemoryPool.
    * The method first bump maps the shading normal and then redirects the call to the Material.
    * @param i_dg DifferentialGeometry instance defining the surface point.
    * @param i_triangle_index Index of the triangle in the corresponding TriangleMesh.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return A pointer to the resulting BSDF object. The object is allocated in the specified memory pool.
    */
    BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const;

  private:
    // Not implemented, not a value type.
    Primitive();
    Primitive(const Primitive&);
    Primitive &operator=(const Primitive&);

    void _Bump(const DifferentialGeometry &i_dg, size_t i_triangle_index, DifferentialGeometry &o_bumped_dg) const;

  private:
    intrusive_ptr<TriangleMesh> mp_mesh;
    intrusive_ptr<Material> mp_material;
    intrusive_ptr<Texture<double> > mp_bump_map;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Primitive::Primitive(intrusive_ptr<TriangleMesh> ip_mesh, intrusive_ptr<Material> ip_material, intrusive_ptr<Texture<double> > ip_bump_map):
mp_mesh(ip_mesh), mp_material(ip_material), mp_bump_map(ip_bump_map)
  {
  ASSERT(ip_mesh);
  ASSERT(ip_material);
  }

intrusive_ptr<TriangleMesh> Primitive::GetTriangleMesh() const
  {
  return mp_mesh;
  }

intrusive_ptr<Material> Primitive::GetMaterial() const
  {
  return mp_material;
  }

inline BSDF *Primitive::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
  {
  ASSERT(mp_mesh);
  ASSERT(mp_material);

  if (mp_bump_map)
    {
    DifferentialGeometry bumped_dg;
    _Bump(i_dg, i_triangle_index, bumped_dg);
    return mp_material->GetBSDF(bumped_dg, i_triangle_index, i_pool);
    }
  else
    return mp_material->GetBSDF(i_dg, i_triangle_index, i_pool);
  }

#endif // PRIMITIVE_H