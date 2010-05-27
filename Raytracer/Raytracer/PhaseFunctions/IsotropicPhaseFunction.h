#ifndef ISOTROPIC_PHASE_FUNCTION_H
#define ISOTROPIC_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>

class IsotropicPhaseFunction
  {
  public:
    double operator()(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

double IsotropicPhaseFunction::operator()(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  return 1.0/(4.0*M_PI);
  }

#endif // ISOTROPIC_PHASE_FUNCTION_H