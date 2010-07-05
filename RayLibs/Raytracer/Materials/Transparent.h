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
    Transparent(intrusive_ptr<const Texture<Spectrum_d> > ip_reflectance, intrusive_ptr<const Texture<Spectrum_d> > ip_transmittance, double i_refractive_index);

    intrusive_ptr<const Texture<Spectrum_d> > GetReflectanceTexture() const;

    intrusive_ptr<const Texture<Spectrum_d> > GetTransmittanceTexture() const;

    double GetRefractiveIndex() const;

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
    intrusive_ptr<const Texture<Spectrum_d> > mp_reflectance, mp_transmittance;

    double m_refractive_index;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct Transparent to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const Transparent *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_reflectance = ip_material->GetReflectanceTexture();
  intrusive_ptr<const Texture<Spectrum_d> > p_transmittance = ip_material->GetTransmittanceTexture();
  double refractive_index = ip_material->GetRefractiveIndex();

  i_ar << p_reflectance;
  i_ar << p_transmittance;
  i_ar << refractive_index;
  }

/**
* Constructs Transparent with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, Transparent *ip_material, const unsigned int i_version)
  {
  intrusive_ptr<const Texture<Spectrum_d> > p_reflectance, p_transmittance;
  double refractive_index;

  i_ar >> p_reflectance;
  i_ar >> p_transmittance;
  i_ar >> refractive_index;

  ::new(ip_material)Transparent(p_reflectance, p_transmittance, refractive_index);
  }

/**
* Serializes Transparent to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, Transparent &i_material, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(i_material);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(Transparent)

#endif // TRANSPARENT_H