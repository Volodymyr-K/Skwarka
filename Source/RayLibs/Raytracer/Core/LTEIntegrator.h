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

#ifndef LTE_INTEGRATOR_H
#define LTE_INTEGRATOR_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "CoreCommon.h"
#include "Spectrum.h"
#include "Scene.h"
#include "Sample.h"
#include "Sampler.h"
#include "Intersection.h"

/**
* An abstract class defining the contract for the Light Transport Equation (LTE) integrator implementations.
* An LTE integrator is an algorithm for numerical evaluation of the light transport equation taking into account both the surface scattering and media scattering.
*/
class LTEIntegrator: public ReferenceCounted
  {
  public:
    /**
    * Computes radiance along the specified ray.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences. Can be NULL.
    * @param i_ts Thread specifics (memory pool, random number generator etc.).
    * @return Resulting radiance value.
    */
    Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, ThreadSpecifics i_ts) const;

    /**
    * Requests 1D and 2D samples sequences needed for the LTE integrator.
    * This method should be called before the Radiance() method is called.
    *
    * The method calls private virtual _RequestSamples() method for surface integration samples.
    */
    void RequestSamples(intrusive_ptr<Sampler> ip_sampler);

    virtual ~LTEIntegrator();

  protected:
    /**
    * Creates LTEIntegrator instance with the specified scene.
    * @param ip_scene Scene instance. Should not be NULL.
    */
    LTEIntegrator(intrusive_ptr<const Scene> ip_scene);

    /**
    * The method for derived classes to compute radiance for the specularly reflected ray.
    * The method calls Radiance() method to compute the radiance which results in a recursive call.
    */
    Spectrum_d _SpecularReflect(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, ThreadSpecifics i_ts) const;

    /**
    * The method for derived classes to compute radiance for the specularly transmitted ray.
    * The method calls Radiance() method to compute the radiance which results in a recursive call.
    */
    Spectrum_d _SpecularTransmit(const RayDifferential &i_ray, const Intersection &i_intersection, const BSDF *ip_bsdf, const Sample *ip_sample, ThreadSpecifics i_ts) const;

  private:
    /**
    * Computes surface radiance along the specified ray.
    * The method will only be called when the ray does intersect some primitive in the scene.
    * This is a virtual private method that derived classes must implement. This method is called by non-virtual public Radiance() method (Template Method pattern).
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param i_intersection Intersection of the specified ray with the nearest primitive in the scene.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be NULL, in this case this is up to the implementation how to generate sample values.
    * @param i_ts Thread specifics (memory pool, random number generator etc.).
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _SurfaceRadiance(const RayDifferential &i_ray, const Intersection &i_intersection, const Sample *ip_sample, ThreadSpecifics i_ts) const = 0;

    /**
    * Computes media radiance and transmittance for the specified ray.
    * This is a virtual private method that derived classes must implement. This method is called by non-virtual public Radiance() method (Template Method pattern).
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be NULL, in this case this is up to the implementation how to generate sample values.
    * @param o_transmittance Resulting transmittance value. All spectrum components will be in [0;1] range.
    * @param i_ts Thread specifics (memory pool, random number generator etc.).
    * @return Resulting radiance value.
    */
    virtual Spectrum_d _MediaRadianceAndTranmsittance(const Ray &i_ray, const Sample *ip_sample, SpectrumCoef_d &o_transmittance, ThreadSpecifics i_ts) const = 0;

    /**
    * Requests 1D and 2D samples sequences needed for the surface part of the LTE integration.
    * This is a virtual private method that derived classes can implement. This method is called by non-virtual public RequestSamples() method (NVI pattern).
    * Default implementation does nothing.
    */
    virtual void _RequestSamples(intrusive_ptr<Sampler> ip_sampler);

  private:
    // Not implemented, not a value type.
    LTEIntegrator(const LTEIntegrator&);
    LTEIntegrator &operator=(const LTEIntegrator&);

  private:
    intrusive_ptr<const Scene> mp_scene;
  };

#endif // LTE_INTEGRATOR_H