#ifndef PRIMITIVE_H
#define PRIMITIVE_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "TriangleMesh.h"
#include "LightSources.h"
#include "Spectrum.h"
#include "Texture.h"
#include "Material.h"
#include "BSDF.h"

/**
* Encapsulates geometrical, material and light emitting properties of an object.
* The class is mostly used to map Material and AreaLightSource objects to the corresponding TriangleMesh objects.
* The class also contains a bump map that is used to perturb the shading normals before evaluating the BSDF.
*/
class Primitive: public ReferenceCounted
  {
  public:
    /**
    * Creates Primitive instance with the specified TriangleMesh, Material and bump map.
    * ip_bump_map parameter can be NULL; in that case the bump mapping is not used.
    * @param ip_mesh TriangleMesh describing shape of the primitive. Should not be null.
    * @param ip_material Material describing scattering properties of the primitive. Should not be null.
    * @param ip_area_light_source AreaLightSource describing light emitting properties of the primitive.
    * Can be null (in this case the primitive does not emit any light by itself).
    * @param ip_bump_map Bump map. Can be null.
    */
    Primitive(intrusive_ptr<const TriangleMesh> ip_mesh, intrusive_ptr<const Material> ip_material,
      intrusive_ptr<const AreaLightSource> ip_area_light_source = NULL, intrusive_ptr<const Texture<double> > ip_bump_map = NULL);

    /**
    * Returns a pointer to the TriangleMesh the primitive is associated with.
    */
    intrusive_ptr<const TriangleMesh> GetTriangleMesh() const;

    /**
    * Returns a raw pointer to the TriangleMesh the primitive is associated with.
    * @warning The calling code should never utilize the pointer after the Primitive is destroyed.
    */
    const TriangleMesh *GetTriangleMesh_RawPtr() const;

    /**
    * Returns a pointer to the Material the primitive is associated with.
    */
    intrusive_ptr<const Material> GetMaterial() const;

    /**
    * Returns a raw pointer to the Material the primitive is associated with.
    * @warning The calling code should never utilize the pointer after the Primitive is destroyed.
    */
    const Material *GetMaterial_RawPtr() const;

    /**
    * Returns a pointer to the AreaLightSource the primitive is associated with or NULL if the primitive does not have emission properties.
    */
    intrusive_ptr<const AreaLightSource> GetAreaLightSource() const;

    /**
    * Returns a raw pointer to the AreaLightSource the primitive is associated with or NULL if the primitive does not have emission properties.
    * @warning The calling code should never utilize the pointer after the Primitive is destroyed.
    */
    const AreaLightSource *GetAreaLightSource_RawPtr() const;

    /**
    * Returns a pointer to BSDF describing local scattering properties at the specified surface point.
    * The BSDF object and all its BxDFs is allocated using the provided MemoryPool.
    * The method first bump maps the shading normal and then redirects the call to the Material.
    * @param i_dg DifferentialGeometry instance defining the surface point.
    * @param i_triangle_index Index of the triangle in the corresponding TriangleMesh.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return A pointer to the resulting BSDF object. The object is allocated in the specified memory pool.
    */
    const BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const;

    /**
    * Returns self-emittance of the primitive at the specified surface point.
    * @param i_dg DifferentialGeometry object defining the surface point.
    * @param i_triangle_index Index of the mesh triangle for which the light radiance is to be computed.
    * @param i_light_direction Direction of the light. Should be normalized.
    * @return Light radiance.
    */
    Spectrum_d SelfEmittance(const DifferentialGeometry &i_dg, size_t i_triangle_index, const Vector3D_d &i_light_direction) const;

  private:
    // Not implemented, not a value type.
    Primitive();
    Primitive(const Primitive&);
    Primitive &operator=(const Primitive&);

    /**
    * The helper private method that bump maps the shading normal based on the bump map texture.
    * The method should only be called if mp_bump_map is not NULL.
    */
    void _Bump(const DifferentialGeometry &i_dg, size_t i_triangle_index, DifferentialGeometry &o_bumped_dg) const;

  private:
    intrusive_ptr<const TriangleMesh> mp_mesh;
    intrusive_ptr<const Material> mp_material;
    intrusive_ptr<const Texture<double> > mp_bump_map;
    intrusive_ptr<const AreaLightSource> mp_area_light_source;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Primitive::Primitive(intrusive_ptr<const TriangleMesh> ip_mesh, intrusive_ptr<const Material> ip_material,
                            intrusive_ptr<const AreaLightSource> ip_area_light_source, intrusive_ptr<const Texture<double> > ip_bump_map):
mp_mesh(ip_mesh), mp_material(ip_material), mp_area_light_source(ip_area_light_source), mp_bump_map(ip_bump_map)
  {
  ASSERT(ip_mesh);
  ASSERT(ip_material);
  }

inline intrusive_ptr<const TriangleMesh> Primitive::GetTriangleMesh() const
  {
  return mp_mesh;
  }

inline const TriangleMesh *Primitive::GetTriangleMesh_RawPtr() const
  {
  return mp_mesh.get();
  }

inline intrusive_ptr<const Material> Primitive::GetMaterial() const
  {
  return mp_material;
  }

inline const Material *Primitive::GetMaterial_RawPtr() const
  {
  return mp_material.get();
  }

inline intrusive_ptr<const AreaLightSource> Primitive::GetAreaLightSource() const
  {
  return mp_area_light_source;
  }

inline const AreaLightSource *Primitive::GetAreaLightSource_RawPtr() const
  {
  return mp_area_light_source.get();
  }

inline const BSDF *Primitive::GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const
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

inline Spectrum_d Primitive::SelfEmittance(const DifferentialGeometry &i_dg, size_t i_triangle_index, const Vector3D_d &i_light_direction) const
  {
  ASSERT(i_light_direction.IsNormalized());

  if (mp_area_light_source)
    return mp_area_light_source->Radiance(i_dg, i_triangle_index, i_light_direction);
  else
    return Spectrum_d(0.0);
  }

#endif // PRIMITIVE_H