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

#ifndef OREN_NAYAR_TEST_H
#define OREN_NAYAR_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/OrenNayar.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class OrenNayarTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_OrenNayar_Type()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(SpectrumCoef_d(1.0),0.1) );

      TS_ASSERT(bxdf->GetType() == (BSDF_REFLECTION | BSDF_DIFFUSE));
      }

    void test_OrenNayar_Reciprocity()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(SpectrumCoef_d(1.0),0.1) );

      Vector3D_d v1=Vector3D_d(0.2,0.5,0.5).Normalized();
      Vector3D_d v2=Vector3D_d(-0.1,0.3,0.1).Normalized();
      SpectrumCoef_d val1 = bxdf->Evaluate(v1, v2);
      SpectrumCoef_d val2 = bxdf->Evaluate(v2, v1);

      TS_ASSERT_EQUALS(val1,val2);
      }

    void test_OrenNayar_Sample()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(SpectrumCoef_d(1.0),0.1) );
      size_t num_samples=1000;

      bool correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        SpectrumCoef_d sp = bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (exitant[2]<0.0 || pdf<0.0)
          correct=false;
        if (sp[0]<0.0 || sp[1]<0.0 || sp[2]<0.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    // When sigma parameter is zero Oren-Nayar should give the same results as Lambertian.
    void test_OrenNayar_ZeroVariance()
      {
      shared_ptr<BxDF> oren_nayar = shared_ptr<BxDF>( new OrenNayar(SpectrumCoef_d(1.0),0.0) );
      shared_ptr<BxDF> lambertian = shared_ptr<BxDF>( new Lambertian(SpectrumCoef_d(1.0)) );
      size_t num_samples=1000;

      bool equal=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        SpectrumCoef_d sp1 = oren_nayar->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);
        SpectrumCoef_d sp2 = lambertian->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (sp1!=sp2)
          equal=false;
        }

      TS_ASSERT(equal);
      }

    void test_OrenNayar_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(SpectrumCoef_d(0.9),0.1) );

      size_t num_samples=100;
      std::vector<Point2D_d> samples(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples,num_samples,true);

      SpectrumCoef_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, SpectrumCoef_d(0.8974), 0.001); // This is an empirical value.
      }

    void test_OrenNayar_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new OrenNayar(SpectrumCoef_d(0.9),0.1) );

      size_t num_samples=400;
      std::vector<Point2D_d> samples1(num_samples*num_samples), samples2(num_samples*num_samples);
      SamplingRoutines::StratifiedSampling2D(samples1.begin(),num_samples,num_samples,true);
      SamplingRoutines::StratifiedSampling2D(samples2.begin(),num_samples,num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      SpectrumCoef_d total=bxdf->TotalScattering(true, sequence1, sequence2);
      CustomAssertDelta(total, SpectrumCoef_d(0.888), 0.002); // This is an empirical value.
      }
  };

#endif // OREN_NAYAR_TEST_H