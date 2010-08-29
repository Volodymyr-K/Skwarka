#ifndef SPOT_POINT_LIGHT_H
#define SPOT_POINT_LIGHT_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/LightSources.h>

/**
* Point light source that emits light in a cone of directions with a gradual falloff in the transition zone.
* The directions the light is emitted at are defined by two cones with the same directions : the inner cone (with smaller spread angle) and the outer cone (with larger spread angle).
* The emitted light inside the inner cone is constant and the light is not emitted outside of the outer cone.
* The light gradually falloffs outside of the inner cone and inside the outer cone.
*/
class SpotPointLight: public DeltaLightSource
  {
  public:
    /**
    * Creates SpotPointLight instance with the specified position, cone direction, intensity value and spread angles of the inner and outer cones.
    * @param i_position Position of the point light.
    * @param i_direction Direction of the cones. Should be normalized.
    * @param i_intensity Intensity of the light emitted inside the inner cone.
    * @param i_internal_cone_angle Spread angle of the inner cone (in radians). Should be in [0;PI] range. Should be lesser or equal than i_outer_cone_angle.
    * @param i_outer_cone_angle Spread angle of the outer cone (in radians). Should be in [0;PI] range. Should be greater or equal than i_internal_cone_angle.
    */
    SpotPointLight(const Point3D_d &i_position, Vector3D_d i_direction, const Spectrum_d &i_intensity, double i_internal_cone_angle, double i_outer_cone_angle);

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
    double _Falloff(double i_cos) const;

  private:
    SpotPointLight() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    Point3D_d m_position;
    Vector3D_d m_direction, m_e2, m_e3;

    Spectrum_d m_intensity, m_power;

    double m_internal_cone_angle, m_outer_cone_angle;
    double m_inner_angle_cos, m_outer_angle_cos;
    double m_inv_cos_difference;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void SpotPointLight::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<DeltaLightSource>(*this);
  i_ar & m_position;
  i_ar & m_direction;
  i_ar & m_e2;
  i_ar & m_e3;
  i_ar & m_intensity;
  i_ar & m_power;
  i_ar & m_internal_cone_angle;
  i_ar & m_outer_cone_angle;
  i_ar & m_inner_angle_cos;
  i_ar & m_outer_angle_cos;
  i_ar & m_inv_cos_difference;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(SpotPointLight)

#endif // SPOT_POINT_LIGHT_H