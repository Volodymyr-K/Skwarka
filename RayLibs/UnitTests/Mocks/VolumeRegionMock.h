#ifndef VOLUME_REGION_MOCK_H
#define VOLUME_REGION_MOCK_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/VolumeRegion.h>

/*
VolumeRegion mock implementation.
Always returns the same value everywhere (pretty much as HomogeneousVolumeRegion does).
*/
class VolumeRegionMock: public VolumeRegion
  {
  public:
    VolumeRegionMock(BBox3D_d i_bounds, Spectrum_d &i_emission, Spectrum_d &i_absorption, Spectrum_d &i_scattering):
    m_bounds(i_bounds), m_emission(i_emission), m_absorption(i_absorption), m_scattering(i_scattering)
      {
      ASSERT(InRange(i_emission, 0.0, DBL_INF));
      ASSERT(InRange(i_absorption, 0.0, DBL_INF));
      ASSERT(InRange(i_scattering, 0.0, DBL_INF));

      m_attenuation = m_absorption + m_scattering;
      }

    BBox3D_d GetBounds() const
      {
      return m_bounds;
      }

    bool Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const
      {
      return m_bounds.Intersect(i_ray, op_t_begin, op_t_end);
      }

    Spectrum_d Emission(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_emission : Spectrum_d(0.0);
      }

    Spectrum_d Absorption(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_absorption : Spectrum_d(0.0);
      }

    Spectrum_d Scattering(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_scattering : Spectrum_d(0.0);
      }

    Spectrum_d Attenuation(const Point3D_d &i_point) const
      {
      return m_bounds.Inside(i_point) ? m_attenuation : Spectrum_d(0.0);
      }

    double Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
      {
      return m_bounds.Inside(i_point) ? 1.0/(4.0*M_PI) : 0.0;
      }

    Spectrum_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
      {
      ASSERT(i_ray.m_direction.IsNormalized());
      ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

      double t_begin, t_end;
      if (m_bounds.Intersect(i_ray, &t_begin, &t_end))
        return fabs(t_end-t_begin) * m_attenuation;
      else
        return Spectrum_d(0.0);
      }


  private:
    BBox3D_d m_bounds;

    Spectrum_d m_emission, m_absorption, m_scattering, m_attenuation;
  };

#endif // VOLUME_REGION_MOCK_H