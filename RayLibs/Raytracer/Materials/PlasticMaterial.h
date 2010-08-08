#ifndef PLASTIC_MATERIAL_H
#define PLASTIC_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* Reflective Material that models plastic as a mixture of diffuse and glossy scattering functions.
* The glossiness of the glossy part is defined by a roughness parameter. The material is isotropic.
*/
class PlasticMaterial: public Material
  {
  public:
    /**
    * Creates PlasticMaterial instance with the specified textures defining the reflectances and roughness.
    * The sum of diffuse and glossy reflectances should not be greater than 1.0 (for all spectrum components).
    * @param ip_diffuse_reflectance The texture defining the reflectance(color) of the diffuse part. Each spectrum component should be in [0;1] range.
    * @param ip_glossy_reflectance The texture defining the reflectance of the glossy layer. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface. Values should be in [0;1] range.
    */
    PlasticMaterial(intrusive_ptr<const Texture<Spectrum_d> > ip_diffuse_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_glossy_reflectance,
      intrusive_ptr<const Texture<double> > ip_roughness);

    intrusive_ptr<const Texture<Spectrum_d> > GetDiffuseReflectanceTexture() const;

    intrusive_ptr<const Texture<Spectrum_d> > GetGlossyReflectanceTexture() const;

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
    intrusive_ptr<const Texture<Spectrum_d> > mp_diffuse_reflectance, mp_glossy_reflectance;

    intrusive_ptr<const Texture<double> > mp_roughness;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct PlasticMaterial to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const PlasticMaterial *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_diffuse_reflectance = ip_material->GetDiffuseReflectanceTexture();
  intrusive_ptr<const Texture<Spectrum_d> > p_glossy_reflectance = ip_material->GetGlossyReflectanceTexture();
  intrusive_ptr<const Texture<double> > p_roughness = ip_material->GetRoughnessTexture();

  i_ar << p_diffuse_reflectance;
  i_ar << p_glossy_reflectance;
  i_ar << p_roughness;
  }

/**
* Constructs PlasticMaterial with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, PlasticMaterial *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_diffuse_reflectance, p_glossy_reflectance;
  intrusive_ptr<const Texture<double> > p_roughness;

  i_ar >> p_diffuse_reflectance;
  i_ar >> p_glossy_reflectance;
  i_ar >> p_roughness;

  ::new(ip_material)PlasticMaterial(p_diffuse_reflectance, p_glossy_reflectance, p_roughness);
  }

/**
* Serializes PlasticMaterial to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, PlasticMaterial &i_material, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(i_material);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(PlasticMaterial)

#endif // PLASTIC_MATERIAL_H