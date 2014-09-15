#include "GridDensityVolumeRegion.h"

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_IMPLEMENT(GridDensityVolumeRegion);

GridDensityVolumeRegion::GridDensityVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_base_emission, SpectrumCoef_d &i_base_absorption, SpectrumCoef_d &i_base_scattering,
                                                 intrusive_ptr<const PhaseFunction> ip_phase_function, const std::vector<std::vector<std::vector<float> > > &i_densities):
DensityVolumeRegion(i_bounds, i_base_emission, i_base_absorption, i_base_scattering, ip_phase_function), m_bounds(i_bounds), m_densities(i_densities)
  {
  ASSERT(i_densities.empty()==false && i_densities[0].empty()==false && i_densities[0][0].empty() == false);

  m_size_x=i_densities.size();
  m_size_y=i_densities[0].size();
  m_size_z=i_densities[0][0].size();

  // Check that all internal vectors have the same size.
  for(size_t i=0;i<i_densities.size();++i)
    {
    ASSERT(i_densities[i].size() == i_densities[0].size());
    for(size_t j=0;j<i_densities[i].size();++j)
      {
      ASSERT(i_densities[i][j].size() == i_densities[0][0].size());
      for(size_t k=0;k<i_densities[i][j].size();++k)
        ASSERT(i_densities[i][j][k]>=0.0);
      }
    }

  ASSERT(m_bounds.Volume() > 0.0);
  if (m_bounds.m_min[0] > m_bounds.m_max[0]) std::swap(m_bounds.m_min[0], m_bounds.m_max[0]);
  if (m_bounds.m_min[1] > m_bounds.m_max[1]) std::swap(m_bounds.m_min[1], m_bounds.m_max[1]);
  if (m_bounds.m_min[2] > m_bounds.m_max[2]) std::swap(m_bounds.m_min[2], m_bounds.m_max[2]);

  m_inv_extent_x = 1.0/fabs(i_bounds.m_max[0]-m_bounds.m_min[0]);
  m_inv_extent_y = 1.0/fabs(m_bounds.m_max[1]-m_bounds.m_min[1]);
  m_inv_extent_z = 1.0/fabs(m_bounds.m_max[2]-m_bounds.m_min[2]);
  }

bool GridDensityVolumeRegion::Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const
  {
  return m_bounds.Intersect(i_ray, op_t_begin, op_t_end);
  }

double GridDensityVolumeRegion::_Density(const Point3D_d &i_point) const
  {
  if (m_bounds.Inside(i_point)==false)
    return 0.0;

  double x = m_size_x*(i_point[0]-m_bounds.m_min[0])*m_inv_extent_x - 0.5;
  double y = m_size_y*(i_point[1]-m_bounds.m_min[1])*m_inv_extent_y - 0.5;
  double z = m_size_z*(i_point[2]-m_bounds.m_min[2])*m_inv_extent_z - 0.5;

  // The trick here is that if x,y or z is negative it will clamp it to 0 instead of rounding down to -1.
  int int_x = (int)(x), int_y = (int)(y), int_z = (int)(z);

  int int_x1 = std::min((int)(x+1), (int)m_size_x-1), int_y1 = std::min((int)(y+1), (int)m_size_y-1), int_z1 = std::min((int)(z+1), (int)m_size_z-1);
  ASSERT(int_x>=0 && int_y>=0 && int_z>=0);

  // if x,y or z is negative the dx,dy or dz will be negative too but that's ok because in this case we will interpolate between the same values (see below).
  double dx = x - int_x, dy = y - int_y, dz = z - int_z;

  // Trilinearly interpolate density values to compute local density.
  double d00 = MathRoutines::LinearInterpolate(dx, (double)m_densities[int_x][int_y][int_z], (double)m_densities[int_x1][int_y][int_z]);
  double d10 = MathRoutines::LinearInterpolate(dx, (double)m_densities[int_x][int_y1][int_z], (double)m_densities[int_x1][int_y1][int_z]);
  double d01 = MathRoutines::LinearInterpolate(dx, (double)m_densities[int_x][int_y][int_z1], (double)m_densities[int_x1][int_y][int_z1]);
  double d11 = MathRoutines::LinearInterpolate(dx, (double)m_densities[int_x][int_y1][int_z1], (double)m_densities[int_x1][int_y1][int_z1]);

  double d0 = MathRoutines::LinearInterpolate(dy, d00, d10);
  double d1 = MathRoutines::LinearInterpolate(dy, d01, d11);
  return MathRoutines::LinearInterpolate(dz, d0, d1);
  }