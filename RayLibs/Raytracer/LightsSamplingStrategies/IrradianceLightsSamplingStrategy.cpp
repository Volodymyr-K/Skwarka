#include "IrradianceLightsSamplingStrategy.h"
#include <Raytracer/Core/SpectrumRoutines.h>
#include <Math/MathRoutines.h>

IrradianceLightsSamplingStrategy::IrradianceLightsSamplingStrategy(const LightSources &i_light_sources): LightsSamplingStrategy(),
m_light_sources(i_light_sources)
  {
  m_infinity_lights_num = i_light_sources.m_infinite_light_sources.size();
  m_area_lights_num = i_light_sources.m_area_light_sources.size();
  m_total_lights_num = m_infinity_lights_num+m_area_lights_num;

  for(size_t i=0;i<m_area_lights_num;++i)
    {
    BBox3D_d bbox = Convert<double>(m_light_sources.m_area_light_sources[i]->GetTriangleMesh_RawPtr()->GetBounds());
    double power = SpectrumRoutines::Luminance(m_light_sources.m_area_light_sources[i]->Power());
    m_area_lights_intensities.push_back( power/(M_PI*bbox.Area()) );
    }
  }

void IrradianceLightsSamplingStrategy::GetLightsCDF(const Point3D_d &i_point, double *o_lights_CDF) const
  {
  ASSERT(o_lights_CDF);

  if (m_total_lights_num==0)
    return;

  for(size_t i=0;i<m_infinity_lights_num;++i)
    o_lights_CDF[i] = SpectrumRoutines::Luminance(m_light_sources.m_infinite_light_sources[i]->Fluence());

  for(size_t i=0;i<m_area_lights_num;++i)
    {
    BBox3D_d bbox = Convert<double>(m_light_sources.m_area_light_sources[i]->GetTriangleMesh_RawPtr()->GetBounds());
    double intensity = m_area_lights_intensities[i];

    double solid_angle = MathRoutines::SubtendedSolidAngle(i_point, bbox);
    o_lights_CDF[i+m_infinity_lights_num] = solid_angle*intensity;
    }

  _Weights_To_CDF(o_lights_CDF);
  }

void IrradianceLightsSamplingStrategy::GetLightsCDF(const Point3D_d &i_point, const Vector3D_d &i_normal, double *o_lights_CDF) const
  {
  ASSERT(o_lights_CDF);
  ASSERT(i_normal.IsNormalized());

  if (m_total_lights_num==0)
    return;

  for(size_t i=0;i<m_infinity_lights_num;++i)
    o_lights_CDF[i] =SpectrumRoutines::Luminance( m_light_sources.m_infinite_light_sources[i]->Irradiance(i_normal));

  for(size_t i=0;i<m_area_lights_num;++i)
    {
    BBox3D_d bbox = Convert<double>(m_light_sources.m_area_light_sources[i]->GetTriangleMesh_RawPtr()->GetBounds());
    double intensity = m_area_lights_intensities[i];

    if (bbox.Inside(i_point))
      o_lights_CDF[i+m_infinity_lights_num] = M_PI*intensity;
    else
      {
      // Estimate the cosine value by averaging the cosine values at the vertices.
      // This does not give the correct value but is much faster (and easier) than trying to integrate it over the box surface.
      double average_cosine = 0.0;
      for(unsigned char j=0;j<8;++j)
        {
        Point3D_d vertex(j&1 ? bbox.m_min[0] : bbox.m_max[0], j&2 ? bbox.m_min[1] : bbox.m_max[1], j&4 ? bbox.m_min[2] : bbox.m_max[2]);
        Vector3D_d dir=Vector3D_d(vertex-i_point).Normalized();
        average_cosine += std::max(0.0, dir*i_normal);
        }
      average_cosine *= 0.125; // Divide by 8 to average the cosine value.

      if (average_cosine > 0.0)
        {
        /*
        Due to the ad-hoc way we used to estimate cosine value it may differ from the correct one. Too low cosine values can lead to a big variance in certain cases.
        To avoid that we clamp it by 0.1 and thus allow only one order of magnitude of difference between max and min cosine values.
        This can cause over-estimation of lights near the horizon but it is much better than underestimating other lights.
        */
        if (average_cosine < 0.1) average_cosine = 0.1;

        double solid_angle = MathRoutines::SubtendedSolidAngle(i_point, bbox);
        o_lights_CDF[i+m_infinity_lights_num] = solid_angle*intensity*average_cosine;
        }
      else
        o_lights_CDF[i+m_infinity_lights_num] = 0.0;
      }
    }

  _Weights_To_CDF(o_lights_CDF);
  }

void IrradianceLightsSamplingStrategy::_Weights_To_CDF(double *io_lights_CDF) const
  {
  ASSERT(m_total_lights_num>0);

  for(size_t i=1;i<m_total_lights_num;++i)
    {
    ASSERT(io_lights_CDF[i] >= 0.0);
    io_lights_CDF[i]+=io_lights_CDF[i-1];
    }

  if (io_lights_CDF[m_total_lights_num-1]>0.0)
    {
    double inv = 1.0/io_lights_CDF[m_total_lights_num-1];
    for(size_t i=0;i<m_total_lights_num;++i)
      io_lights_CDF[i] *= inv;
    }
  else
    {
    // If all lights have zero power luminance we sample them with equal probabilities.
    double inv = 1.0/m_total_lights_num;
    for(size_t i=0;i<m_total_lights_num;++i)
      io_lights_CDF[i] = (i+1.0) * inv;
    }
  }