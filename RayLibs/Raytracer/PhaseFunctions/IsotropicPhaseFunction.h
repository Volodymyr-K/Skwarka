#ifndef ISOTROPIC_PHASE_FUNCTION_H
#define ISOTROPIC_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* Isotropic phase function that scatters light uniformly in all directions.
*/
class IsotropicPhaseFunction: public PhaseFunction
  {
  public:
    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline double IsotropicPhaseFunction::ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  return 1.0/(4.0*M_PI);
  }

/**
* Serializes IsotropicPhaseFunction to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, IsotropicPhaseFunction &i_phase_function, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<PhaseFunction>(i_phase_function);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(IsotropicPhaseFunction)

#endif // ISOTROPIC_PHASE_FUNCTION_H