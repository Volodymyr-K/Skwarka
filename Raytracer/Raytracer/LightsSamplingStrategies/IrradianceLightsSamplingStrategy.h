#ifndef IRRADIANCE_LIGHTS_SAMPLING_H
#define IRRADIANCE_LIGHTS_SAMPLING_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/LightsSamplingStrategy.h>
#include <Raytracer/Core/LightSources.h>
#include <vector>

/**
* The irradiance-based LightsSamplingStrategy implementation.
* The probability of each light source to be sampled is proportional to the (estimated) irradiance from the light source at the point.
*/
class IrradianceLightsSamplingStrategy: public LightsSamplingStrategy
  {
  public:
    /**
    * Creates IrradianceLightsSamplingStrategy instance for the specified light sources.
    */
    IrradianceLightsSamplingStrategy(const LightSources &i_light_sources);

    /**
    * Returns CDF for the infinity and area lights at the specified 3D point being shaded.
    * Provided output array should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be ordered in the o_lights_CDF array so that the CDF values corresponding to the infinity lights are written before
    * the CDF values corresponding to the area lights. Infinity and area lights are ordered with respect to the order defined by LightSources class.
    * @param i_point 3D point being shaded.
    * @param[out] o_lights_CDF Output array where the CDF values will be written to. Should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be in non-descending order and will all be in [0;1] range.
    */
    virtual void GetLightsCDF(const Point3D_d &i_point, double *o_lights_CDF) const;

    /**
    * Returns CDF for the infinity and area lights at the specified 3D point being shaded and for the specified surface normal.
    * Provided output array should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be ordered in the o_lights_CDF array so that the CDF values corresponding to the infinity lights are written before
    * the CDF values corresponding to the area lights. Infinity and area lights are ordered with respect to the order defined by LightSources class.
    * @param i_point 3D point being shaded.
    * @param i_normal Surface normal at the specified point. Should be normalized.
    * @param[out] o_lights_CDF Output array where the CDF values will be written to. Should have enough space to fit as many values as there are infinity and area lights in the scene.
    * The CDF values will be in non-descending order and will all be in [0;1] range.
    */
    virtual void GetLightsCDF(const Point3D_d &i_point, const Vector3D_d &i_normal, double *o_lights_CDF) const;

  private:
    void _Weights_To_CDF(double *io_lights_CDF) const;

  private:
    const LightSources &m_light_sources;
    size_t m_infinity_lights_num, m_area_lights_num, m_total_lights_num;

    std::vector<double> m_area_lights_intensities;
  };

#endif // IRRADIANCE_LIGHTS_SAMPLING_H