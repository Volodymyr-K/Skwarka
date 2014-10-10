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

#ifndef HOMOGENEOUS_VOLUME_REGION_TEST_H
#define HOMOGENEOUS_VOLUME_REGION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
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
      m_absorption = SpectrumCoef_d(5,7,9);
      m_scattering = SpectrumCoef_d(0.1,0.0,0.9);
      intrusive_ptr<PhaseFunction> p_phase_function( new PhaseFunctionMock );

      mp_volume.reset(new HomogeneousVolumeRegion(m_bounds, m_emission, m_absorption, m_scattering, p_phase_function));
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
        SpectrumCoef_d tmp = mp_volume->Absorption(point);
        SpectrumCoef_d correct = m_bounds.Inside(point) ? m_absorption : SpectrumCoef_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Scattering()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        SpectrumCoef_d tmp = mp_volume->Scattering(point);
        SpectrumCoef_d correct = m_bounds.Inside(point) ? m_scattering : SpectrumCoef_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Attenuation()
      {
      size_t N=100;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        SpectrumCoef_d tmp = mp_volume->Attenuation(point);
        SpectrumCoef_d correct = m_bounds.Inside(point) ? m_absorption+m_scattering : SpectrumCoef_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

    void test_HomogeneousVolumeRegion_Phase()
      {
      size_t N=100;
      intrusive_ptr<PhaseFunction> p_phase_function( new PhaseFunctionMock );
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Vector3D_d d1 = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        Vector3D_d d2 = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();

        double tmp = mp_volume->Phase(point,d1,d2);
        double correct = m_bounds.Inside(point) ? p_phase_function->ScatteringPDF(d1,d2) : 0.0;
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

        SpectrumCoef_d tmp = mp_volume->OpticalThickness(ray, 1.0, 0.0);

        double t0, t1;
        SpectrumCoef_d correct = m_bounds.Intersect(ray, &t0, &t1) ? fabs(t1-t0)*(m_absorption+m_scattering) : SpectrumCoef_d(0.0);
        TS_ASSERT_EQUALS(tmp, correct);
        }
      }

  private:
    BBox3D_d m_bounds;
    Spectrum_d m_emission;
    SpectrumCoef_d m_absorption, m_scattering;

    intrusive_ptr<VolumeRegion> mp_volume;
  };

#endif // HOMOGENEOUS_VOLUME_REGION_TEST_H
