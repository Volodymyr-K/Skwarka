#ifndef RAYLEIGH_PHASE_FUNCTION_H
#define RAYLEIGH_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* Phase function implementing the Rayleigh scattering.
* Rayleigh scattering is observed on particles only slightly smaller than the wavelength of light (cigarette smoke and dust).
*/
class RayleighPhaseFunction: public PhaseFunction
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double RayleighPhaseFunction::ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double costheta = i_incoming*i_outgoing;
  return  3.0/(16.0*M_PI) * (1.0 + costheta * costheta);
  }

/**
* Serializes RayleighPhaseFunction to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, RayleighPhaseFunction &i_phase_function, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<PhaseFunction>(i_phase_function);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(RayleighPhaseFunction)

#endif // RAYLEIGH_PHASE_FUNCTION_H