#ifndef PARALLEL_LIGHT_H
#define PARALLEL_LIGHT_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/LightSources.h>

/**
* Parallel light source that imitates lighting from a very distant object (e.g. sun).
*/
class ParallelLight: public DeltaLightSource
  {
  public:
    /**
    * Creates ParallelLight instance with the specified direction and radiance value.
    * @param i_direction Light direction. Should be normalized.
    * @param i_radiance Radiance value. 
    * @param i_world_bounds Scene bounding box.
    */
    ParallelLight(const Vector3D_d &i_direction, const Spectrum_d &i_radiance, const BBox3D_d &i_world_bounds);

    /**
    * Returns the total power of the light source, i.e. the light flux.
    * The power is estimated by computing the flux for the scene bounding box.
    */
    virtual Spectrum_d Power() const;

    /**
    * Returns the light source radiance at the specified point.
    * @param i_point Lighted point.
    * @param[out] o_lighting_ray Ray to the light source from the specified point. The direction component of the ray is normalized. The ray is unbounded.
    * @return Radiance value.
    */
    virtual Spectrum_d Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const;

    /**
    * Samples outgoing light ray.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Irradiance value.
    */
    virtual Spectrum_d SamplePhoton(const Point2D_d &i_sample, Ray &o_photon_ray, double &o_pdf) const;

  private:
    ParallelLight() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    Vector3D_d m_direction;
    BBox3D_d m_world_bounds;

    // Total projected area of the bounding box.
    double m_bbox_projected_area;

    Spectrum_d m_radiance, m_power;

    // Triangles of the scene bounding box "visible" to the light.
    Triangle3D_d m_bbox_emitting_triangles[6];

    // CDF for the triangles in m_bbox_emitting_triangles array.
    // The CDF is based on the triangle's projected area.
    double m_area_CDF[6];
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void ParallelLight::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<DeltaLightSource>(*this);
  i_ar & m_direction;
  i_ar & m_world_bounds;
  i_ar & m_bbox_projected_area;
  i_ar & m_radiance;
  i_ar & m_power;
  i_ar & m_bbox_emitting_triangles;
  i_ar & m_area_CDF;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(ParallelLight)

#endif // POINT_LIGHT_H