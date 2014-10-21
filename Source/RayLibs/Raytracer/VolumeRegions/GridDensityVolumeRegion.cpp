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

#include "GridDensityVolumeRegion.h"

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(GridDensityVolumeRegion);

GridDensityVolumeRegion::GridDensityVolumeRegion(const BBox3D_d &i_bounds, SpectrumCoef_d &i_base_absorption, SpectrumCoef_d &i_base_scattering,
                                                 intrusive_ptr<const PhaseFunction> ip_phase_function, const std::vector<std::vector<std::vector<float>>> &i_densities):
DensityVolumeRegion(i_bounds, i_base_absorption, i_base_scattering, ip_phase_function), m_bounds(i_bounds)
  {
  ASSERT(i_densities.empty()==false && i_densities[0].empty()==false && i_densities[0][0].empty() == false);

  m_size_x=i_densities.size();
  m_size_y=i_densities[0].size();
  m_size_z=i_densities[0][0].size();

  // Check that all internal vectors have the same size.
  m_densities.assign(m_size_x*m_size_y*m_size_z, DensityCell());
  size_t ind=0;
  for(size_t i=0;i<i_densities.size();++i)
    {
    ASSERT(i_densities[i].size() == i_densities[0].size());
    for(size_t j=0;j<i_densities[i].size();++j)
      {
      ASSERT(i_densities[i][j].size() == i_densities[0][0].size());
      for (size_t k=0; k<i_densities[i][j].size(); ++k)
        {
        ASSERT(i_densities[i][j][k]>=0.0);
        size_t i1 = i+1<m_size_x ? i+1 : i, j1 = j+1<m_size_y ? j+1 : j, k1 = k+1<m_size_z ? k+1 : k;

        m_densities[ind].m_density[0][0][0]=i_densities[i][j][k];
        m_densities[ind].m_density[0][0][1]=i_densities[i][j][k1];
        m_densities[ind].m_density[0][1][0]=i_densities[i][j1][k];
        m_densities[ind].m_density[0][1][1]=i_densities[i][j1][k1];
        m_densities[ind].m_density[1][0][0]=i_densities[i1][j][k];
        m_densities[ind].m_density[1][0][1]=i_densities[i1][j][k1];
        m_densities[ind].m_density[1][1][0]=i_densities[i1][j1][k];
        m_densities[ind].m_density[1][1][1]=i_densities[i1][j1][k1];
        ++ind;
        }
      }
    }

  ASSERT(m_bounds.Volume() > 0.0);
  if (m_bounds.m_min[0] > m_bounds.m_max[0]) std::swap(m_bounds.m_min[0], m_bounds.m_max[0]);
  if (m_bounds.m_min[1] > m_bounds.m_max[1]) std::swap(m_bounds.m_min[1], m_bounds.m_max[1]);
  if (m_bounds.m_min[2] > m_bounds.m_max[2]) std::swap(m_bounds.m_min[2], m_bounds.m_max[2]);

  m_inv_extent_x = m_size_x/fabs(i_bounds.m_max[0]-m_bounds.m_min[0]);
  m_inv_extent_y = m_size_y/fabs(m_bounds.m_max[1]-m_bounds.m_min[1]);
  m_inv_extent_z = m_size_z/fabs(m_bounds.m_max[2]-m_bounds.m_min[2]);
  }

bool GridDensityVolumeRegion::Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const
  {
  return m_bounds.Intersect(i_ray, op_t_begin, op_t_end);
  }

double GridDensityVolumeRegion::_Density(const Point3D_d &i_point) const
  {
  ASSERT(m_bounds.Inside(i_point));

  double x = (i_point[0]-m_bounds.m_min[0])*m_inv_extent_x - 0.5;
  double y = (i_point[1]-m_bounds.m_min[1])*m_inv_extent_y - 0.5;
  double z = (i_point[2]-m_bounds.m_min[2])*m_inv_extent_z - 0.5;

  // The trick here is that if x,y or z is negative it will clamp it to 0 instead of rounding down to -1.
  int int_x = (int)(x), int_y = (int)(y), int_z = (int)(z);
  ASSERT(int_x>=0 && int_y>=0 && int_z>=0);

  // if x,y or z is negative the dx,dy or dz will be negative too but that's ok because in this case we will interpolate between the same values (see below).
  double dx = x - int_x, dy = y - int_y, dz = z - int_z;

  // Trilinearly interpolate density values to compute local density.
  DensityCell cell = m_densities[int_x*m_size_y*m_size_z + int_y*m_size_z + int_z];
  double d00 = MathRoutines::LinearInterpolate(dx, (double)cell.m_density[0][0][0], (double)cell.m_density[1][0][0]);
  double d10 = MathRoutines::LinearInterpolate(dx, (double)cell.m_density[0][1][0], (double)cell.m_density[1][1][0]);
  double d01 = MathRoutines::LinearInterpolate(dx, (double)cell.m_density[0][0][1], (double)cell.m_density[1][0][1]);
  double d11 = MathRoutines::LinearInterpolate(dx, (double)cell.m_density[0][1][1], (double)cell.m_density[1][1][1]);

  double d0 = MathRoutines::LinearInterpolate(dy, d00, d10);
  double d1 = MathRoutines::LinearInterpolate(dy, d01, d11);
  return MathRoutines::LinearInterpolate(dz, d0, d1);
  }