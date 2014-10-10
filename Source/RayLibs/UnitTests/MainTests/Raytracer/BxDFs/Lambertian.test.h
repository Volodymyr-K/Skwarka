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

#ifndef LAMBERTIAN_TEST_H
#define LAMBERTIAN_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/Lambertian.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class LambertianTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_Lambertian_Type()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(SpectrumCoef_d(1.0)) );

      TS_ASSERT(bxdf->GetType() == (BSDF_REFLECTION | BSDF_DIFFUSE));
      }

    void test_Lambertian_Sample()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(SpectrumCoef_d(1.0)) );
      size_t num_samples=1000;

      bool correct=true;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

        double pdf;
        Vector3D_d exitant;
        SpectrumCoef_d sp = bxdf->Sample(Vector3D_d(0.5,0.5,0.5).Normalized(), exitant, sample, pdf);

        if (sp!=SpectrumCoef_d(INV_PI))
          {
          TS_FAIL("Wrong Lambertian value");
          break;
          }

        if (exitant.IsNormalized() == false)
          {
          TS_FAIL("Sampled direction is not normalized");
          break;
          }
        }
      }

    void test_Lambertian_TotalScattering1()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(SpectrumCoef_d(0.9)) );

      size_t num_samples=10;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      SpectrumCoef_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      TS_ASSERT_EQUALS(total, SpectrumCoef_d(0.9));
      }

    void test_Lambertian_TotalScattering2()
      {
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new Lambertian(SpectrumCoef_d(0.9)) );

      size_t num_samples=10;
      std::vector<Point2D_d> samples1(num_samples), samples2(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples1.begin(),num_samples,true);
      SamplingRoutines::LatinHypercubeSampling2D(samples2.begin(),num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      SpectrumCoef_d total=bxdf->TotalScattering(true, sequence1, sequence2);
      TS_ASSERT_EQUALS(total, SpectrumCoef_d(0.9));
      }
  };

#endif // LAMBERTIAN_TEST_H