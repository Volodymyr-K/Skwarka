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

    Point3D_d GetPosition() const;

    Spectrum_d GetIntensity() const;

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
    Point3D_d m_position;

    Spectrum_d m_intensity;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct PointLight to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const PointLight *ip_light, const unsigned int i_version)
  {
  Point3D_d position = ip_light->GetPosition();
  Spectrum_d intensity = ip_light->GetIntensity();

  i_ar << position;
  i_ar << intensity;
  }

/**
* Constructs PointLight with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, PointLight *ip_light, const unsigned int i_version)
  {
  Point3D_d position;
  Spectrum_d intensity;

  i_ar >> position;
  i_ar >> intensity;

  ::new(ip_light)PointLight(position, intensity);
  }

/**
* Serializes PointLight to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, PointLight &i_light, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<DeltaLightSource>(i_light);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(PointLight)

#endif // POINT_LIGHT_H