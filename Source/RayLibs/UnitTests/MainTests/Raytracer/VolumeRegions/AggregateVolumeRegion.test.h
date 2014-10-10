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

#ifndef AGGREGATE_VOLUME_REGION_TEST_H
#define AGGREGATE_VOLUME_REGION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/VolumeRegions/AggregateVolumeRegion.h>
#include <Math/ThreadSafeRandom.h>
#include "Mocks/VolumeRegionMock.h"

class AggregateVolumeRegionTestSuite : public CxxTest::TestSuite
  {
  public:
    AggregateVolumeRegionTestSuite()
      {
      m_bounds1 = BBox3D_d(Point3D_d(0,0,0), Point3D_d(2,2,2));
      m_emission1 = Spectrum_d(1,1.5,2);
      m_absorption1 = SpectrumCoef_d(5,7,9);
      m_scattering1 = SpectrumCoef_d(0.1,0.0,0.9);

      m_bounds2 = BBox3D_d(Point3D_d(1,1,1), Point3D_d(3,3,3));
      m_emission2 = Spectrum_d(2,0.5,1);
      m_absorption2 = SpectrumCoef_d(2,1,4);
      m_scattering2 = SpectrumCoef_d(5.1,0.3,3.9);

      mp_region1.reset(new VolumeRegionMock(m_bounds1, m_emission1, m_absorption1, m_scattering1));
      mp_region2.reset(new VolumeRegionMock(m_bounds2, m_emission2, m_absorption2, m_scattering2));

      std::vector<intrusive_ptr<const VolumeRegion>> regions;
      regions.push_back(mp_region1);
      regions.push_back(mp_region2);

      mp_aggregate.reset(new AggregateVolumeRegion(regions));
      }

    void test_AggregateVolumeRegion_GetBounds()
      {
      TS_ASSERT_EQUALS(mp_aggregate->GetBounds().m_min, m_bounds1.m_min);
      TS_ASSERT_EQUALS(mp_aggregate->GetBounds().m_max, m_bounds2.m_max);
      }

    void test_AggregateVolumeRegion_Intersect()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d origin(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        Vector3D_d direction = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        double length = RandomDouble(10.0);
        Ray ray(origin, direction, length);

        double t0, t1, u0, u1;
        bool f1 = mp_region1->Intersect(ray, &t0, &t1);
        bool f2 = mp_region2->Intersect(ray, &u0, &u1);

        double k0, k1;
        bool f3 = mp_aggregate->Intersect(ray, &k0, &k1);

        TS_ASSERT_EQUALS(f1 || f2, f3);
        if (f1 || f2)
          {
          double mn = DBL_MAX, mx = -DBL_MAX;
          if (f1) {mn = std::min(mn,t0);mx = std::max(mx,t1);}
          if (f2) {mn = std::min(mn,u0);mx = std::max(mx,u1);}

          TS_ASSERT_EQUALS(mn, k0);
          TS_ASSERT_EQUALS(mx, k1);
          }
        }
      }

    void test_AggregateVolumeRegion_Emission()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        Spectrum_d tmp = mp_aggregate->Emission(point);
        Spectrum_d correct = (m_bounds1.Inside(point) ? m_emission1 : Spectrum_d(0.0)) + (m_bounds2.Inside(point) ? m_emission2 : Spectrum_d(0.0));
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_AggregateVolumeRegion_Absorption()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        SpectrumCoef_d tmp = mp_aggregate->Absorption(point);
        SpectrumCoef_d correct = (m_bounds1.Inside(point) ? m_absorption1 : SpectrumCoef_d(0.0)) + (m_bounds2.Inside(point) ? m_absorption2 : SpectrumCoef_d(0.0));
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_AggregateVolumeRegion_Scattering()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        SpectrumCoef_d tmp = mp_aggregate->Scattering(point);
        SpectrumCoef_d correct = (m_bounds1.Inside(point) ? m_scattering1 : SpectrumCoef_d(0.0)) + (m_bounds2.Inside(point) ? m_scattering2 : SpectrumCoef_d(0.0));
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_AggregateVolumeRegion_Attenuation()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        SpectrumCoef_d tmp = mp_aggregate->Attenuation(point);
        SpectrumCoef_d correct = (m_bounds1.Inside(point) ? m_absorption1+m_scattering1 : SpectrumCoef_d(0.0)) + (m_bounds2.Inside(point) ? m_absorption2+m_scattering2 : SpectrumCoef_d(0.0));
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_AggregateVolumeRegion_Phase()
      {
      size_t N=1000;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        Vector3D_d d1 = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        Vector3D_d d2 = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();

        double tmp = mp_aggregate->Phase(point,d1,d2);
        double correct = (m_bounds1.Inside(point) || m_bounds2.Inside(point)) ? 1.0/(4.0*M_PI) : 0.0;
        TS_ASSERT_DELTA(tmp, correct, (1e-10));
        }
      }

    void test_AggregateVolumeRegion_OpticalThickness()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(10)-5, RandomDouble(10)-5, RandomDouble(10)-5);
        Vector3D_d direction = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        double length = RandomDouble(10.0);
        Ray ray(point, direction, length);

        SpectrumCoef_d tmp = mp_aggregate->OpticalThickness(ray, 1.0, 0.0);

        double t0, t1, u0, u1;
        SpectrumCoef_d correct = (m_bounds1.Intersect(ray, &t0, &t1) ? fabs(t1-t0)*(m_absorption1+m_scattering1) : SpectrumCoef_d(0.0)) +
          (m_bounds2.Intersect(ray, &u0, &u1) ? fabs(u1-u0)*(m_absorption2+m_scattering2) : SpectrumCoef_d(0.0));
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }


  private:
    BBox3D_d m_bounds1;
    Spectrum_d m_emission1;
    SpectrumCoef_d m_absorption1, m_scattering1;

    BBox3D_d m_bounds2;
    Spectrum_d m_emission2;
    SpectrumCoef_d m_absorption2, m_scattering2;

    intrusive_ptr<VolumeRegion> mp_aggregate, mp_region1, mp_region2;
  };

#endif // AGGREGATE_VOLUME_REGION_TEST_H
