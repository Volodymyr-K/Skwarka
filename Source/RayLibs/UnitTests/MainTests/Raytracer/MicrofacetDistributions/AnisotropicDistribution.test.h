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

#ifndef ANISOTROPIC_DISTRIBUTION_TEST_H
#define ANISOTROPIC_DISTRIBUTION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/Constants.h>
#include <Raytracer/MicrofacetDistributions/BlinnDistribution.h>
#include <Raytracer/MicrofacetDistributions/AnisotropicDistribution.h>
#include <Math/ThreadSafeRandom.h>

class AnisotropicDistributionTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_AnisotropicDistribution_EvaluateZeroExponent()
      {
      AnisotropicDistribution aniso(0.0,0.0);
      double value=aniso.Evaluate(Vector3D_d(0.5,-0.5,0.5).Normalized());
      TS_ASSERT_EQUALS(value,INV_PI);
      }

    void test_AnisotropicDistribution_Sample()
      {
      AnisotropicDistribution aniso(10.0,1.0);

      bool correct=true;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,-0.5,-0.5).Normalized();
        Vector3D_d exitant;
        aniso.Sample(incident, exitant, sample, pdf);
        if (exitant.IsNormalized()==false || pdf<0.0)
          correct=false;
        }

      TS_ASSERT(correct);
      }

    // Tests that for high exponents sampled directions are always in the same hemisphere.
    void test_AnisotropicDistribution_SampleHighExponent()
      {
      AnisotropicDistribution aniso(500.0, 600.0);

      bool correct_hemisphere=true;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,-0.5,-0.5).Normalized();
        Vector3D_d exitant;
        aniso.Sample(incident, exitant, sample, pdf);

        if (exitant[2]>0.0)
          correct_hemisphere=false;
        }

      TS_ASSERT(correct_hemisphere);
      }

    // Tests that PDF() method returns the same PDF value as the Sample() method does.
    void test_AnisotropicDistribution_PDF()
      {
      AnisotropicDistribution aniso(10.0,1.0);

      bool correct_pdf=true;
      for(size_t i=0;i<1000;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));

        double pdf;
        Vector3D_d incident=Vector3D_d(0.5,-0.5,-0.5).Normalized();
        Vector3D_d exitant;
        aniso.Sample(incident, exitant, sample, pdf);

        double pdf2=aniso.PDF(incident,exitant);
        if (fabs(pdf-pdf2)>(1e-5)*pdf)
          correct_pdf=false;
        }

      TS_ASSERT(correct_pdf);
      }

    // Tests that for equal u and v exponents the Evaluate() method returns the same values that the Blinn distribution does.
    void test_AnisotropicDistribution_Isotropic()
      {
      AnisotropicDistribution aniso(10.0,10.0);
      BlinnDistribution blinn(10.0);

      bool correct=true;
      for(size_t i=0;i<1000;++i)
        {
        Vector3D_d incident=Vector3D_d(RandomDouble(1.0),RandomDouble(1.0),RandomDouble(1.0)).Normalized();
        double t1 = aniso.Evaluate(incident);
        double t2 = blinn.Evaluate(incident);

        if (fabs(t1-t2) > (1e-10))
          correct=false;
        }

      TS_ASSERT(correct);
      }
  };

#endif // ANISOTROPIC_DISTRIBUTION_TEST_H