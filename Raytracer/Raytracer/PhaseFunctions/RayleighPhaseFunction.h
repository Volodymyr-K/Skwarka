#ifndef RAYLEIGH_PHASE_FUNCTION_H
#define RAYLEIGH_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>

/**
* Phase function implementing the Rayleigh scattering.
* Rayleigh scattering is observed on particles only slightly smaller than the wavelength of light (cigarette smoke and dust).
*/
class RayleighPhaseFunction
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double operator()(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

double RayleighPhaseFunction::operator()(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double costheta = i_incoming*i_outgoing;
  return  3.0/(16.0*M_PI) * (1.0 + costheta * costheta);
  }

#endif // RAYLEIGH_PHASE_FUNCTION_H