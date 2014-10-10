/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MERL_MEASURED_TEST_H
#define MERL_MEASURED_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Math/ThreadSafeRandom.h>
#include <Raytracer/BxDFs/MERLMeasured.h>
#include <Math/SamplingRoutines.h>
#include <sstream>
#include <fstream>

class MERLMeasuredTestSuite : public CxxTest::TestSuite
  {
  public:
    void setUp()
      {
      if (mp_lambertian_data == NULL)
        {
        mp_lambertian_data = _CreateLambertianMERLData();
        mp_tungsten_carbide_data = _LoadMERLDataFromFile("TestData/MERLMeasured/tungsten-carbide.binary");

        mp_lambertian.reset( new MERLMeasured(mp_lambertian_data.get()) );
        mp_tungsten_carbide.reset( new MERLMeasured(mp_tungsten_carbide_data.get()) );
        }
      }

    void test_MERLMeasured_Type()
      {
      TS_ASSERT(mp_tungsten_carbide->GetType() == (BSDF_REFLECTION | BSDF_GLOSSY));
      }

    void test_MERLMeasured_EvaluateLambertian()
      {
      Vector3D_d v1=Vector3D_d(1,2,3).Normalized();
      Vector3D_d v2=Vector3D_d(-2,0.5,1).Normalized();
      SpectrumCoef_d brdf = mp_lambertian->Evaluate(v1,v2);
      CustomAssertDelta(brdf, SpectrumCoef_d(INV_PI), (1e-3)); // So large epsilon value is due to the inaccurate HalfFloat type being used by MERLMeasuredData.
      }

    void test_MERLMeasured_Reciprocity()
      {
      Vector3D_d v1=Vector3D_d(0.2,0.5,0.8).Normalized();
      Vector3D_d v2=Vector3D_d(-0.1,-0.3,0.9).Normalized();
      SpectrumCoef_d val1 = mp_tungsten_carbide->Evaluate(v1, v2);
      SpectrumCoef_d val2 = mp_tungsten_carbide->Evaluate(v2, v1);

      TS_ASSERT_EQUALS(val1,val2);
      }

    void test_MERLMeasured_Sample()
      {
      size_t num_samples=1000;

      bool correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        SpectrumCoef_d sp = mp_tungsten_carbide->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (pdf<0.0)
          correct=false;
        if (sp[0]<0.0 || sp[1]<0.0 || sp[2]<0.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    void test_MERLMeasured_PDF()
      {
      size_t num_samples=1000;

      bool pdf_correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
        Vector3D_d exitant;
        SpectrumCoef_d sp = mp_tungsten_carbide->Sample(incident, exitant, sample, pdf);

        double pdf2 = mp_tungsten_carbide->PDF(incident,exitant);
        if (fabs(pdf-pdf2)>(1e-5)*pdf)
          pdf_correct=false;
        }

      TS_ASSERT(pdf_correct);
      }

    void test_MERLMeasured_PDFSum()
      {
      size_t num_samples_sqrt = 1500;
      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      double sum=0;

      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(), num_samples_sqrt, num_samples_sqrt, true);
      for(size_t i=0;i<num_samples_sqrt*num_samples_sqrt;++i)
        {
        Vector3D_d exitant = SamplingRoutines::UniformHemisphereSampling(samples[i]);

        double pdf1 = mp_tungsten_carbide->PDF(incident,exitant);
        sum+=pdf1*2.0*M_PI/(num_samples_sqrt*num_samples_sqrt);

        exitant[2]=-exitant[2];
        double pdf2 = mp_tungsten_carbide->PDF(incident,exitant);
        sum+=pdf2*2.0*M_PI/(num_samples_sqrt*num_samples_sqrt);
        }

      TS_ASSERT_DELTA(sum, 1.0, 0.04);
      }

    void test_MERLMeasured_TotalScattering1()
      {
      size_t num_samples_sqrt=300;
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_sqrt,num_samples_sqrt,true);

      SpectrumCoef_d total=mp_tungsten_carbide->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, SpectrumCoef_d(0.0765,0.0634,0.0747), 0.001); // This is an empirical value.
      }

    void test_MERLMeasured_TotalScattering2()
      {
      size_t num_samples_sqrt=500;
      std::vector<Point2D_d> samples1(num_samples_sqrt*num_samples_sqrt), samples2(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples_sqrt,num_samples_sqrt,true);
      SamplingRoutines::Shuffle(samples2.begin(), samples2.size());

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      SpectrumCoef_d total=mp_tungsten_carbide->TotalScattering(true, sequence1, sequence2);
      CustomAssertDelta(total, SpectrumCoef_d(0.0721, 0.0627, 0.0732), 0.001); // This is an empirical value.
      }

  private:
    intrusive_ptr<MERLMeasuredData> _CreateLambertianMERLData() const
      {
      std::stringstream s;
      const int THETA_H=90,THETA_D=90,PHI_D=180;
      double scales[3] = {1.0/1500, 1.15/1500, 1.66/1500};

      s.write((const char *)&THETA_H, sizeof(int));
      s.write((const char *)&THETA_D, sizeof(int));
      s.write((const char *)&PHI_D, sizeof(int));

      for(unsigned char c=0;c<3;++c)
        {
        const double brdf=INV_PI/scales[c];
        for(size_t i=0;i<THETA_H*THETA_D*PHI_D;++i)
          s.write((const char *)&brdf, sizeof(double));
        }

      return intrusive_ptr<MERLMeasuredData>( new MERLMeasuredData(s) );
      }

    intrusive_ptr<MERLMeasuredData> _LoadMERLDataFromFile(std::string i_filename) const
      {
      std::ifstream file(i_filename.c_str(), std::ios::in | std::ios::binary);
      return intrusive_ptr<MERLMeasuredData>( new MERLMeasuredData(file) );
      }

  private:
    intrusive_ptr<MERLMeasuredData> mp_lambertian_data, mp_tungsten_carbide_data;
    shared_ptr<BxDF> mp_lambertian, mp_tungsten_carbide;
  };

#endif // MERL_MEASURED_TEST_H