#ifndef POINT_LIGHT_TEST_H
#define POINT_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/LightSources/PointLight.h>
#include <Math/ThreadSafeRandom.h>
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

      size_t num_samples=1000;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
        Ray photon_ray;
        double pdf;
        Spectrum_d sampled_irradiance = p_light->SamplePhoton(sample, photon_ray, pdf);

        if (sampled_irradiance != intensity)
          {
          TS_FAIL("Wrong radiance value.");
          break;
          }

        if (photon_ray.m_direction.IsNormalized() == false)
          {
          TS_FAIL("The photon ray direction is not normalized.");
          break;
          }

        if (photon_ray(photon_ray.m_min_t) != light_pos)
          {
          TS_FAIL("Wrong photon ray.");
          break;
          }

        if (IsPositiveInf(photon_ray.m_max_t)==false)
          {
          TS_FAIL("Photon ray is bounded.");
          break;
          }

        if (pdf <= 0.0)
          {
          TS_FAIL("PDF value is not positive.");
          break;
          }

        }
      }
  };

#endif // POINT_LIGHT_TEST_H
