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

#ifndef POINT_LIGHT_TEST_H
#define POINT_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class PointLightTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_PointLight_Power()
      {
      intrusive_ptr<DeltaLightSource> p_light(new PointLight(Point3D_d(0,0,0), Spectrum_d(1.0,2.0,3.0)));

      Spectrum_d power = p_light->Power();
      TS_ASSERT_EQUALS(power, 4.0*M_PI*Spectrum_d(1.0,2.0,3.0));
      }

    void test_PointLight_Lighting()
      {
      Point3D_d light_pos(0,0,0);
      Spectrum_d intensity(1.0,2.0,3.0);
      intrusive_ptr<DeltaLightSource> p_light(new PointLight(light_pos, intensity));

      Point3D_d point(2.0,3.0,4.0);
      Ray lighting_ray;
      Spectrum_d lighting = p_light->Lighting(point, lighting_ray);

      TS_ASSERT(lighting_ray.m_direction.IsNormalized());

      CustomAssertDelta(lighting_ray(lighting_ray.m_min_t), point, 1e-10);
      CustomAssertDelta(lighting_ray(lighting_ray.m_max_t), light_pos, 1e-10);

      double dist_sqr = Vector3D_d(lighting_ray(lighting_ray.m_max_t)-lighting_ray(lighting_ray.m_min_t)).LengthSqr();
      CustomAssertDelta(lighting, intensity/dist_sqr, 1e-10);
      }

    void test_PointLight_SamplePhoton()
      {
      Point3D_d light_pos(0,0,0);
      Spectrum_d intensity(1.0,2.0,3.0);
      intrusive_ptr<DeltaLightSource> p_light(new PointLight(light_pos, intensity));

      size_t num_samples_sqrt=100, num_samples=num_samples_sqrt*num_samples_sqrt;
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_sqrt,num_samples_sqrt,true);

      Spectrum_d sum;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
        Ray photon_ray;
        double pdf;
        Spectrum_d sampled_irradiance = p_light->SamplePhoton(sample, photon_ray, pdf);

        if (sampled_irradiance != intensity)
          {
          TS_FAIL("Wrong radiance value.");
          return;
          }

        if (photon_ray.m_direction.IsNormalized() == false)
          {
          TS_FAIL("The photon ray direction is not normalized.");
          return;
          }

        if (photon_ray(photon_ray.m_min_t) != light_pos)
          {
          TS_FAIL("Wrong photon ray.");
          return;
          }

        if (IsPositiveInf(photon_ray.m_max_t)==false)
          {
          TS_FAIL("Photon ray is bounded.");
          return;
          }

        if (pdf <= 0.0)
          {
          TS_FAIL("PDF value is not positive.");
          return;
          }

        // Integrate the irradiance value to check it later.
        sum += sampled_irradiance / pdf;
        }

      // Check that the integrated irradiance is equal to the light's power.
      sum /= (double)num_samples;
      Spectrum_d power = p_light->Power();
      CustomAssertDelta(sum, power, 1e-6);
      }
  };

#endif // POINT_LIGHT_TEST_H
