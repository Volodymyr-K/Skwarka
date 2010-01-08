#ifndef PARALLEL_LIGHT_TEST_H
#define PARALLEL_LIGHT_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/Numerics.h>
#include <Raytracer/LightSources/ParallelLight.h>
#include <Math/ThreadSafeRandom.h>
#include <vector>

class ParallelLightTestSuite : public CxxTest::TestSuite
  {
  public:
    void test_ParallelLight_Power()
      {
      BBox3D_d bbox(Point3D_d(0,0,0), Point3D_d(10,20,30));
      intrusive_ptr<DeltaLightSource> p_light(new ParallelLight(Vector3D_d(1,1,1).Normalized(), Spectrum_d(1.0,2.0,3.0), bbox));

      Spectrum_d power = p_light->Power();
      double projected_area = (10*20+10*30+20*30)*sqrt(1.0/3.0);;
      CustomAssertDelta(power, projected_area*Spectrum_d(1.0,2.0,3.0), 1e-6);
      }

    void test_ParallelLight_Lighting()
      {
      Vector3D_d direction = Vector3D_d(1,1,1).Normalized();
      Spectrum_d radiance(1.0,2.0,3.0);

      BBox3D_d bbox(Point3D_d(0,0,0), Point3D_d(10,20,30));
      intrusive_ptr<DeltaLightSource> p_light(new ParallelLight(direction, radiance, bbox));

      Point3D_d point(2.0,3.0,4.0);
      Ray lighting_ray;
      Spectrum_d lighting = p_light->Lighting(point, lighting_ray);

      TS_ASSERT(lighting_ray.m_direction.IsNormalized());
      TS_ASSERT(IsInf(lighting_ray.m_max_t));

      CustomAssertDelta(lighting_ray(lighting_ray.m_min_t), point, 1e-10);

      TS_ASSERT_EQUALS(lighting, radiance);
      }

    void test_ParallelLight_SamplePhoton()
      {
      Vector3D_d direction = Vector3D_d(1,1.2,1.3).Normalized();
      Spectrum_d radiance(1.0,2.0,3.0);

      BBox3D_d bbox(Point3D_d(0,0,0), Point3D_d(10,20,30));
      intrusive_ptr<DeltaLightSource> p_light(new ParallelLight(direction, radiance, bbox));

      size_t num_samples=10000;
      for(size_t i=0;i<num_samples;++i)
        {
        Point2D_d sample(RandomDouble(1.0), RandomDouble(1.0));
        Ray photon_ray;
        double pdf;
        Spectrum_d radiance = p_light->SamplePhoton(sample, photon_ray, pdf);

        if (radiance != radiance)
          {
          TS_FAIL("Wrong radiance value.");
          break;
          }

        if (photon_ray.m_direction.IsNormalized() == false)
          {
          TS_FAIL("The photon ray direction is not normalized.");
          break;
          }

        if (bbox.Inside( photon_ray(photon_ray.m_min_t) ))
          {
          TS_FAIL("The photon ray origin is in the bounding box.");
          break;
          }

        if (bbox.Intersect(photon_ray) == false)
          {
          TS_FAIL("The photon ray does not intersect the bounding box.");
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

#endif // PARALLEL_LIGHT_TEST_H
