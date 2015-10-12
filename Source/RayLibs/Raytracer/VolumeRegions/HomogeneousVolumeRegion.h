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

#ifndef HOMOGENEOUS_VOLUME_REGION_H
#define HOMOGENEOUS_VOLUME_REGION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/VolumeRegion.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* Implementation of the VolumeRegion with constant emission, absorption and scattering.
* The phase function does not depend on the point coordinates and is defined by the PhaseFunction implementation.
*/
class HomogeneousVolumeRegion: public VolumeRegion
  {
  public:
    /**
    * Creates HomogeneousVolumeRegion instance with specified bounding box, emission, absorption and scattering.
    * The constructor also takes an instance of the phase function.
    */
    HomogeneousVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_emission, SpectrumCoef_d &i_absorption, SpectrumCoef_d &i_scattering, intrusive_ptr<const PhaseFunction> ip_phase_function);

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
    bool Intersect(const Ray &i_ray, double *op_t_begin, double *op_t_end) const;

    /**
    * Returns emission density of the volume region at the specified point.
    */
    Spectrum_d Emission(const Point3D_d &i_point) const;

    /**
    * Returns absorption density of the volume region at the specified point.
    */
    SpectrumCoef_d Absorption(const Point3D_d &i_point) const;

    /**
    * Returns scattering density of the volume region at the specified point.
    */
    SpectrumCoef_d Scattering(const Point3D_d &i_point) const;

    /**
    * Returns attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
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
    * The method also takes two additional parameters for MonteCarlo integration that are not used by this implementation though.
    * @param i_ray Ray for which the optical thickness is to be computed. Ray direction should be normalized.
    * @param i_step Step size for the MonteCarlo integration. Should be greater than 0.0. Not used by this implementation.
    * @param i_offset_sample The sample value used for MonteCarlo integration to choose position in the segments for evaluating attenuation value.
    * Should be in [0;1) range. Not used by this implementation.
    * @return Optical thickness.
    */
    SpectrumCoef_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const;

  private:
    BBox3D_d m_bounds;

    Spectrum_d m_emission;
    SpectrumCoef_d m_absorption, m_scattering, m_attenuation;

    intrusive_ptr<const PhaseFunction> mp_phase_function;
  };

#endif // HOMOGENEOUS_VOLUME_REGION_H