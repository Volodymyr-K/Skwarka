#ifndef MATTE_H
#define MATTE_H

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
class Matte: public Material
  {
  public:
    /**
    * Creates Matte object with the specified textures defining the reflectivity and roughness of the material.
    * @param ip_reflectance The texture defining the total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    * @param i_sigma The texture defining the roughness of the surface. Values should be be in [0;1] range.
    */
    Matte(intrusive_ptr<const Texture<Spectrum_d> > ip_reflectance, intrusive_ptr<const Texture<double> > ip_sigma);

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
    intrusive_ptr<const Texture<Spectrum_d> > mp_reflectance;

    intrusive_ptr<const Texture<double> > mp_sigma;
  };

#endif // MATTE_H