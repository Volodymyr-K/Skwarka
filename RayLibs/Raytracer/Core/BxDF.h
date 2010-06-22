#ifndef BXDF_H
#define BXDF_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Sample.h"

/**
* This enum defines the types of a BxDF implementation.
* All BxDFs can be classified by two properties:
*   - Surface interaction.
*     - Reflection.
*     - Transmission.
*   - Scattering type.
*     - Diffuse. A BxDF is diffuse if the scattering does not depend on the incident or exitant direction.
*     - Specular. A BxDF is specular if it is defined by a singular delta function.
*     - Glossy. All other scattering types.
* Each BxDF can be of either reflection or transmission type, but not both.
* Each BxDF can be either diffuse, glossy or specular.
* The enum can be used as a bit mask to define a subset of BxDF types.
* @sa BxDF
*/
enum BxDFType
  {
  BSDF_NONE             = 0,
  BSDF_REFLECTION       = 1<<0,
  BSDF_TRANSMISSION     = 1<<1,
  BSDF_DIFFUSE          = 1<<2,
  BSDF_GLOSSY           = 1<<3,
  BSDF_SPECULAR         = 1<<4,
  BSDF_ALL_TYPES        = BSDF_DIFFUSE | BSDF_GLOSSY | BSDF_SPECULAR,
  BSDF_ALL_REFLECTION   = BSDF_REFLECTION | BSDF_ALL_TYPES,
  BSDF_ALL_TRANSMISSION = BSDF_TRANSMISSION | BSDF_ALL_TYPES,
  BSDF_ALL              = BSDF_ALL_REFLECTION | BSDF_ALL_TRANSMISSION
  };

bool IsReflection(BxDFType i_type);
bool IsTransmission(BxDFType i_type);
bool IsDiffuse(BxDFType i_type);
bool IsGlossy(BxDFType i_type);
bool IsSpecular(BxDFType i_type);

/**
* An abstract class defining the contract for all BRDF and BTDF implementations (hence the name BxDF).
* The class provides methods to compute the BxDF value for a given incident and exitant directions, sample BxDF value for
* an incident direction and compute total hemisphere scattering.
* All BxDF implementation are assumed to be isotropic in the sense that their properties only depend on the mutual location of
* incident and exitant directions and do not change if both vectors are rotated around the normal simultaneously.
*
* A subtle thing that needs to be followed by the implementations is related to the difference between geometric and shading normals.
* To avoid "light leaks" and "dark spots" Evaluate() method should not check for the correct hemisphere and should return
* reasonable result even if the vectors are in different hemispheres for a reflection BxDF and the same hemisphere for a transmission BxDF.
* What is considered "reasonable" is up to the implementations.
*
* @warning The BxDFs have a specific memory management. All BxDFs are allocated in a special memory pool. When the memory is freed
* the destructors for the BxDFs are not called. That means that all BxDFs should be lightweight objects and should not allocate any
* resources that need to be freed in the destructor (e.g. dynamical memory, shared pointers etc.).
* @sa BSDF
*/
class BxDF
  {
  public:
    virtual ~BxDF();

    /**
    * Returns BxDF type.
    */
    BxDFType GetType() const;

    /**
    * Returns true if the BxDF matches the specified BxDFType bit mask.
    */
    bool MatchesFlags(BxDFType i_flags) const;

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * All specular BxDFs must return black spectrum.
    */
    virtual Spectrum_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const = 0;

    /**
    * Samples BxDF value for the specified incident direction.
    * The sampled exitant direction may be in any hemisphere, regardless of the incident direction.
    * Default implementation uses cosine hemisphere sampling for the exitant direction.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction in the correct hemisphere. The returned value should be normalized.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. The returned value should be greater or equal than zero.
    * @return Sampled BxDF value.
    */
    virtual Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * Implementation of this method should be consistent with implementation of Sample() method so that they both use the same sampling distribution.
    * All specular BxDFs must return zero PDF.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @return PDF value. Greater or equal than zero.
    */
    virtual double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a unit of light coming from the specified incident direction.
    * Default implementation uses Monte Carlo integration to estimate the total scattering value.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_samples 2D Samples sequence to be used for sampling the hemisphere. Should have at least one sample.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d TotalScattering(const Vector3D_d &i_incident, SamplesSequence2D i_samples) const;

    /**
    * Returns total scattering (i.e. fraction of scattered light) assuming a light coming uniformly from the specified hemisphere.
    * The method takes two 2D samples sequences that must have the same number of elements.
    * Default implementation uses Monte Carlo integration to estimate the total scattering value.
    * @param i_hemisphere Defines the hemisphere of the incoming light.
    * Value true corresponds to the hemisphere above XY plane (i.e. with positive Z coordinate) and
    * value false corresponds to the hemisphere below XY plane (i.e. with negative Z coordinate).
    * @param i_samples1 First samples sequence. Should have the same number of elements that i_samples2 has.
    * @param i_samples2 Second samples sequence. Should have the same number of elements that i_samples1 has.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d TotalScattering(bool i_hemisphere, SamplesSequence2D i_samples1, SamplesSequence2D i_samples2) const;

  protected:
    /**
    * Creates BxDF instance of the specified type.
    */
    BxDF(BxDFType i_type);

  private:
    const BxDFType m_type;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline bool IsReflection(BxDFType i_type)
  {
  return (i_type&BSDF_REFLECTION)!=0;
  }

inline bool IsTransmission(BxDFType i_type)
  {
  return (i_type&BSDF_TRANSMISSION)!=0;
  }

inline bool IsDiffuse(BxDFType i_type)
  {
  return (i_type&BSDF_DIFFUSE)!=0;
  }

inline bool IsGlossy(BxDFType i_type)
  {
  return (i_type&BSDF_GLOSSY)!=0;
  }

inline bool IsSpecular(BxDFType i_type)
  {
  return (i_type&BSDF_SPECULAR)!=0;
  }

inline BxDF::BxDF(BxDFType i_type):
  m_type(i_type)
  {
  // Assert that i_type is either reflection or transmission but not both.
  ASSERT(
    (IsReflection(i_type) && IsTransmission(i_type)==false) || 
    (IsReflection(i_type)==false && IsTransmission(i_type)));

  // Assert that i_type is either diffuse, glossy or specular.
  ASSERT(
    (IsDiffuse(i_type) && IsGlossy(i_type)==false && IsSpecular(i_type)==false) || 
    (IsDiffuse(i_type)==false && IsGlossy(i_type) && IsSpecular(i_type)==false) || 
    (IsDiffuse(i_type)==false && IsGlossy(i_type)==false && IsSpecular(i_type)));
  }

inline BxDF::~BxDF()
  {
  }

inline BxDFType BxDF::GetType() const
  {
  return m_type;
  }

inline bool BxDF::MatchesFlags(BxDFType i_flags) const
  {
  return (m_type & i_flags) == m_type;
  }

#endif // BXDF_H