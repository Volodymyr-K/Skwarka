#ifndef HG_PHASE_FUNCTION_H
#define HG_PHASE_FUNCTION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/PhaseFunction.h>

/**
* The parameterized phase function by Henyey and Greenstein. The scattering characteristics are defined by the parameter called "asymmetry parameter"
*/
class HGPhaseFunction: public PhaseFunction
  {
  public:
    /**
    * Creates HGPhaseFunction instance with the specified asymmetry parameter.
    * Negative values of the asymmetry parameter correspond to back-scattering while positive values correspond to forward-scattering.
    * If the asymmetry parameter has value equal to zero the phase function is the isotropic one.
    * @param i_g Asymmetry parameter. Should be in [-1;1] range.
    */
    HGPhaseFunction(double i_g);

    /**
    * Returns probability density for the light to be scattered in the specified outgoing direction given the specified incoming direction.
    */
    double ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;

  private:
    HGPhaseFunction() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    double m_g;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline HGPhaseFunction::HGPhaseFunction(double i_g): m_g(i_g)
  {
  }

inline double HGPhaseFunction::ScatteringPDF(const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const
  {
  double costheta = i_incoming*i_outgoing;
  return 1.0 / (4.0 * M_PI) * (1.0 - m_g*m_g) / pow(1.0 + m_g*m_g - 2.0 * m_g * costheta, 1.5);
  }

/**
* Serializes to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void HGPhaseFunction::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<PhaseFunction>(*this);
  i_ar & m_g;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(HGPhaseFunction)

#endif // HG_PHASE_FUNCTION_H