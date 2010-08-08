#ifndef SUBSTRATE_H
#define SUBSTRATE_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* Reflective Material that models a diffuse underlying surface with a glossy specular surface above it.
* The glossiness of the specular surface is defined by a roughness parameter which can be different for U and V directions (in this case the material is anisotropic).
* The material uses FresnelBlend BxDF implementation with Blinn or Anisotropic microfacet distribution. 
*/
class Substrate: public Material
  {
  public:
    /**
    * Creates Substrate instance with the specified textures defining the reflectances and microfacet distribution.
    * The roughness is constant for U and V directions and thus the material is isotropic.
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse surface. Each spectrum component should be positive.
    * @param ip_specular_reflectance The texture defining the reflectance of the glossy layer at normal incidence. Each spectrum component should be positive.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    Substrate(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_specular_reflectance, 
      intrusive_ptr<const Texture<double> > ip_roughness);

    /**
    * Creates Substrate instance with the specified textures defining the reflectances and microfacet distribution.
    * The roughness can be different for U and V directions and thus the material is anisotropic.
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse surface. Each spectrum component should be positive.
    * @param ip_specular_reflectance The texture defining the reflectance of the glossy layer at normal incidence. Each spectrum component should be positive.
    * @param ip_u_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    * @param ip_v_roughness The texture defining the roughness of the surface in U direction. Values should be in [0;1] range.
    */
    Substrate(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_specular_reflectance, 
      intrusive_ptr<const Texture<double> > ip_u_roughness, intrusive_ptr<const Texture<double> > ip_v_roughness);

    intrusive_ptr<const Texture<Spectrum_d> > GetDiffuseReflectanceTexture() const;

    intrusive_ptr<const Texture<Spectrum_d> > GetSpecularReflectanceTexture() const;

    /**
    * Returns texture defining the roughness of the specular surface.
    * If the material is anisotropic the returned texture is NULL.
    */
    intrusive_ptr<const Texture<double> > GetRoughnessTexture() const;

    /**
    * Returns texture defining the roughness of the specular surface in U direction.
    * If the material is isotropic the returned texture is NULL.
    */
    intrusive_ptr<const Texture<double> > GetURoughnessTexture() const;

    /**
    * Returns texture defining the roughness of the specular surface in V direction.
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
    intrusive_ptr<const Texture<Spectrum_d> > mp_specular_reflectance, mp_diffuse_reflectance;

    intrusive_ptr<const Texture<double> > mp_roughness, mp_u_roughness, mp_v_roughness;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct Substrate to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const Substrate *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_diffuse_reflectance = ip_material->GetDiffuseReflectanceTexture();
  intrusive_ptr<const Texture<Spectrum_d> > p_specular_reflectance = ip_material->GetSpecularReflectanceTexture();
  intrusive_ptr<const Texture<double> > p_roughness = ip_material->GetRoughnessTexture();
  intrusive_ptr<const Texture<double> > p_u_roughness = ip_material->GetURoughnessTexture();
  intrusive_ptr<const Texture<double> > p_v_roughness = ip_material->GetVRoughnessTexture();

  i_ar << p_diffuse_reflectance;
  i_ar << p_specular_reflectance;
  i_ar << p_roughness;
  i_ar << p_u_roughness;
  i_ar << p_v_roughness;
  }

/**
* Constructs Substrate with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, Substrate *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_diffuse_reflectance, p_specular_reflectance;
  intrusive_ptr<const Texture<double> > p_roughness, p_u_roughness, p_v_roughness;

  i_ar >> p_diffuse_reflectance;
  i_ar >> p_specular_reflectance;
  i_ar >> p_roughness;
  i_ar >> p_u_roughness;
  i_ar >> p_v_roughness;

  if (p_roughness)
    ::new(ip_material)Substrate(p_diffuse_reflectance, p_specular_reflectance, p_roughness);
  else
    ::new(ip_material)Substrate(p_diffuse_reflectance, p_specular_reflectance, p_u_roughness, p_v_roughness);
  }

/**
* Serializes Substrate to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, Substrate &i_material, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(i_material);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(Substrate)

#endif // SUBSTRATE_H