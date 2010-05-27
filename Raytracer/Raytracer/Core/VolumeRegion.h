#ifndef VOLUME_REGION_H
#define VOLUME_REGION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Spectrum.h"

/**
* Base abstract class for participating media.
* The class defines interface for the volume regions with emission, absorption and scattering properties.
* The participating media is assumed to be isotropic in the sense that it's properties do not depend on the ray direction.
*/
class VolumeRegion: public ReferenceCounted
  {
  public:
    /**
    * Returns bounding box of the volume region.
    */
    virtual BBox3D_d GetBounds() const = 0;

    /**
    * Returns true if the ray intersects volume region and computes ray parametric coordinates of the intersection region.
    * @param i_ray Input ray. Direction component should be normalized.
    * @param op_t_begin Parametric coordinate of the begin of the intersection region. Can be NULL.
    * @param op_t_end Parametric coordinate of the end of the intersection region. Can be NULL.
    * @return true if the ray intersects the volume region.
    */
    virtual bool Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const = 0;

    /**
    * Returns emission density of the volume region at the specified point.
    */
    virtual Spectrum_d Emission(const Point3D_d &i_point) const = 0;

    /**
    * Returns absorption density of the volume region at the specified point.
    */
    virtual Spectrum_d Absorption(const Point3D_d &i_point) const = 0;

    /**
    * Returns scattering density of the volume region at the specified point.
    */
    virtual Spectrum_d Scattering(const Point3D_d &i_point) const = 0;
    
    /**
    * Returns attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
    */
    virtual Spectrum_d Attenuation(const Point3D_d &i_point) const = 0;

    /**
    * Returns value of the phase function at the specified point for the specified incoming and outgoing directions.
    * The phase function for a fixed incoming direction defines probability distribution over the sphere for the outgoing direction
    * and thus should sum up to 1.0 when integrated over the sphere.
    * @param i_point Point in the volume region.
    * @param i_incoming Incoming direction, i.e. the direction of the light ray before the scattering. Should be normalized.
    * @param i_outgoing Outgoing direction, i.e. the direction of the light ray after the scattering. Should be normalized.
    * @return PDF value. Should be greater or equal than 0.0;
    */
    virtual double Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const = 0;

    /**
    * Returns optical thickness of the volume region for the specified ray.
    * The method also takes two additional parameters used by MonteCarlo integration if there's no analytical solution.
    * @param i_ray Ray for which the optical thickness is to be computed.
    * @param i_step Step size for the MonteCarlo integration. Should be greater than 0.0
    * @param i_offset_sample The sample value used for MonteCarlo integration to choose position in the segments for evaluating attenuation value. Should be in [0;1) range.
    * @return Optical thickness.
    */
    virtual Spectrum_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const = 0;

    virtual ~VolumeRegion() {};

  protected:
    VolumeRegion() {};

  private:
    // Not implemented, not a value type.
    VolumeRegion(const VolumeRegion&);
    VolumeRegion &operator=(const VolumeRegion&);
  };

/**
* Abstract implementation of the VolumeRegion with emission, absorption and scattering being proportional to the density of the media particles.
* The phase function does not depend on the point coordinates and is defined by the template parameter of the class.
* The class declares new virtual method _Density() that sub-classes must implement.
*/
template<typename PhaseFunction>
class DensityVolumeRegion: public VolumeRegion
  {
  public:
    /**
    * Creates DensityVolumeRegion instance with specified base emission, absorption and scattering. The real properties are evaluated by multiplying the base values by the density.
    * The constructor also takes ans instance of the phase function.
    */
    DensityVolumeRegion(Spectrum_d &i_base_emission, Spectrum_d &i_base_absorption, Spectrum_d &i_base_scattering, const PhaseFunction &i_phase_function);

    /**
    * Returns emission density of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    Spectrum_d Emission(const Point3D_d &i_point) const;

    /**
    * Returns absorption density of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    Spectrum_d Absorption(const Point3D_d &i_point) const;

    /**
    * Returns scattering density of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    Spectrum_d Scattering(const Point3D_d &i_point) const;

    /**
    * Returns attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
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
    * The method takes two additional parameters used by MonteCarlo integration.
    * @param i_ray Ray for which the optical thickness is to be computed.
    * @param i_step Step size for the MonteCarlo integration. Should be greater than 0.0
    * @param i_offset_sample The sample value used for MonteCarlo integration to choose position in the segments for evaluating attenuation value. Should be in [0;1) range.
    * @return Optical thickness.
    */
    Spectrum_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const;

  private:
    /**
    * Private virtual function for the sub-classes to implement that returns density of the media particles at the specified point.
    */
    double virtual _Density(const Point3D_d &i_point) const = 0;

  private:
    Spectrum_d m_base_emission, m_base_absorption, m_base_scattering, m_base_attenuation;

    PhaseFunction m_phase_function;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<typename PhaseFunction>
DensityVolumeRegion<PhaseFunction>::DensityVolumeRegion(Spectrum_d &i_base_emission, Spectrum_d &i_base_absorption,
                                                        Spectrum_d &i_base_scattering, const PhaseFunction &i_phase_function):
m_base_emission(i_base_emission), m_base_absorption(i_base_absorption), m_base_scattering(i_base_scattering), m_phase_function(i_phase_function)
  {
  ASSERT(InRange(i_base_emission, 0.0, DBL_INF));
  ASSERT(InRange(i_base_absorption, 0.0, DBL_INF));
  ASSERT(InRange(i_base_scattering, 0.0, DBL_INF));

  m_base_attenuation = m_base_absorption+m_base_scattering;
  }

template<typename PhaseFunction>
Spectrum_d DensityVolumeRegion<PhaseFunction>::Emission(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_emission;
  }

template<typename PhaseFunction>
Spectrum_d DensityVolumeRegion<PhaseFunction>::Absorption(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_absorption;
  }

template<typename PhaseFunction>
Spectrum_d DensityVolumeRegion<PhaseFunction>::Scattering(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_scattering;
  }

template<typename PhaseFunction>
Spectrum_d DensityVolumeRegion<PhaseFunction>::Attenuation(const Point3D_d &i_point) const
  {
  return _Density(i_point) * m_base_attenuation;
  }

template<typename PhaseFunction>
double DensityVolumeRegion<PhaseFunction>::Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  return m_phase_function(i_incoming, i_outgoing);
  }

template<typename PhaseFunction>
Spectrum_d DensityVolumeRegion<PhaseFunction>::OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const
  {
  ASSERT(i_ray.m_direction.IsNormalized());
  ASSERT(i_step > 0.0 && i_offset_sample >= 0.0 && i_offset_sample < 1.0);

  double t_begin, t_end;
  if (this->IntersectTest(i_ray, &t_begin, &t_end)==false)
    return Spectrum_d(0.0);

  double optical_thickness;
  for (double t = t_begin + i_offset_sample * i_step; t<t_end; t += i_step)
    optical_thickness += _Density( i_ray(t) );

  return (optical_thickness * i_step) * m_base_attenuation;
  }

#endif // VOLUME_REGION_H