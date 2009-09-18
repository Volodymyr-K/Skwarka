#ifndef BSDF_H
#define BSDF_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "DifferentialGeometry.h"
#include "BxDF.h"
#include <Math/Util.h>

/**
* The class represents full scattering properties of a surface point.
* It is implemented as a collection of individual BxDFs each describing a certain type of scattering.
* The BxDF components are stored in the array of fixed size (the size is defined by static MAX_BXDFS_NUM constant).
*
* The class provides methods to compute the BSDF value for a given incident and exitant directions, sample BSDF value for
* an incident direction and compute total hemisphere scattering.
* Similarly to BxDF the BSDF is isotropic in the sense that its properties only depend on the mutual location of
* incident and exitant directions and do not change if both vectors are rotated around the normal simultaneously.
*
* @warning The BSDF has a specific memory management. All BSDFs are allocated in a special memory pool. When the memory is freed
* the destructor for the BSDF is not called. That means that BSDF is a lightweight class with no resources allocated (e.g. dynamical memory etc.).
* @sa BxDF
*/
class BSDF
  {
  public:
    /**
    * Creates BSDF instance for the specified surface point defines by the DifferentialGeometry.
    * The shading normal is used to defines the BSDF's local coordinate system, not the geometrical one.
    * @param i_dg DifferentialGeometry objects defining the surface point.
    * @param i_refractive_index Refractive index of the object. Should be greater than zero.
    */
    BSDF(const DifferentialGeometry &i_dg, double i_refractive_index=1.0);

    /**
    * Returns refractive index of the object.
    */
    double GetRefractiveIndex() const;

    /**
    * Adds new BxDF component to the BSDF.
    * No more than MAX_BXDFS_NUM components can be added.
    */
    void AddBxDF(BxDF *p_bxdf);

    /**
    * Returns total number of BxDF components added so far.
    */
    size_t GetComponentsNum() const;

    /**
    * Returns total number of BxDF components matching the specified flags.
    */
    size_t GetComponentsNum(BxDFType i_flags) const;

    /**
    * Returns BxDF value for the specified incident and exitant directions.
    * Only BxDF components matching specified flags are considered.
    */
    Spectrum_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, BxDFType i_flags=BSDF_ALL) const;

    /**
    * Samples BSDF value for the specified incident direction.
    * The method selects a BxDF component to be sampled based on the specified sample value. Sampling that component gives
    * the exitant direction. After the chosen component is sampled the resulting BSDF and PDF value is computed using 
    * all BxDF components that match the specified flags.
    * If the chosen component was a specular one than the other BxDFs components are not used and the BSDF and PDF value is returned
    * directly from the sampled BxDFs.
    * @param i_incident Incident direction. Should be normalized.
    * @param[out] o_exitant Exitant direction. The returned value should be normalized.
    * @param i_sample 2D sample used to sample the selected BxDFs. Should be in [0;1]^2 range.
    * @param i_component_sample A sample value used to select a BxDF component. Should be in [0;1] range.
    * @param[out] o_pdf PDF value for the sampled exitant direction. The returned value should be greater or equal than zero.
    * @param[out] o_sampled_type BxDFType of the sampled component.
    * @param i_flags Specifies the subset of BxDF components to be sampled.
    * @return Sampled BSDF value.
    */
    Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
      const Point2D_d &i_sample, double i_component_sample, double &o_pdf, BxDFType &o_sampled_type, BxDFType i_flags=BSDF_ALL) const;

    /**
    * Samples BSDF value for the specified incident direction.
    * The method is identical to the other Sample() method of the class except that the sample value is generated using
    * a random generator. Please refer to the other Sample() method for the details.
    */
    Spectrum_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant,
      double &o_pdf, BxDFType &o_sampled_type, BxDFType i_flags=BSDF_ALL) const;

    /**
    * Returns PDF value for the specified incident and exitant direction.
    * Only BxDF components matching specified flags are considered.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_exitant Exitant direction. Should be normalized.
    * @param i_flags Specifies the subset of BxDF components.
    * @return PDF value. Greater or equal than zero.
    */
    double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant, BxDFType i_flags=BSDF_ALL) const;

    /**
    * Returns total hemisphere scattering assuming a unit of light coming from the specified incident direction.
    * The method generates Latin Hypercube samples sequences for each of the BxDF components.
    * Only BxDF components matching specified flags are considered.
    * @param i_incident Incident direction. Should be normalized.
    * @param i_samples_num Number of samples to be generated for each matching BxDF component. Should be greater than zero.
    * @param i_flags Specifies the subset of BxDF components.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    Spectrum_d TotalScattering(const Vector3D_d &i_incident, size_t i_samples_num=16, BxDFType i_flags=BSDF_ALL) const;

    /**
    * Returns total hemisphere scattering assuming a light coming uniformly from the entire hemisphere.
    * The method generates Latin Hypercube samples sequences for each of the BxDF components.
    * Only BxDF components matching specified flags are considered.
    * @param i_samples_num Number of samples to be generated for each matching BxDF component. Should be greater than zero.
    * @param i_flags Specifies the subset of BxDF components.
    * @return Total scattering value. Each spectrum component will be in [0;1] range.
    */
    Spectrum_d TotalScattering(size_t i_samples_num=16, BxDFType i_flags=BSDF_ALL) const;

  private:
    /**
    * A helper method that converts a vector in the world coordinate system to the local one.
    */
    Vector3D_d WorldToLocal(const Vector3D_d &i_vector) const;

    /**
    * A helper method that converts a vector in the local coordinate system to the world one.
    */
    Vector3D_d LocalToWorld(const Vector3D_d &i_vector) const;

  private:
    static const size_t MAX_BXDFS_NUM = 8;
    BxDF *m_BxDFs[MAX_BXDFS_NUM];
    size_t m_BxDFs_num;

    // Three base vectors defining the local orthonormal coordinate system.
    Vector3D_d m_normal, m_e1, m_e2;
    Vector3D_d m_geometric_normal;
    double m_refractive_index;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline BSDF::BSDF(const DifferentialGeometry &i_dg, double i_refractive_index):
m_normal(i_dg.m_shading_normal), m_geometric_normal(i_dg.m_geometric_normal), m_refractive_index(i_refractive_index), m_BxDFs_num(0)
  {
  ASSERT(i_refractive_index>0.0);
  MathRoutines::CoordinateSystem(i_dg.m_shading_normal, m_e1, m_e2);
  }

inline double BSDF::GetRefractiveIndex() const
  {
  return m_refractive_index;
  }

inline void BSDF::AddBxDF(BxDF *p_bxdf)
  {
  ASSERT(m_BxDFs_num<MAX_BXDFS_NUM);
  m_BxDFs[m_BxDFs_num++]=p_bxdf;
  }

inline size_t BSDF::GetComponentsNum() const
  {
  return m_BxDFs_num;
  }

inline size_t BSDF::GetComponentsNum(BxDFType i_flags) const
  {
  size_t ret=0;
  for(size_t i=0;i<m_BxDFs_num;++i)
    if (m_BxDFs[i]->MatchesFlags(i_flags))
      ++ret;

  return ret;
  }

inline Vector3D_d BSDF::WorldToLocal(const Vector3D_d &i_vector) const
  {
  return Vector3D_d(i_vector*m_e1, i_vector*m_e2, i_vector*m_normal);
  }

inline Vector3D_d BSDF::LocalToWorld(const Vector3D_d &i_vector) const
  {
  return i_vector[0]*m_e1+i_vector[1]*m_e2+i_vector[2]*m_normal;
  }

#endif // BSDF_H