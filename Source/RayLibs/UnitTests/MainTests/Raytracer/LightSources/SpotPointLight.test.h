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

#ifndef SPOT_POINT_LIGHT_TEST_H
#define SPOT_POINT_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LightSources/SpotPointLight.h>
#include <Math/ThreadSafeRandom.h>
#include <Math/SamplingRoutines.h>
#include <vector>

class SpotPointLightTestSuite : public CxxTest::TestSuite
  {
  public:

    void test_SpotPointLight_Power()
      {
      double inner_angle = 1.0, outer_angle = 1.5;
      double inner_cos = cos(inner_angle), outer_cos = cos(outer_angle);
      Spectrum_d I(1.0,2.0,3.0);
      intrusive_ptr<DeltaLightSource> p_light(new SpotPointLight(Point3D_d(0,0,0), Vector3D_d(1,0,0), I, inner_angle, outer_angle));

      Spectrum_d power = p_light->Power();

      size_t N = 10000;
      Spectrum_d sum;
      for(size_t i=0;i<N;++i)
        {
        double theta = (1.0*i/N)*M_PI;
        double dtheta = (1.0/N)*M_PI;

        double cos_t = cos(theta);

        double tmp=0.0;
        if (theta<inner_angle) tmp=1.0;
        else if (theta>outer_angle) tmp=0.0;
        else tmp = pow((cos_t-outer_cos)/(inner_cos-outer_cos), 4.0);

        sum += I * tmp * 2.0*M_PI * dtheta * sin(theta);
        }

      CustomAssertDelta(power, sum, (1e-6));
      }

    void test_SpotPointLight_Lighting()
      {
      Point3D_d light_pos(0,0,0);
      Vector3D_d direction(1,0,0);
      double inner_angle = 1.0, outer_angle = 1.5;
      double inner_cos = cos(inner_angle), outer_cos = cos(outer_angle);
      Spectrum_d intensity(1.0,2.0,3.0);
      intrusive_ptr<DeltaLightSource> p_light(new SpotPointLight(light_pos, direction, intensity, inner_angle, outer_angle));

      // Test the case when the lighted point is outside of the outer cone.
      Ray lighting_ray;
      Spectrum_d lighting = p_light->Lighting(Point3D_d(-2.0,3.0,4.0), lighting_ray);
      TS_ASSERT (lighting.IsBlack());

      // Test the case when the lighted point is inside the inner cone.
      Point3D_d point(3.0,0.0,0.0);
      lighting = p_light->Lighting(point, lighting_ray);
      TS_ASSERT(lighting_ray.m_direction.IsNormalized());

      CustomAssertDelta(lighting_ray(lighting_ray.m_min_t), point, 1e-10);
      CustomAssertDelta(lighting_ray(lighting_ray.m_max_t), light_pos, 1e-10);

      double dist_sqr = Vector3D_d(lighting_ray(lighting_ray.m_max_t)-lighting_ray(lighting_ray.m_min_t)).LengthSqr();
      CustomAssertDelta(lighting, intensity/dist_sqr, 1e-10);

      // Test the case when the lighted point is inside the outer cone but outside of the inner cone.
      double cosine = 0.30552; // Actually, any value will work.
      lighting = p_light->Lighting(Point3D_d(cosine,sqrt(1.0-cosine*cosine),0.0), lighting_ray);

      dist_sqr = Vector3D_d(lighting_ray(lighting_ray.m_max_t)-lighting_ray(lighting_ray.m_min_t)).LengthSqr();
      CustomAssertDelta(lighting, pow((cosine-outer_cos)/(inner_cos-outer_cos),4.0)*intensity/dist_sqr, 1e-10);
      }

    void test_SpotPointLight_SamplePhoton()
      {
      Point3D_d light_pos(0,0,0);
      Vector3D_d direction(1,0,0);
      double inner_angle = 1.0, outer_angle = 1.5;
      double inner_cos = cos(inner_angle), outer_cos = cos(outer_angle);
      Spectrum_d intensity(1.0,2.0,3.0);
      intrusive_ptr<DeltaLightSource> p_light(new SpotPointLight(light_pos, direction, intensity, inner_angle, outer_angle));

      size_t num_samples_sqrt=300, num_samples=num_samples_sqrt*num_samples_sqrt;
      std::vector<Point2D_d> samples(num_samples_sqrt*num_samples_sqrt);
      SamplingRoutines::StratifiedSampling2D(samples.begin(),num_samples_sqrt,num_samples_sqrt,true);

      Spectrum_d sum;
      for(size_t i=0;i<num_samples;++i)
        {
        Ray photon_ray;
        double pdf;
        Spectrum_d sampled_irradiance = p_light->SamplePhoton(samples[i], photon_ray, pdf);

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
      CustomAssertDelta(sum, power, 1e-3);
      }
  };

#endif // SPOT_POINT_LIGHT_TEST_H
