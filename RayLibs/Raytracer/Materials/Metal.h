#ifndef METAL_H
#define METAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* The glossy-reflecting material implementation that mimics scattering properties of a metal.
* The material uses Microfacet BxDF implementation with Blinn microfacet distribution. 
*/
class Metal: public Material
  {
  public:
    /**
    * Creates Metal instance with the specified textures defining the refractive coefficient, absorption and roughness.
    * @param ip_refractive_index The texture defining the refractive index of the metal. Each spectrum component should be positive.
    * @param ip_absoprtion The texture defining the absorption of the metal. Each spectrum component should be positive.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be be in [0;1] range.
    */
    Metal(intrusive_ptr<const Texture<Spectrum_d> > ip_refractive_index, intrusive_ptr<const Texture<Spectrum_d> > ip_absoprtion,
      intrusive_ptr<const Texture<double> > ip_roughness);

    /**
    * Creates Metal instance with the specified textures defining the reflectance and roughness.
    * The refractive indices and absorptions coefficients are approximated based on the reflectance values.
    * @param ip_reflectance The texture defining the total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be be in [0;1] range.
    */
    Metal(intrusive_ptr<const Texture<Spectrum_d> > ip_reflectance, intrusive_ptr<const Texture<double> > ip_roughness);

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
    intrusive_ptr<const Texture<Spectrum_d> > mp_refractive_index, mp_absoprtion, mp_reflectance;

    intrusive_ptr<const Texture<double> > mp_roughness;
  };

#endif // METAL_H