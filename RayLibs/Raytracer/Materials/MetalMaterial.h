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

    intrusive_ptr<const Texture<SpectrumCoef_d> > GetRefractiveIndexTexture() const;

    intrusive_ptr<const Texture<SpectrumCoef_d> > GetAbsoprtionTexture() const;

    intrusive_ptr<const Texture<SpectrumCoef_d> > GetReflectanceTexture() const;

    /**
    * Returns texture defining the roughness of the material.
    * If the material is anisotropic the returned texture is NULL.
    */
    intrusive_ptr<const Texture<double> > GetRoughnessTexture() const;

    /**
    * Returns texture defining the roughness of the material in U direction.
    * If the material is isotropic the returned texture is NULL.
    */
    intrusive_ptr<const Texture<double> > GetURoughnessTexture() const;

    /**
    * Returns texture defining the roughness of the material in V direction.
    * If the material is isotropic the returned texture is NULL.
    */
    intrusive_ptr<const Texture<double> > GetVRoughnessTexture() const;

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
    intrusive_ptr<const Texture<SpectrumCoef_d> > mp_refractive_index, mp_absorption, mp_reflectance;

    intrusive_ptr<const Texture<double> > mp_roughness, mp_u_roughness, mp_v_roughness;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct MetalMaterial to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const MetalMaterial *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<SpectrumCoef_d> > p_refractive_index = ip_material->GetRefractiveIndexTexture();
  intrusive_ptr<const Texture<SpectrumCoef_d> > p_absorption = ip_material->GetAbsoprtionTexture();
  intrusive_ptr<const Texture<SpectrumCoef_d> > p_reflectance = ip_material->GetReflectanceTexture();
  intrusive_ptr<const Texture<double> > p_roughness = ip_material->GetRoughnessTexture();
  intrusive_ptr<const Texture<double> > p_u_roughness = ip_material->GetURoughnessTexture();
  intrusive_ptr<const Texture<double> > p_v_roughness = ip_material->GetVRoughnessTexture();

  i_ar << p_refractive_index;
  i_ar << p_absorption;
  i_ar << p_reflectance;
  i_ar << p_roughness;
  i_ar << p_u_roughness;
  i_ar << p_v_roughness;
  }

/**
* Constructs MetalMaterial with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, MetalMaterial *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<SpectrumCoef_d> > p_refractive_index, p_absorption, p_reflectance;
  intrusive_ptr<const Texture<double> > p_roughness, p_u_roughness, p_v_roughness;

  i_ar >> p_refractive_index;
  i_ar >> p_absorption;
  i_ar >> p_reflectance;
  i_ar >> p_roughness;
  i_ar >> p_u_roughness;
  i_ar >> p_v_roughness;

  if (p_refractive_index && p_absorption)
    {
    if (p_roughness)
      ::new(ip_material)MetalMaterial(p_refractive_index, p_absorption, p_roughness);
    else
      ::new(ip_material)MetalMaterial(p_refractive_index, p_absorption, p_u_roughness, p_v_roughness);
    }
  else if (p_reflectance)
    {
    if (p_roughness)
      ::new(ip_material)MetalMaterial(p_reflectance, p_roughness);
    else
      ::new(ip_material)MetalMaterial(p_reflectance, p_u_roughness, p_v_roughness);
    }
  else
    ASSERT(0 && "Invalid data serialized for MetalMaterial.");
  }

/**
* Serializes MetalMaterial to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, MetalMaterial &i_material, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(i_material);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(MetalMaterial)

#endif // METAL_MATERIAL_H