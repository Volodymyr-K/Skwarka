#ifndef UBER_MATERIAL_H
#define UBER_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* A "kitchen sink" material representing the union of many other materials.
* This is a highly parameterized material that is useful for importing different materials from other formats.
*/
class UberMaterial: public Material
  {
  public:

    /**
    * Creates UberMaterial instance with the specified textures defining the various of the material.
    * @param ip_Kd The texture defining the total diffuse reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_Ks The texture defining the total glossy reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_Kr The texture defining the total specular reflectance. Each spectrum component should be in [0;1] range.
    * @param ip_roughness The texture defining the roughness of the surface (for the glossy component). Values should be in [0;1] range.
    * @param ip_opacity The texture defining the opacity of the surface. Each spectrum component should be in [0;1] range.
    * @param i_refractive_index The texture defining the opacity of the surface. Each spectrum component should be in [0;1] range.
    */
    UberMaterial(intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kd,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Ks,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_Kr,
      intrusive_ptr<const Texture<double>> ip_roughness,
      intrusive_ptr<const Texture<SpectrumCoef_d>> ip_opacity,
      double i_refractive_index);

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
    UberMaterial() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Texture<SpectrumCoef_d>> mp_Kd, mp_Ks, mp_Kr, mp_opacity;
    intrusive_ptr<const Texture<double>> mp_roughness;
    double m_refractive_index;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void UberMaterial::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(*this);
  i_ar & mp_Kd;
  i_ar & mp_Ks;
  i_ar & mp_Kr;
  i_ar & mp_opacity;
  i_ar & mp_roughness;
  i_ar & m_refractive_index;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(UberMaterial)

#endif // UBER_MATERIAL_H