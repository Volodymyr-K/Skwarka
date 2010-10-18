#ifndef MIX_MATERIAL_H
#define MIX_MATERIAL_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Raytracer/Core/Material.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include <Raytracer/Core/Texture.h>
#include <Raytracer/Core/BSDF.h>

/**
* Material implementation that combines two other pluggable Material implementations with user-specified weights.
* Used to "mix" two different materials based on some texture of weights.
* Can also be used with a single reference material (in this case it simply scales it).
*/
class MixMaterial: public Material
  {
  public:
    /**
    * Creates MixMaterial instance with the two specified reference materials and the texture defining the scaling weight.
    * @param ip_material1 First reference material. Should not be NULL.
    * @param ip_material2 Second reference material. Can be NULL.
    * @param ip_scale Scales texture. Each spectrum component should be in [0;1] range.
    * The first reference material will be scaled with these weights.
    * The second reference material will be scaled with weights equal to 1 minus these weights.
    */
    MixMaterial(intrusive_ptr<const Material> ip_material1, intrusive_ptr<const Material> ip_material2,
      intrusive_ptr<const Texture<SpectrumCoef_d> > ip_scale);

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
    MixMaterial() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    intrusive_ptr<const Material> mp_material1, mp_material2;

    intrusive_ptr<const Texture<SpectrumCoef_d> > mp_scale;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void MixMaterial::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Material>(*this);
  i_ar & mp_material1;
  i_ar & mp_material2;
  i_ar & mp_scale;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(MixMaterial)

#endif // MIX_MATERIAL_H