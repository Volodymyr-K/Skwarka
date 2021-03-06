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

#ifndef POWER_LIGHTS_SAMPLING_H
#define POWER_LIGHTS_SAMPLING_H

#include <Common/Common.h>
#include <Raytracer/Core/LightsSamplingStrategy.h>
#include <Raytracer/Core/LightSources.h>

/**
* The power-based LightsSamplingStrategy implementation. The probability of each light to be sampled is proportional to the luminance of its power.
*/
class PowerLightsSamplingStrategy: public LightsSamplingStrategy
  {
  public:
    /**
    * Creates PowerLightsSamplingStrategy instance for the specified light sources.
    */
    PowerLightsSamplingStrategy(const LightSources &i_light_sources);

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

    ~PowerLightsSamplingStrategy();

  private:
    size_t m_lights_num;
    double *mp_lights_CDF;
  };

#endif // POWER_LIGHTS_SAMPLING_H