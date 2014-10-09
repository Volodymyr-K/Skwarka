#ifndef MIE_HAZY_PHASE_FUNCTION_H
#define MIE_HAZY_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* Phase function implementing the Mie scattering for sparse particle density.
* Mie scattering is observed on particles roughly the same size as the wavelength of light (water droplets or fog).
*/
class MieHazyPhaseFunction: public PhaseFunction
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double MieHazyPhaseFunction::ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double costheta = i_incoming*i_outgoing;
  return (0.50 + 4.50 * pow(0.5 * (1.0 + costheta), 8.0)) / (4.0*M_PI);
  }

/**
* Serializes MieHazyPhaseFunction to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, MieHazyPhaseFunction &i_phase_function, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<PhaseFunction>(i_phase_function);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(MieHazyPhaseFunction)

#endif // MIE_HAZY_PHASE_FUNCTION_H