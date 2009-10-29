#ifndef INFINITY_LIGHT_SOURCE_MOCK_H
#define INFINITY_LIGHT_SOURCE_MOCK_H

#include <Common/Common.h>
#include <Raytracer/Core/LightSources.h>
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>

/*
InfiniteLightSource mock implementation.
Simulates constant light from the entire sphere.
*/
class InfiniteLightSourceMock: public InfiniteLightSource
  {
  public:
    InfiniteLightSourceMock(Spectrum_d i_radiance, const BBox3D_d &i_world_bounds);

    virtual Spectrum_d Radiance(const RayDifferential &i_ray) const;
    virtual Spectrum_d Power() const;
    virtual Spectrum_d SampleLighting(const Point3D_d &i_point, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const;
    virtual double LightingPDF(const Point3D_d &i_point, const Vector3D_d &i_lighting_direction) const;
    virtual Spectrum_d SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const;
    virtual Spectrum_d Irradiance(const Vector3D_d &i_normal) const;
    virtual Spectrum_d Irradiance() const;

  private:
    Spectrum_d m_radiance;
    BBox3D_d m_world_bounds;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline InfiniteLightSourceMock::InfiniteLightSourceMock(Spectrum_d i_radiance, const BBox3D_d &i_world_bounds):
  m_radiance(i_radiance), m_world_bounds(i_world_bounds)
  {
  }

inline Spectrum_d InfiniteLightSourceMock::Radiance(const RayDifferential &i_ray) const
  {
  return m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::Power() const
  {
  return (M_PI*m_world_bounds.Area())*m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::SampleLighting(const Point3D_d &i_point, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const
  {
  o_lighting_ray=Ray(i_point,SamplingRoutines::UniformSphereSampling(i_sample));
  o_pdf=SamplingRoutines::UniformSpherePDF();
  return m_radiance;
  }

inline double InfiniteLightSourceMock::LightingPDF(const Point3D_d &i_point, const Vector3D_d &i_lighting_direction) const
  {
  return SamplingRoutines::UniformSpherePDF();
  }

inline Spectrum_d InfiniteLightSourceMock::SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  double x = fabs(m_world_bounds.m_max[0]-m_world_bounds.m_min[0]);
  double y = fabs(m_world_bounds.m_max[1]-m_world_bounds.m_min[1]);
  double z = fabs(m_world_bounds.m_max[2]-m_world_bounds.m_min[2]);
  double radius = 0.5*sqrt(x*x+y*y+z*z);
  Point3D_d center = (m_world_bounds.m_max+m_world_bounds.m_min)/2.0;

  Point3D_d ray_origin = center+SamplingRoutines::UniformSphereSampling(i_position_sample)*radius;
  Vector3D_d ray_direction = SamplingRoutines::UniformSphereSampling(i_direction_sample);

  // Revert ray direction to point inside the world sphere.
  if (ray_direction * Vector3D_d(ray_origin-center) > 0.0)
    ray_direction *= -1.0;

  o_photon_ray=Ray(ray_origin, ray_direction);
  o_pdf=2.0*SamplingRoutines::UniformSpherePDF()/(M_PI*radius*radius);
  return m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::Irradiance(const Vector3D_d &i_normal) const
  {
  return M_PI*m_radiance;
  }

inline Spectrum_d InfiniteLightSourceMock::Irradiance() const
  {
  return 4.0*M_PI*m_radiance;
  }

#endif // INFINITY_LIGHT_SOURCE_MOCK_H