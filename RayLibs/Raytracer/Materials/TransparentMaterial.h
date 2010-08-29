#ifndef TRANSPARENT_MATERIAL_H
#define TRANSPARENT_MATERIAL_H

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
class TransparentMaterial: public Material
  {
  public:
    /**
    * Creates TransparentMaterial object with the specified textures defining the reflectivity and transparency of the material
    * and the specified refractive index which is assumed to be constant everywhere.
    */
    TransparentMaterial(intrusive_ptr<const Texture<SpectrumCoef_d> > ip_reflectance, intrusive_ptr<const Texture<SpectrumCoef_d> > ip_transmittance, double i_refractive_index);

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
    TransparentMaterial() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Texture<SpectrumCoef_d> > mp_reflectance, mp_transmittance;

    double m_refractive_index;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void TransparentMaterial::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(*this);
  i_ar & mp_reflectance;
  i_ar & mp_transmittance;
  i_ar & m_refractive_index;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(TransparentMaterial)

#endif // TRANSPARENT_MATERIAL_H