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

    intrusive_ptr<const Texture<Spectrum_d> > GetRefractiveIndexTexture() const;

    intrusive_ptr<const Texture<Spectrum_d> > GetAbsoprtionTexture() const;

    intrusive_ptr<const Texture<Spectrum_d> > GetReflectanceTexture() const;

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
    intrusive_ptr<const Texture<Spectrum_d> > mp_refractive_index, mp_absoprtion, mp_reflectance;

    intrusive_ptr<const Texture<double> > mp_roughness;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct Metal to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const Metal *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_refractive_index = ip_material->GetRefractiveIndexTexture();
  intrusive_ptr<const Texture<Spectrum_d> > p_absoprtion = ip_material->GetAbsoprtionTexture();
  intrusive_ptr<const Texture<Spectrum_d> > p_reflectance = ip_material->GetReflectanceTexture();
  intrusive_ptr<const Texture<double> > p_roughness = ip_material->GetRoughnessTexture();

  i_ar << p_refractive_index;
  i_ar << p_absoprtion;
  i_ar << p_reflectance;
  i_ar << p_roughness;
  }

/**
* Constructs Metal with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, Metal *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_refractive_index, p_absoprtion, p_reflectance;
  intrusive_ptr<const Texture<double> > p_roughness;

  i_ar >> p_refractive_index;
  i_ar >> p_absoprtion;
  i_ar >> p_reflectance;
  i_ar >> p_roughness;

  if (p_refractive_index && p_absoprtion)
    ::new(ip_material)Metal(p_refractive_index, p_absoprtion, p_roughness);
  else if (p_reflectance)
    ::new(ip_material)Metal(p_reflectance, p_roughness);
  else
    ASSERT(0 && "Invalid data serialized for Metal material.");
  }

/**
* Serializes Metal to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, Metal &i_material, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(i_material);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(Metal)

#endif // METAL_H