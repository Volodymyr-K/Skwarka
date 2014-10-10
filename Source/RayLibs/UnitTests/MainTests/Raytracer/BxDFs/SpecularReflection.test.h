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

#ifndef SPECULAR_REFLECTION_TEST_H
#define SPECULAR_REFLECTION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/BxDFs/SpecularReflection.h>
#include <Raytracer/Core/Fresnel.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class SpecularReflectionTestSuite : public CxxTest::TestSuite
  {
  typedef SpecularReflection<FresnelConductor> SpecularMetal;

  public:
    void test_SpecularReflection_Type()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularMetal(SpectrumCoef_d(1.0),fresnel) );

      TS_ASSERT(bxdf->GetType() == (BSDF_REFLECTION | BSDF_SPECULAR));
      }

    void test_SpecularReflection_Sample()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularMetal(SpectrumCoef_d(1.0),fresnel) );

      Point2D_d sample(RandomDouble(1.0),RandomDouble(1.0));

      double pdf;
      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant;
      SpectrumCoef_d sp = bxdf->Sample(incident, exitant, sample, pdf);

      TS_ASSERT_EQUALS(exitant, Vector3D_d(-incident[0],-incident[1],incident[2]));
      TS_ASSERT_EQUALS(pdf,1.0);
      }

    // Tests that PDF returns 0.0 even for the reflected direction.
    void test_SpecularReflection_PDF()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularMetal(SpectrumCoef_d(1.0),fresnel) );

      Vector3D_d incident=Vector3D_d(0.5,0.5,0.5).Normalized();
      Vector3D_d exitant=Vector3D_d(-incident[0],-incident[1],incident[2]);
      double pdf = bxdf->PDF(incident,exitant);

      TS_ASSERT_EQUALS(pdf,0.0);
      }

    void test_SpecularReflection_TotalScattering1()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularMetal(SpectrumCoef_d(1.0),fresnel) );

      size_t num_samples=10000;
      std::vector<Point2D_d> samples(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples.begin(),num_samples,true);

      SpectrumCoef_d total=bxdf->TotalScattering(Vector3D_d(0.5,0.5,0.5).Normalized(), SamplesSequence2D(&samples[0], (&samples[0]) + samples.size()));
      CustomAssertDelta(total, SpectrumCoef_d(0.848117), (1e-6)); // This is an empirical value.
      }

    void test_SpecularReflection_TotalScattering2()
      {
      FresnelConductor fresnel(SpectrumCoef_d(0.37), SpectrumCoef_d(2.82));
      shared_ptr<BxDF> bxdf = shared_ptr<BxDF>( new SpecularMetal(SpectrumCoef_d(1.0),fresnel) );

      size_t num_samples=50000;
      std::vector<Point2D_d> samples1(num_samples), samples2(num_samples);
      SamplingRoutines::LatinHypercubeSampling2D(samples1.begin(),num_samples,true);
      SamplingRoutines::LatinHypercubeSampling2D(samples2.begin(),num_samples,true);

      SamplesSequence2D sequence1(&samples1[0], (&samples1[0]) + samples1.size());
      SamplesSequence2D sequence2(&samples2[0], (&samples2[0]) + samples2.size());

      SpectrumCoef_d total=bxdf->TotalScattering(true, sequence1, sequence2);
      CustomAssertDelta(total, SpectrumCoef_d(0.852811), (1e-6)); // This is an empirical value.
      }

  };

#endif // SPECULAR_REFLECTION_TEST_H