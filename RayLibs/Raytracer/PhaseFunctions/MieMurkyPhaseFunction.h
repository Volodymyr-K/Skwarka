#ifndef MIE_MURKY_PHASE_FUNCTION_H
#define MIE_MURKY_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* Phase function implementing the Mie scattering for dense particle density.
* Mie scattering is observed on particles roughly the same size as the wavelength of light (water droplets or fog).
*/
class MieMurkyPhaseFunction: public PhaseFunction
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double MieMurkyPhaseFunction::ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double costheta = i_incoming*i_outgoing;
  return (0.5 + 16.5 * pow(0.5 * (1.0 + costheta), 32.0)) / (4.0*M_PI);
  }

/**
* Serializes MieMurkyPhaseFunction to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, MieMurkyPhaseFunction &i_phase_function, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<PhaseFunction>(i_phase_function);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(MieMurkyPhaseFunction)

#endif // MIE_MURKY_PHASE_FUNCTION_H