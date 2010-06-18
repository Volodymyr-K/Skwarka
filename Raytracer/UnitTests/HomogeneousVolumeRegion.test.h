#ifndef HOMOGENEOUS_VOLUME_REGION_TEST_H
#define HOMOGENEOUS_VOLUME_REGION_TEST_H

#include <cxxtest/TestSuite.h>
#include "CustomValueTraits.h"
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/VolumeRegions/HomogeneousVolumeRegion.h>
#include <Math/ThreadSafeRandom.h>
#include "Mocks/PhaseFunctionMock.h"

class HomogeneousVolumeRegionTestSuite : public CxxTest::TestSuite
  {
  public:
    HomogeneousVolumeRegionTestSuite()
      {
      m_bounds = BBox3D_d(Point3D_d(1,2,3), Point3D_d(10,20,30));
      m_emission = Spectrum_d(1,1.5,2);
      m_absorption = Spectrum_d(5,7,9);
      m_scattering = Spectrum_d(0.1,0.0,0.9);
      PhaseFunctionMock phase_function;

      mp_volume.reset(new HomogeneousVolumeRegion<PhaseFunctionMock>(m_bounds, m_emission, m_absorption, m_scattering, phase_function));
      }
  
    void test_HomogeneousVolumeRegion_GetBounds()
      {
      TS_ASSERT_EQUALS(mp_volume->GetBounds().m_min, m_bounds.m_min);
      TS_ASSERT_EQUALS(mp_volume->GetBounds().m_max, m_bounds.m_max);
      }

    void test_HomogeneousVolumeRegion_Intersect()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d origin(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Vector3D_d direction = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        double length = RandomDouble(60.0);
        Ray ray(origin, direction, length);

        double t0, t1;
        bool f1 = mp_volume->Intersect(ray, &t0, &t1);

        double u0, u1;
        bool f2 = m_bounds.Intersect(ray, &u0, &u1);

        TS_ASSERT_EQUALS(f1, f2);
        if (f1)
          {
          TS_ASSERT_EQUALS(t0, u0);
          TS_ASSERT_EQUALS(t1, u1);
          }
        }
      }

    void test_HomogeneousVolumeRegion_Emission()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Spectrum_d tmp = mp_volume->Emission(point);
        Spectrum_d correct = m_bounds.Inside(point) ? m_emission : Spectrum_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Absorption()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Spectrum_d tmp = mp_volume->Absorption(point);
        Spectrum_d correct = m_bounds.Inside(point) ? m_absorption : Spectrum_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Scattering()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Spectrum_d tmp = mp_volume->Scattering(point);
        Spectrum_d correct = m_bounds.Inside(point) ? m_scattering : Spectrum_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Attenuation()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Spectrum_d tmp = mp_volume->Attenuation(point);
        Spectrum_d correct = m_bounds.Inside(point) ? m_absorption+m_scattering : Spectrum_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Phase()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Vector3D_d d1 = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        Vector3D_d d2 = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();

        double tmp = mp_volume->Phase(point,d1,d2);
        double correct = m_bounds.Inside(point) ? PhaseFunctionMock()(d1,d2) : 0.0;
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_OpticalThickness()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Vector3D_d direction = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        double length = RandomDouble(60.0);
        Ray ray(point, direction, length);

        Spectrum_d tmp = mp_volume->OpticalThickness(ray, 1.0, 0.0);

        double t0, t1;
        Spectrum_d correct = m_bounds.Intersect(ray, &t0, &t1) ? fabs(t1-t0)*(m_absorption+m_scattering) : Spectrum_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

  private:
    BBox3D_d m_bounds;
    Spectrum_d m_emission, m_absorption, m_scattering;

    intrusive_ptr<VolumeRegion> mp_volume;
  };

#endif // HOMOGENEOUS_VOLUME_REGION_TEST_H
