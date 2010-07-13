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
* The material uses FresnelBlend BxDF implementation with Blinn microfacet distribution. 
*/
class Substrate: public Material
  {
  public:
    /**
    * Creates Substrate instance with the specified textures defining the reflectances and microfacet distribution.
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse surface. Each spectrum component should be positive.
    * @param ip_specular_reflectance The texture defining the reflectance of the glossy layer at normal incidence. Each spectrum component should be positive.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be be in [0;1] range.
    */
    Substrate(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_specular_reflectance, 
      intrusive_ptr<const Texture<double> > ip_roughness);

    intrusive_ptr<const Texture<Spectrum_d> > GetDiffuseReflectanceTexture() const;

    intrusive_ptr<const Texture<Spectrum_d> > GetSpecularReflectanceTexture() const;

    intrusive_ptr<const Texture<double> > GetRoughnessTexture() const;

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

    intrusive_ptr<const Texture<double> > mp_roughness;
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

  i_ar << p_diffuse_reflectance;
  i_ar << p_specular_reflectance;
  i_ar << p_roughness;
  }

/**
* Constructs Substrate with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, Substrate *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_diffuse_reflectance, p_specular_reflectance;
  intrusive_ptr<const Texture<double> > p_roughness;

  i_ar >> p_diffuse_reflectance;
  i_ar >> p_specular_reflectance;
  i_ar >> p_roughness;

  ::new(ip_material)Substrate(p_diffuse_reflectance, p_specular_reflectance, p_roughness);
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