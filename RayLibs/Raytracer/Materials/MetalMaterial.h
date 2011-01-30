#ifndef METAL_MATERIAL_H
#define METAL_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* The glossy-reflecting material implementation that mimics scattering properties of a metal.
* The glossiness of the metal is defined by a roughness parameter which can be different for U and V directions (in this case the material is anisotropic).
* The material uses Microfacet BxDF implementation with Blinn or Anisotropic microfacet distribution. 
*/
class MetalMaterial: public Material
  {
  public:
    /**
    * Creates MetalMaterial instance with the specified textures defining the refractive coefficient, absorption and roughness.
    * The roughness is constant for U and V directions and thus the material is isotropic.
    * @param ip_refractive_index The texture defining the refractive index of the metal. Each spectrum component should be positive.
    * @param ip_absorption The texture defining the absorption of the metal. Each spectrum component should be positive.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d> > ip_refractive_index, intrusive_ptr<const Texture<SpectrumCoef_d> > ip_absorption,
      intrusive_ptr<const Texture<double> > ip_roughness);

    /**
    * Creates MetalMaterial instance with the specified textures defining the refractive coefficient, absorption and roughness.
    * The roughness can be different for U and V directions and thus the material is anisotropic.
    * @param ip_refractive_index The texture defining the refractive index of the metal. Each spectrum component should be positive.
    * @param ip_absorption The texture defining the absorption of the metal. Each spectrum component should be positive.
    * @param ip_u_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    * @param ip_v_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    */
    MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d> > ip_refractive_index, intrusive_ptr<const Texture<SpectrumCoef_d> > ip_absorption,
      intrusive_ptr<const Texture<double> > ip_u_roughness, intrusive_ptr<const Texture<double> > ip_v_roughness);

    /**
    * Creates MetalMaterial instance with the specified textures defining the reflectance and roughness.
    * The roughness is constant for U and V directions and thus the material is isotropic.
    * The refractive indices and absorptions coefficients are approximated based on the reflectance values.
    * @param ip_reflectance The texture defining the total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d> > ip_reflectance, intrusive_ptr<const Texture<double> > ip_roughness);

    /**
    * Creates MetalMaterial instance with the specified textures defining the reflectance and roughness.
    * The roughness can be different for U and V directions and thus the material is anisotropic.
    * The refractive indices and absorptions coefficients are approximated based on the reflectance values.
    * @param ip_reflectance The texture defining the total hemisphere reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_u_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    * @param ip_v_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    */
    MetalMaterial(intrusive_ptr<const Texture<SpectrumCoef_d> > ip_reflectance,
      intrusive_ptr<const Texture<double> > ip_u_roughness, intrusive_ptr<const Texture<double> > ip_v_roughness);

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
    MetalMaterial() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Texture<SpectrumCoef_d> > mp_refractive_index, mp_absorption, mp_reflectance;

    intrusive_ptr<const Texture<double> > mp_roughness, mp_u_roughness, mp_v_roughness;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void MetalMaterial::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(*this);
  i_ar & mp_refractive_index;
  i_ar & mp_absorption;
  i_ar & mp_reflectance;
  i_ar & mp_roughness;
  i_ar & mp_u_roughness;
  i_ar & mp_v_roughness;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(MetalMaterial)

#endif // METAL_MATERIAL_H