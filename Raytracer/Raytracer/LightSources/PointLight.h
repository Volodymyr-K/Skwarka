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
    * Initializes the light source for the given scene.
    * The method does nothing for the point light source implementation.
    */
    virtual void Initialize(intrusive_ptr<Scene> ip_scene);

    /**
    * Returns the total power of the light source, i.e. the light flux.
    */
    virtual Spectrum_d Power() const;

    /**
    * Returns the light source radiance at the specified point.
    * @param i_point Lighted point.
    * @param[out] o_lighting_ray Ray to the light source from the specified point.
    * @return Radiance value.
    */
    virtual Spectrum_d Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const;

    /**
    * Samples outgoing light ray.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SamplePhoton(const Point2D_d &i_sample, Ray &o_photon_ray, double &o_pdf) const;

  private:
    Point3D_d m_position;

    Spectrum_d m_intensity;
  };

#endif // POINT_LIGHT_H