#include "PowerLightsSamplingStrategy.h"
#include <cstring>

PowerLightsSamplingStrategy::PowerLightsSamplingStrategy(const LightSources &i_light_sources): LightsSamplingStrategy()
  {
  size_t infinity_lights_num = i_light_sources.m_infinite_light_sources.size();
  size_t area_lights_num = i_light_sources.m_area_light_sources.size();
  m_lights_num = infinity_lights_num + area_lights_num;

  if (m_lights_num == 0)
    {
    mp_lights_CDF = NULL;
    return;
    }
  else
    mp_lights_CDF = new double[m_lights_num];

  for(size_t i=0;i<infinity_lights_num;++i)
    {
    mp_lights_CDF[i] = i_light_sources.m_infinite_light_sources[i]->Power().Luminance();
    ASSERT(mp_lights_CDF[i] >= 0.0);
    if (i>0)
      mp_lights_CDF[i] += mp_lights_CDF[i-1];
    }

  for(size_t i=0;i<area_lights_num;++i)
    {
    size_t j=infinity_lights_num+i;
    mp_lights_CDF[j] = i_light_sources.m_area_light_sources[i]->Power().Luminance();
    ASSERT(mp_lights_CDF[j] >= 0.0);

    if (j>0)
      mp_lights_CDF[j] += mp_lights_CDF[j-1];
    }

  // Normalize CDF values.
  double total_power = mp_lights_CDF[m_lights_num-1];
  if (total_power > 0.0)
    {
    double inv = 1.0/total_power;
    for(size_t i=0;i<m_lights_num;++i)
      mp_lights_CDF[i] *= inv;
    }
  else
    {
    // If all lights have zero power luminance we sample them with equal probabilities.
    double inv = 1.0/m_lights_num;
    for(size_t i=0;i<m_lights_num;++i)
      mp_lights_CDF[i] = (i+1.0) * inv;
    }
  }

PowerLightsSamplingStrategy::~PowerLightsSamplingStrategy()
  {
  delete[] mp_lights_CDF;
  }

void PowerLightsSamplingStrategy::GetLightsCDF(const Point3D_d &i_point, double *o_lights_CDF) const
  {
  ASSERT(o_lights_CDF);

  memcpy(o_lights_CDF, mp_lights_CDF, m_lights_num*sizeof(double));
  }

void PowerLightsSamplingStrategy::GetLightsCDF(const Point3D_d &i_point, const Vector3D_d &i_normal, double *o_lights_CDF) const
  {
  ASSERT(o_lights_CDF);
  ASSERT(i_normal.IsNormalized());

  memcpy(o_lights_CDF, mp_lights_CDF, m_lights_num*sizeof(double));
  }