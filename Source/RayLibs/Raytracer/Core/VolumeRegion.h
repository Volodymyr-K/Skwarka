/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef VOLUME_REGION_H
#define VOLUME_REGION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "PhaseFunction.h"

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
    virtual bool Intersect(const Ray &i_ray, double *op_t_begin, double *op_t_end) const = 0;

    /**
    * Returns emission density of the volume region at the specified point.
    */
    virtual Spectrum_d Emission(const Point3D_d &i_point) const = 0;

    /**
    * Returns absorption density of the volume region at the specified point.
    */
    virtual SpectrumCoef_d Absorption(const Point3D_d &i_point) const = 0;

    /**
    * Returns scattering density of the volume region at the specified point.
    */
    virtual SpectrumCoef_d Scattering(const Point3D_d &i_point) const = 0;
    
    /**
    * Returns attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
    */
    virtual SpectrumCoef_d Attenuation(const Point3D_d &i_point) const = 0;

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
    * @param i_ray Ray for which the optical thickness is to be computed. Ray direction should be normalized.
    * @param i_step Step size for the MonteCarlo integration. Should be greater than 0.0
    * @param i_offset_sample The sample value used for MonteCarlo integration to choose position in the segments for evaluating attenuation value. Should be in [0;1) range.
    * @return Optical thickness.
    */
    virtual SpectrumCoef_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const = 0;

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
* The phase function does not depend on the point coordinates and is defined by the PhaseFunction implementation.
* The class declares new virtual method _Density() that sub-classes must implement.
*/
class DensityVolumeRegion: public VolumeRegion
  {
  public:
    /**
    * Returns bounding box of the volume region.
    */
    BBox3D_d GetBounds() const;

    /**
    * Returns emission density of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    Spectrum_d Emission(const Point3D_d &i_point) const;

    /**
    * Returns absorption density of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    SpectrumCoef_d Absorption(const Point3D_d &i_point) const;

    /**
    * Returns scattering density of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    SpectrumCoef_d Scattering(const Point3D_d &i_point) const;

    /**
    * Returns attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
    * The method multiplies the base value by the density of the media particles.
    */
    SpectrumCoef_d Attenuation(const Point3D_d &i_point) const;

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
    virtual SpectrumCoef_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const;


  protected:
    /**
    * Creates DensityVolumeRegion instance with specified base emission, absorption and scattering. The real properties are evaluated by multiplying the base values by the density.
    * The constructor also takes ans instance of the phase function.
    */
    DensityVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_base_emission, SpectrumCoef_d &i_base_absorption,
      SpectrumCoef_d &i_base_scattering, intrusive_ptr<const PhaseFunction> ip_phase_function);

  private:
    /**
    * Private virtual function for the sub-classes to implement that returns density of the media particles at the specified point.
    */
    virtual double _Density(const Point3D_d &i_point) const = 0;

  private:
    BBox3D_d m_bounds;
    Spectrum_d m_base_emission;
    SpectrumCoef_d m_base_absorption, m_base_scattering, m_base_attenuation;

    intrusive_ptr<const PhaseFunction> mp_phase_function;
  };

#endif // VOLUME_REGION_H