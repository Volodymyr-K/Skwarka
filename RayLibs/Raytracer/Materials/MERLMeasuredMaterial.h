#ifndef MERL_MEASURED_MATERIAL_H
#define MERL_MEASURED_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/BxDFs/MERLMeasured.h>

/**
* The glossy-reflecting material implementation based on measured MERL BRDF reflectance data.
*/
class MERLMeasuredMaterial: public Material
  {
  public:
    /**
    * Creates MERLMeasuredMaterial instance with the specified measured data instance.
    * @param ip_merl_measured_data MERLMeasuredData pointer. Should not be NULL.
    */
    MERLMeasuredMaterial(intrusive_ptr<const MERLMeasuredData> ip_merl_measured_data);

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
    MERLMeasuredMaterial() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes MERLMeasuredMaterial to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const MERLMeasuredData> mp_merl_measured_data;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void MERLMeasuredMaterial::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(*this);
  i_ar & mp_merl_measured_data;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(MERLMeasuredMaterial)

#endif // MERL_MEASURED_MATERIAL_H