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

#include "NoiseRoutines.h"
#include "MathRoutines.h"

#include <cmath>
#include <algorithm>

namespace NoiseRoutines
  {
  const size_t NOISE_PERM_SIZE = 256;

  // Defines the permutation table for the Perlin Noise function.
  // Contains two copies of the same data to avoid doing costly MOD operation at runtime.
  static int NoisePerm[2 * NOISE_PERM_SIZE] =
    {
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,

    // Now just repeat the same data
    151, 160, 137, 91, 90, 15,
    131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
    88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
    135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
    5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
    223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
    129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
    49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
    138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180
    };

  // Computes dot product of the gradient vector at the lattice point (i_x,i_y,i_z) and the vector (dx,dy,dz)
  // The gradient vector is computed by doing a three nested permutations via the NoisePerm table.
  double Grad(int i_x, int i_y, int i_z, double i_dx, double i_dy, double i_dz) {
    ASSERT(i_x <= NOISE_PERM_SIZE && i_y <= NOISE_PERM_SIZE && i_z <= NOISE_PERM_SIZE);
    int h = NoisePerm[NoisePerm[NoisePerm[i_x]+i_y]+i_z];

    // The four low-order bits determine which of the 16 gradient vectors is to be used.
    h &= 15;

    // All components of all gradient vectors are either -1, 0 or 1
    double u = h<8 || h==12 || h==13 ? i_dx : i_dy;
    double v = h<4 || h==12 || h==13 ? i_dy : i_dz;
    return ((h&1) ? -u : u) + ((h&2) ? -v : v);
    }

  // Smoothing function
  double NoiseWeight(double i_t)
    {
    double t3 = i_t*i_t*i_t;
    double t4 = t3*i_t;
    return 6.0*t4*i_t - 15.0*t4 + 10.0*t3;
    }

  double PerlinNoise(double i_x, double i_y, double i_z)
    {
    // Compute noise cell coordinates and offsets
    int ix = (int)floor(i_x), iy = (int)floor(i_y), iz = (int)floor(i_z);
    double dx = i_x - ix, dy = i_y - iy, dz = i_z - iz;
    ASSERT(dx>=0 && dy>=0 && dz>=0);
    ASSERT(dx<=1.0 && dy<=1.0 && dz<=1.0);

    // Compute gradient weights
    ix &= (NOISE_PERM_SIZE-1);
    iy &= (NOISE_PERM_SIZE-1);
    iz &= (NOISE_PERM_SIZE-1);
    double w000 = Grad(ix, iy, iz, dx, dy, dz);
    double w100 = Grad(ix+1, iy, iz, dx-1, dy, dz);
    double w010 = Grad(ix, iy+1, iz, dx, dy-1, dz);
    double w110 = Grad(ix+1, iy+1, iz, dx-1, dy-1, dz);
    double w001 = Grad(ix, iy, iz+1, dx, dy, dz-1);
    double w101 = Grad(ix+1, iy, iz+1, dx-1, dy, dz-1);
    double w011 = Grad(ix, iy+1, iz+1, dx, dy-1, dz-1);
    double w111 = Grad(ix+1, iy+1, iz+1, dx-1, dy-1, dz-1);

    // First, we smooth the the {dx,dy,dz} vector to ensure continuity of the first and second derivative of the noise
    // as lookup points move between lattice cells.
    double wx = NoiseWeight(dx), wy = NoiseWeight(dy), wz = NoiseWeight(dz);

    // Compute trilinear interpolation of weights
    double x00 = MathRoutines::LinearInterpolate(wx, w000, w100);
    double x10 = MathRoutines::LinearInterpolate(wx, w010, w110);
    double x01 = MathRoutines::LinearInterpolate(wx, w001, w101);
    double x11 = MathRoutines::LinearInterpolate(wx, w011, w111);
    double y0 = MathRoutines::LinearInterpolate(wy, x00, x10);
    double y1 = MathRoutines::LinearInterpolate(wy, x01, x11);
    return MathRoutines::LinearInterpolate(wz, y0, y1);
    }

  double PerlinNoise(const Point3D_d &i_point)
    {
    return PerlinNoise(i_point[0], i_point[1], i_point[2]);
    }

  /**
  * Smoothly interpolates between 0 and 1 using a cubic Hermite spline.
  * Values below i_min are considered zero, values above i_max are considered one.
  */
  double SmoothStep(double i_min, double i_max, double i_value)
    {
    double v = MathRoutines::Clamp((i_value - i_min) / (i_max - i_min), 0.0, 1.0);
    return v * v * (-2.0 * v  + 3.0);
    }

  double FBm(const Point3D_d &i_point, const Vector3D_d &i_dp_dx, const Vector3D_d &i_dp_dy, double i_omega, size_t i_max_octaves)
    {
    ASSERT(i_omega>=0 && i_omega<=1.0);

    // Let's limit the number of octaves by 10 to avoid overflow issues when casting double to int in PerlinNoise() method
    i_max_octaves = std::min(i_max_octaves, (size_t)10);
    // Compute number of octaves for antialiased FBm
    double s2 = std::max(i_dp_dx.LengthSqr(), i_dp_dy.LengthSqr());

    // Antialis the noise by limiting the frequencies beyond the Nyquist limit
    double octaves = s2>DBL_EPS ? std::min((double)i_max_octaves, std::max(1.0, -1.0 - 0.5 * MathRoutines::Log2(s2))) : i_max_octaves;
    int ioctaves = (int)floor(octaves);

    // Compute sum of octaves of noise for FBm
    double sum = 0.0, lambda = 1.0, amplitude = 1.0;
    for (int i = 0; i < ioctaves; ++i)
      {
      sum += amplitude * PerlinNoise(lambda * i_point);
      lambda *= 1.99; // Multiply by 1.99 rather than by 2, in order to reduce the impact of the fact that the noise function is zero at integer lattice points.
      amplitude *= i_omega;
      }

    double partial_octave = octaves - ioctaves;
    sum += amplitude * SmoothStep(0.3, 0.7, partial_octave) * PerlinNoise(lambda * i_point);
    return sum;
    }

  double Turbulence(const Point3D_d &i_point, const Vector3D_d &i_dp_dx, const Vector3D_d &i_dp_dy, double i_omega, size_t i_max_octaves)
    {
    ASSERT(i_omega>=0 && i_omega<=1.0);

    // Let's limit the number of octaves by 10 to avoid overflow issues when casting double to int in PerlinNoise() method
    i_max_octaves = std::min(i_max_octaves, (size_t)10);
    // Compute number of octaves for antialiased FBm
    double s2 = std::max(i_dp_dx.LengthSqr(), i_dp_dy.LengthSqr());

    // Antialis the noise by limiting the frequencies beyond the Nyquist limit
    double octaves = s2>DBL_EPS ? std::min((double)i_max_octaves, std::max(1.0, -1.0 - 0.5 * MathRoutines::Log2(s2))) : i_max_octaves;
    int ioctaves = (int)floor(octaves);

    // Compute sum of octaves of noise for turbulence
    double sum = 0.0, lambda = 1.0, amplitude = 1.0;
    for (int i = 0; i < ioctaves; ++i)
      {
      sum += amplitude * fabs(PerlinNoise(lambda * i_point));
      lambda *= 1.99; // Multiply by 1.99 rather than by 2, in order to reduce the impact of the fact that the noise function is zero at integer lattice points.
      amplitude *= i_omega;
      }

    double partial_octave = octaves - ioctaves;
    sum += amplitude * SmoothStep(0.3, 0.7, partial_octave) * fabs(PerlinNoise(lambda * i_point));

    // finally, add in value to account for average value of fabs(Noise())
    // it's basically the average abs value of Perlin Noise (which is roughly equal to 0.2) multiplied by the sum of geometric progression of i_omega^n terms
    if (octaves<i_max_octaves)
      sum += 0.2 * (amplitude - pow(i_omega, i_max_octaves+1)) / (1.0-i_omega);

    return sum;
    }

  };