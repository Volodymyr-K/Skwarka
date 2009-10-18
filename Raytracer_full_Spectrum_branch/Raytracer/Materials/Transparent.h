#ifndef TRANSPARENT_H
#define TRANSPARENT_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* The specular reflecting and refracting material implementation.
* The material only reflects and refracts light in a single direction.
*/
class Transparent: public Material
  {
  public:
    /**
    * Creates Transparent object with the specified textures defining the reflectivity and transparency of the material
    * and the specified refractive index which is assumed to be constant everywhere.
    */
    Transparent(intrusive_ptr<Texture<Spectrum_d> > ip_reflectance, intrusive_ptr<Texture<Spectrum_d> > ip_transmittance, double i_refractive_index);

    /**
    * Returns a pointer to BSDF describing local scattering properties at the specified surface point.
    * The BSDF object and all its BxDFs is allocated using the provided MemoryPool.
    * @param i_dg DifferentialGeometry instance defining the surface point.
    * @param i_triangle_index Index of the triangle in the corresponding TriangleMesh.
    * @param i_pool Memory pool object that is used for allocating the BSDF and BxDFs objects.
    * @return A pointer to the resulting BSDF object. The object is allocated in the specified memory pool.
    */
    virtual BSDF *GetBSDF(const DifferentialGeometry &i_dg, size_t i_triangle_index, MemoryPool &i_pool) const;

  private:
    intrusive_ptr<Texture<Spectrum_d> > mp_reflectance, mp_transmittance;

    double m_refractive_index;
  };

#endif // TRANSPARENT_H