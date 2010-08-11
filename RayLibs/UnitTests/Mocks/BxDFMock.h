#ifndef BXDF_MOCK_H
#define BXDF_MOCK_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/BxDF.h>

/*
BxDF mock implementation.
For specular types it behaves similarly to reflection and/or transmission.
For other types, it evaluates to a constant value everywhere in the correct hemisphere (just like the Lambertian does).
*/
class BxDFMock: public BxDF
  {
  public:
    BxDFMock(BxDFType i_type, double i_R=1.0): BxDF(i_type), m_R(i_R)
      {
      }

    SpectrumCoef_d Evaluate(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
      {
      if (IsSpecular(GetType()))
        return SpectrumCoef_d(0.0);
      else
        return m_R*SpectrumCoef_d(INV_PI);
      }

    SpectrumCoef_d Sample(const Vector3D_d &i_incident, Vector3D_d &o_exitant, const Point2D_d &i_sample, double &o_pdf) const
      {
      if (IsSpecular(GetType()))
        {
        o_exitant=i_incident;
        o_exitant[0]=-o_exitant[0];
        o_exitant[1]=-o_exitant[1];
        if (IsTransmission(GetType()))
          o_exitant[2]=-o_exitant[2];

        o_pdf=1.0;
        return SpectrumCoef_d(m_R)/fabs(i_incident[2]);
        }
      else
        return BxDF::Sample(i_incident,o_exitant,i_sample,o_pdf);
      }

    double PDF(const Vector3D_d &i_incident, const Vector3D_d &i_exitant) const
      {
      if (IsSpecular(GetType()))
        return 0.0;
      else
        return BxDF::PDF(i_incident, i_exitant);
      }

  private:
    double m_R;
  };

#endif // BXDF_MOCK_H