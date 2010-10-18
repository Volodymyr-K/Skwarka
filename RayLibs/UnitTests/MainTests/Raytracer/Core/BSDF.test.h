#ifndef BSDF_TEST_H
#define BSDF_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/BSDF.h>
#include <Raytracer/Core/DifferentialGeometry.h>
#include "Mocks/BxDFMock.h"
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>

class BSDFTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      DifferentialGeometry dg;
      dg.m_shading_normal=dg.m_geometric_normal=Vector3D_d(0,0,1);
      dg.m_tangent=Vector3D_d(1,0,0);
      mp_bsdf = shared_ptr<BSDF> (new BSDF(dg));
      }

    void tearDown()
      {
      // Nothing to clear.
      }

    void test_BSDF_Constr()
      {
      DifferentialGeometry dg;
      dg.m_shading_normal=dg.m_shading_normal=Vector3D_d(0,0,1);
      dg.m_tangent=Vector3D_d(1,0,0);
      BSDF bsdf(dg, 1.4);

      TS_ASSERT_EQUALS(bsdf.GetRefractiveIndex(), 1.4);
      }

    void test_BSDF_GetComponentsNum()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY));
      BxDFMock bxdf3(BxDFType(BSDF_TRANSMISSION | BSDF_DIFFUSE));
      BxDFMock bxdf4(BxDFType(BSDF_TRANSMISSION | BSDF_GLOSSY));
      BxDFMock bxdf5(BxDFType(BSDF_TRANSMISSION | BSDF_SPECULAR));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);
      mp_bsdf->AddBxDF(&bxdf3);
      mp_bsdf->AddBxDF(&bxdf4);
      mp_bsdf->AddBxDF(&bxdf5);

      TS_ASSERT_EQUALS(mp_bsdf->GetComponentsNum(),5);
      TS_ASSERT_EQUALS(mp_bsdf->GetComponentsNum(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE)),1);
      TS_ASSERT_EQUALS(mp_bsdf->GetComponentsNum(BxDFType(BSDF_REFLECTION | BSDF_TRANSMISSION | BSDF_DIFFUSE)),2);
      TS_ASSERT_EQUALS(mp_bsdf->GetComponentsNum(BxDFType(BSDF_ALL_TRANSMISSION)),3);
      TS_ASSERT_EQUALS(mp_bsdf->GetComponentsNum(BxDFType(BSDF_ALL)),5);
      TS_ASSERT_EQUALS(mp_bsdf->GetComponentsNum(BxDFType(BSDF_NONE)),0);
      }

    void test_BSDF_Evaluate()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE));

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant=Vector3D_d(-0.5,0.5,0.5).Normalized();

      SpectrumCoef_d v1 = mp_bsdf->Evaluate(incident,exitant);

      mp_bsdf->AddBxDF(&bxdf1);
      SpectrumCoef_d v2 = mp_bsdf->Evaluate(incident,exitant);

      mp_bsdf->AddBxDF(&bxdf2);
      SpectrumCoef_d v3 = mp_bsdf->Evaluate(incident,exitant);

      TS_ASSERT(v1.IsBlack());
      TS_ASSERT(v2.IsBlack()==false);
      TS_ASSERT_EQUALS(v3,2.0*v2);
      }
    
    void test_BSDF_EvaluateWithFlags()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant=Vector3D_d(-0.5,0.5,0.5).Normalized();
      SpectrumCoef_d glossy_only = mp_bsdf->Evaluate(incident,exitant,BxDFType(BSDF_REFLECTION|BSDF_GLOSSY));
      SpectrumCoef_d all = mp_bsdf->Evaluate(incident,exitant,BxDFType(BSDF_ALL));

      TS_ASSERT(glossy_only.IsBlack()==false);
      TS_ASSERT_EQUALS(all,2.0*glossy_only);
      }

    // Adds three components one of which is specular.
    // The Sample() method is called multiple times and the resulted BSDF and PDF values are tested.
    void test_BSDF_Sample()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE), SpectrumCoef_d(0.3));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.4));
      BxDFMock bxdf3(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR), SpectrumCoef_d(0.1));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);
      mp_bsdf->AddBxDF(&bxdf3);

      bool correct=true;
      for(size_t t=0;t<100;++t)
        {
        Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
        Vector3D_d exitant;
        double pdf;
        BxDFType type;
        SpectrumCoef_d val=mp_bsdf->Sample(incident, exitant, pdf, type);

        if (IsSpecular(type))
          {
          if (val != SpectrumCoef_d(0.1/incident[2]))
            correct=false;
          if (pdf != 1.0/3)
            correct=false;
          }
        else
          {
          if (val != SpectrumCoef_d(INV_PI*0.4+INV_PI*0.3))
            correct=false;
          if (pdf != (bxdf1.PDF(incident,exitant)+bxdf2.PDF(incident,exitant)) / 3)
            correct=false;
          }
        }
      TS_ASSERT(correct);
      }

    // Tests the case when no components matches the flags.
    void test_BSDF_SampleNoMatching()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE), SpectrumCoef_d(0.3));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR), SpectrumCoef_d(0.4));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant;
      double pdf;
      BxDFType type;
      SpectrumCoef_d val=mp_bsdf->Sample(incident, exitant, pdf, type, BxDFType(BSDF_REFLECTION|BSDF_GLOSSY));

      TS_ASSERT(type==BSDF_NONE);
      TS_ASSERT_EQUALS(pdf, 0.0);
      TS_ASSERT_EQUALS(val,SpectrumCoef_d(0.0));
      }

    void test_BSDF_PDF()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE), SpectrumCoef_d(0.3));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.4));
      BxDFMock bxdf3(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR), SpectrumCoef_d(0.1));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);
      mp_bsdf->AddBxDF(&bxdf3);

      bool correct=true;
      for(size_t t=0;t<100;++t)
        {
        Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
        Vector3D_d exitant;
        double pdf;
        BxDFType type;
        SpectrumCoef_d val=mp_bsdf->Sample(incident, exitant, pdf, type);

        if (IsSpecular(type)==false)
          if (pdf != mp_bsdf->PDF(incident, exitant))
            correct=false;
        }

      TS_ASSERT(correct);
      }

    void test_BSDF_TotalScattering1()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE), SpectrumCoef_d(0.3));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.4));
      BxDFMock bxdf3(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR), SpectrumCoef_d(0.1));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);
      mp_bsdf->AddBxDF(&bxdf3);

      size_t num_samples=30;
      std::vector<Point2D_d> samples(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples,num_samples,true);

      SamplesSequence2D sequence(&samples[0], (&samples[0]) + samples.size());

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      SpectrumCoef_d total = mp_bsdf->TotalScattering(incident,sequence);

      TS_ASSERT(total[0]<=1.0 && total[1]<=1.0 && total[2]<=1.0);
      CustomAssertDelta(total, SpectrumCoef_d(0.8,0.8,0.8), 0.00001);
      }

    void test_BSDF_TotalScattering2()
      {
      BxDFMock bxdf1(BxDFType(BSDF_REFLECTION | BSDF_DIFFUSE), SpectrumCoef_d(0.3));
      BxDFMock bxdf2(BxDFType(BSDF_REFLECTION | BSDF_GLOSSY), SpectrumCoef_d(0.4));
      BxDFMock bxdf3(BxDFType(BSDF_REFLECTION | BSDF_SPECULAR), SpectrumCoef_d(0.1));

      mp_bsdf->AddBxDF(&bxdf1);
      mp_bsdf->AddBxDF(&bxdf2);
      mp_bsdf->AddBxDF(&bxdf3);

      size_t num_samples=100;
      std::vector<Point2D_d> samples1(num_samples*num_samples), samples2(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples,num_samples,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples,num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      SpectrumCoef_d total = mp_bsdf->TotalScattering(true, sequence1, sequence2);

      TS_ASSERT(total[0]<=1.0 && total[1]<=1.0 && total[2]<=1.0);
      CustomAssertDelta(total, SpectrumCoef_d(0.8,0.8,0.8), 0.01);
      }

  private:
    shared_ptr<BSDF> mp_bsdf;
  };

#endif // BSDF_TEST_H