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

#ifndef NOISE_ROUTINES_H
#define NOISE_ROUTINES_H

#include "Point3D.h"
#include "Vector3D.h"

namespace NoiseRoutines
  {
  /**
  * Generates Perlin noise at the given coordinates.
  */
  double PerlinNoise(double i_x, double i_y, double i_z);

  /**
  * Generates Perlin noise at the given 3D point.
  */
  double PerlinNoise(const Point3D_d &i_point);

  /**
  * Generates Fractional Brownian noise, which is a sum of multiple Perlin noise values at different frequencies (i.e. octaves).
  * Each next octave has twice the frequency of the previous one.
  * The function also antialises the noise by averaging noise octaves beyond the Nyquist limit (which is determined by the dp_dx and dp_dy arguments).
  * @param i_point 3D point where the noise value is to be computed.
  * @param i_dp_dx Mapped X screen-space differential. 
  * @param i_dp_dy Mapped Y screen-space differential.
  * @param i_omega The fading factor. Amplitude of each next octave is multiplied by this factor. Should be in [0;1] range.
  * @param i_max_octaves Max number of octaves to generate.
  * @return The Fractional Brownian noise value.
  */
  double FBm(const Point3D_d &i_point, const Vector3D_d &i_dp_dx, const Vector3D_d &i_dp_dy, double i_omega, size_t i_max_octaves);

  /**
  * Generates turbulence noise, which is basically the same as FBm except that the absolute value of each octave is taken (instead of a signed one).
  * @param i_point 3D point where the noise value is to be computed.
  * @param i_dp_dx Mapped X screen-space differential.
  * @param i_dp_dy Mapped Y screen-space differential.
  * @param i_omega The fading factor. Amplitude of each next octave is multiplied by this factor. Should be in [0;1] range.
  * @param i_max_octaves Max number of octaves to generate.
  * @return The turbulence noise value.
  */
  double Turbulence(const Point3D_d &i_point, const Vector3D_d &i_dp_dx, const Vector3D_d &i_dp_dy, double i_omega, size_t i_max_octaves);
  };

#endif // NOISE_ROUTINES_H