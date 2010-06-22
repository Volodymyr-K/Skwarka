#ifndef HOMOGENEOUS_VOLUME_REGION_H
#define HOMOGENEOUS_VOLUME_REGION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/VolumeRegion.h>

/**
* Implementation of the VolumeRegion with constant emission, absorption and scattering.
* The phase function does not depend on the point coordinates and is defined by the template parameter of the class.
*/
template<typename PhaseFunction>
class HomogeneousVolumeRegion: public VolumeRegion
  {
  public:
    /**
    * Creates HomogeneousVolumeRegion instance with specified bounding box, emission, absorption and scattering.
    * The constructor also takes ans instance of the phase function.
    */
    HomogeneousVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_emission, Spectrum_d &i_absorption, Spectrum_d &i_scattering, const PhaseFunction &i_phase_function);

    /**
    * Returns bounding box of the volume region.
    */
    BBox3D_d GetBounds() const;

    /**
    * Returns true if the ray intersects volume region and computes ray parametric coordinates of the intersection region.
    * @param i_ray Input ray. Direction component should be normalized.
    * @param op_t_begin Parametric coordinate of the begin of the intersection region. Can be NULL.
    * @param op_t_end Parametric coordinate of the end of the intersection region. Can be NULL.
    * @return true if the ray intersects the volume region.
    */
    bool Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const;

    /**
    * Returns emission density of the volume region at the specified point.
    */
    Spectrum_d Emission(const Point3D_d &i_point) const;

    /**
    * Returns absorption density of the volume region at the specified point.
    */
    Spectrum_d Absorption(const Point3D_d &i_point) const;

    /**
    * Returns scattering density of the volume region at the specified point.
    */
    Spectrum_d Scattering(const Point3D_d &i_point) const;

    /**
    * Returns attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
    */
    Spectrum_d Attenuation(const Point3D_d &i_point) const;

    /**
    * Returns value of the phase function at the specified point for the specified incoming and outgoing directions.
    * The phase function for a fixed incoming direction defines probability distribution over the sphere for the outgoing direction
    * and thus should sum up to 1.0 when integrated over the sphere.
    * @param i_point Point in the volume region.
    * @param i_incoming Incoming direction, i.e. the direction of the light ray before the scattering. Should be normalized.
    * @param i_outgoing Outgoing direction, i.e. the direction of the light ray after the scattering. Should be normalized.
    * @return PDF value. Should be greater or equal than 0.0;
    */
    double Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;

    /**
    * Returns optical thickness of the volume region for the specified ray.
    * The method also takes two additional parameters for MonteCarlo integration that are not used by this implementation though.
    * @param i_ray Ray for which the optical thickness is to be computed. Ray direction should be normalized.
    * @param i_step Step size for the MonteCarlo integration. Should be greater than 0.0. Not used by this implementation.
    * @param i_offset_sample The sample value used for MonteCarlo integration to choose position in the segments for evaluating attenuation value.
    * Should be in [0;1) range. Not used by this implementation.
    * @return Optical thickness.
    */
    Spectrum_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const;

  private:
    BBox3D_d m_bounds;

    Spectrum_d m_emission, m_absorption, m_scattering, m_attenuation;

    PhaseFunction m_phase_function;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename PhaseFunction>
HomogeneousVolumeRegion<PhaseFunction>::HomogeneousVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_emission, Spectrum_d &i_absorption,
                                                                Spectrum_d &i_scattering, const PhaseFunction &i_phase_function):
m_bounds(i_bounds), m_emission(i_emission), m_absorption(i_absorption), m_scattering(i_scattering), m_phase_function(i_phase_function)
  {
  ASSERT(InRange(i_emission, 0.0, DBL_INF));
  ASSERT(InRange(i_absorption, 0.0, DBL_INF));
  ASSERT(InRange(i_scattering, 0.0, DBL_INF));

  m_attenuation = m_absorption + m_scattering;
  }

template<typename PhaseFunction>
BBox3D_d HomogeneousVolumeRegion<PhaseFunction>::GetBounds() const
  {
  return m_bounds;
  }

template<typename PhaseFunction>
bool HomogeneousVolumeRegion<PhaseFunction>::Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const
  {
  return m_bounds.Intersect(i_ray, op_t_begin, op_t_end);
  }

template<typename PhaseFunction>
Spectrum_d HomogeneousVolumeRegion<PhaseFunction>::Emission(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_emission : Spectrum_d(0.0);
  }

template<typename PhaseFunction>
Spectrum_d HomogeneousVolumeRegion<PhaseFunction>::Absorption(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_absorption : Spectrum_d(0.0);
  }

template<typename PhaseFunction>
Spectrum_d HomogeneousVolumeRegion<PhaseFunction>::Scattering(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_scattering : Spectrum_d(0.0);
  }

template<typename PhaseFunction>
Spectrum_d HomogeneousVolumeRegion<PhaseFunction>::Attenuation(const Point3D_d &i_point) const
  {
  return m_bounds.Inside(i_point) ? m_attenuation : Spectrum_d(0.0);
  }

template<typename PhaseFunction>
double HomogeneousVolumeRegion<PhaseFunction>::Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  return m_bounds.Inside(i_point) ? m_phase_function(i_incoming, i_outgoing) : 0.0;
  }

template<typename PhaseFunction>
Spectrum_d HomogeneousVolumeRegion<PhaseFunction>::OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

  double t_begin, t_end;
  if (m_bounds.Intersect(i_ray, &t_begin, &t_end))
    return fabs(t_end-t_begin) * m_attenuation;
  else
    return Spectrum_d(0.0);
  }

#endif // HOMOGENEOUS_VOLUME_REGION_H