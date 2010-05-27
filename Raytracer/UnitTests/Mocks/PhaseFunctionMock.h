#ifndef PHASE_FUNCTION_MOCK_H
#define PHASE_FUNCTION_MOCK_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>

/*
FunctionMock mock implementation. Always returns constant value.
*/
class PhaseFunctionMock
  {
  public:
    double operator()(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
      {
      return 1.0/(4.0*M_PI);
      }
  };

#endif // PHASE_FUNCTION_MOCK_H