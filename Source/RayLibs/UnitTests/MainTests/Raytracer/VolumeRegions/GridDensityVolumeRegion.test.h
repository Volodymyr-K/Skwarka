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

#ifndef GRID_DENSITY_VOLUME_REGION_TEST_H
#define GRID_DENSITY_VOLUME_REGION_TEST_H

#include <cxxtest/TestSuite.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/VolumeRegions/GridDensityVolumeRegion.h>
#include <Math/ThreadSafeRandom.h>
#include "Mocks/PhaseFunctionMock.h"

class GridDensityVolumeRegionTestSuite : public CxxTest::TestSuite
  {
  public:

    GridDensityVolumeRegionTestSuite()
      {
      m_bounds = BBox3D_d(Point3D_d(1,2,3), Point3D_d(10,20,30));
      m_absorption = SpectrumCoef_d(0.3,0.2,0.1);
      m_scattering = SpectrumCoef_d(0.2,0.1,0.3);
      intrusive_ptr<PhaseFunction> p_phase_function( new PhaseFunctionMock );

      m_size_x=100;
      m_size_y=110;
      m_size_z=120;
      m_densities.assign(m_size_x, std::vector<std::vector<float>>(m_size_y, std::vector<float>(m_size_z, 0.0)));
      for(size_t i=0;i<m_size_x;++i)
        for(size_t j=0;j<m_size_y;++j)
          for(size_t k=0;k<m_size_z;++k)
            m_densities[i][j][k] = 0.01f*(i+j+k);

      mp_volume.reset(new GridDensityVolumeRegion(m_bounds, m_absorption, m_scattering, p_phase_function, m_densities));
      }

    void test_GridDensityVolumeRegion_GetBounds()
      {
      TS_ASSERT_EQUALS(mp_volume->GetBounds().m_min, m_bounds.m_min);
      TS_ASSERT_EQUALS(mp_volume->GetBounds().m_max, m_bounds.m_max);
      }

    void test_GridDensityVolumeRegion_Intersect()
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

    void test_GridDensityVolumeRegion_Absorption()
      {
      size_t N=1000;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        SpectrumCoef_d tmp = mp_volume->Absorption(point);
        SpectrumCoef_d correct = _GetDensity(point) * m_absorption;
        TS_ASSERT_DELTA(tmp[0], correct[0], correct[0]*0.01);
        TS_ASSERT_DELTA(tmp[1], correct[1], correct[1]*0.01);
        TS_ASSERT_DELTA(tmp[2], correct[2], correct[2]*0.01);
        }
      }

    void test_GridDensityVolumeRegion_Scattering()
      {
      size_t N=1000;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        SpectrumCoef_d tmp = mp_volume->Scattering(point);
        SpectrumCoef_d correct = _GetDensity(point) * m_scattering;
        TS_ASSERT_DELTA(tmp[0], correct[0], correct[0]*0.01);
        TS_ASSERT_DELTA(tmp[1], correct[1], correct[1]*0.01);
        TS_ASSERT_DELTA(tmp[2], correct[2], correct[2]*0.01);
        }
      }

    void test_GridDensityVolumeRegion_Attenuation()
      {
      size_t N=1000;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        SpectrumCoef_d tmp = mp_volume->Attenuation(point);
        SpectrumCoef_d correct = _GetDensity(point) * (m_absorption+m_scattering);
        TS_ASSERT_DELTA(tmp[0], correct[0], correct[0]*0.01);
        TS_ASSERT_DELTA(tmp[1], correct[1], correct[1]*0.01);
        TS_ASSERT_DELTA(tmp[2], correct[2], correct[2]*0.01);
        }
      }

    void test_GridDensityVolumeRegion_Phase()
      {
      size_t N=1000;
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

    void test_GridDensityVolumeRegion_OpticalThickness()
      {
      size_t N=1000;
      for (size_t t=0;t<N;++t)
        {
        Point3D_d point(RandomDouble(60)-30, RandomDouble(60)-30, RandomDouble(60)-30);
        Vector3D_d direction = Vector3D_d(RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0, RandomDouble(2.0)-1.0).Normalized();
        double length = RandomDouble(60.0);
        Ray ray(point, direction, length);

        SpectrumCoef_d tmp = mp_volume->OpticalThickness(ray, 0.1, 0.5);

        double t0, t1;
        SpectrumCoef_d correct;
        if (m_bounds.Intersect(ray, &t0, &t1))
          for(double t=t0;t<t1;t+=0.1)
            {
            double step = std::min(0.1,t1-t);
            correct += step*_GetDensity(ray(t+0.5*step))*(m_absorption+m_scattering);
            }
        TS_ASSERT_DELTA(tmp[0], correct[0], correct[0]*0.01);
        TS_ASSERT_DELTA(tmp[1], correct[1], correct[1]*0.01);
        TS_ASSERT_DELTA(tmp[2], correct[2], correct[2]*0.01);
        }
      }

    // Tests scattering sampling when ray doesn't intersect the volume
    void test_GridDensityVolumeRegion_SampleScattering_NoIntersection()
      {
      size_t N=100;
      for (size_t test=0; test<N; ++test)
        {
        // The ray starts at the boundary but points in an opposite direction
        Point3D_d point(1, 2, 3);
        Vector3D_d direction = Vector3D_d(-1, -1, -1).Normalized();
        Ray ray(point, direction);

        double sample = RandomDouble(1.0), offset = RandomDouble(1.0), pdf, t;
        SpectrumCoef_d transmittance;
        bool scattered = mp_volume->SampleScattering(ray, sample, 0.1, offset, t, pdf, transmittance);
        TS_ASSERT(!scattered);
        CustomAssertDelta(transmittance, SpectrumCoef_d(1.0), DBL_EPS);
        TS_ASSERT_DELTA(pdf, 1.0, DBL_EPS);
        }
      }

    // Tests scattering sampling when ray intersects the volume
    void test_GridDensityVolumeRegion_SampleScattering_Intersection()
      {
      size_t N=100;
      for (size_t test=0; test<N; ++test)
        {
        Point3D_d point(1, 8, 8);
        Vector3D_d direction = Vector3D_d(1, 0, 0).Normalized();
        Ray ray(point, direction);

        double sample = RandomDouble(1.0), offset = RandomDouble(1.0), pdf, t;
        SpectrumCoef_d transmittance, optical_thickness;
        bool scattered = mp_volume->SampleScattering(ray, sample, 0.01, offset, t, pdf, transmittance);
        if (scattered)
          {
          TS_ASSERT(t>=0 && t<9.0); // 9.0 is the dimension of the volume region in X dimension
          optical_thickness = mp_volume->OpticalThickness(Ray(ray.m_origin, ray.m_direction, 0.0, t), 0.01, offset);
          }
        else
          optical_thickness = mp_volume->OpticalThickness(ray, 0.01, offset);

        SpectrumCoef_d transmittance2 = Exp(-1.0*optical_thickness);
        CustomAssertDelta(transmittance, transmittance2, (1e-4));
        }
      }

    // Tests that the PDF sums up to 1.0
    void test_GridDensityVolumeRegion_SampleScattering_PDF()
      {
      Point3D_d point(1, 8, 8);
      Vector3D_d direction = Vector3D_d(1, 0, 0).Normalized();
      Ray ray(point, direction);

      double offset = RandomDouble(1.0), pdf, t, prev_t=0;
      double scatter_probability=0.0, non_scatter_probability=0.0;
      for (double sample=1.0; sample>=0.0; sample-=0.001)
        {
        SpectrumCoef_d transmittance;
        bool scattered = mp_volume->SampleScattering(ray, sample, 0.01, offset, t, pdf, transmittance);
        if (scattered)
          scatter_probability += pdf * (t-prev_t); // The integration domain is the distance along the ray, so we multiply pdf by the delta t.
        else
          non_scatter_probability = pdf;

        prev_t=t;
        }

      double total_probability = scatter_probability+non_scatter_probability;
      TS_ASSERT_DELTA(total_probability, 1.0, 0.01);
      }

  private:
    double _GetDensity(const Point3D_d &i_point) const
      {
      if (m_bounds.Inside(i_point)==false)
        return 0.0;

      Point3D_d point = i_point - m_bounds.m_min;
      double x=m_size_x*point[0]/(m_bounds.m_max[0]-m_bounds.m_min[0])-0.5;
      double y=m_size_y*point[1]/(m_bounds.m_max[1]-m_bounds.m_min[1])-0.5;
      double z=m_size_z*point[2]/(m_bounds.m_max[2]-m_bounds.m_min[2])-0.5;
      return 0.01 * (std::max(0.0,x)+std::max(0.0,y)+std::max(0.0,z));
      }

  private:
    BBox3D_d m_bounds;
    SpectrumCoef_d m_absorption, m_scattering;

    size_t m_size_x, m_size_y, m_size_z;
    std::vector<std::vector<std::vector<float>>> m_densities;
    intrusive_ptr<VolumeRegion> mp_volume;
  };

#endif // GRID_DENSITY_VOLUME_REGION_TEST_H
