#ifndef POINT_LIGHT_H
#define POINT_LIGHT_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/LightSources.h>

/**
* Point light source with a constant intensity for all sphere directions.
*/
class PointLight: public DeltaLightSource
  {
  public:
    /**
    * Creates PointLight instance with the specified position and intensity value.
    */
    PointLight(const Point3D_d &i_position, const Spectrum_d &i_intensity);

    /**
    * Returns the total power of the light source, i.e. the light flux.
    */
    virtual Spectrum_d Power() const;

    /**
    * Returns the light source radiance at the specified point.
    * @param i_point Lighted point.
    * @param[out] o_lighting_ray Ray to the light source from the specified point. The direction component of the ray is normalized.
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
    PointLight() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    Point3D_d m_position;

    Spectrum_d m_intensity;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void PointLight::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<DeltaLightSource>(*this);
  i_ar & m_position;
  i_ar & m_intensity;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(PointLight)

#endif // POINT_LIGHT_H