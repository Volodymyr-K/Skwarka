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

#include "Transform.h"
#include <cmath>

Transform MakeTranslation(const Vector3D_d &i_translation)
  {
  return Transform(Matrix4x4_d(
    1, 0, 0, i_translation[0],
    0, 1, 0, i_translation[1],
    0, 0, 1, i_translation[2],
    0, 0, 0, 1));
  }

Transform MakeScale(double i_x_scale, double i_y_scale, double i_z_scale)
  {
  return Transform(Matrix4x4_d(
    i_x_scale, 0, 0, 0,
    0, i_y_scale, 0, 0,
    0, 0, i_z_scale, 0,
    0, 0, 0, 1));
  }

Transform MakeScale(const double &i_scale)
  {
  return Transform(Matrix4x4_d(
    i_scale, 0, 0, 0,
    0, i_scale, 0, 0,
    0, 0, i_scale, 0,
    0, 0, 0, 1));
  }

Transform MakeRotationX(const double &i_angle)
  {
  double sin_t = sin(i_angle);
  double cos_t = cos(i_angle);
  return Transform(Matrix4x4_d(
    1,     0,      0, 0,
    0, cos_t, -sin_t, 0,
    0, sin_t,  cos_t, 0,
    0,     0,      0, 1));
  }

Transform MakeRotationY(const double &i_angle)
  {
  double sin_t = sin(i_angle);
  double cos_t = cos(i_angle);
  return Transform(Matrix4x4_d(
    cos_t,   0, sin_t, 0,
    0,   1,     0, 0,
    -sin_t,   0, cos_t, 0,
    0,   0,     0, 1));
  }

Transform MakeRotationZ(const double &i_angle)
  {
  double sin_t = sin(i_angle);
  double cos_t = cos(i_angle);
  return Transform(Matrix4x4_d(
    cos_t, -sin_t, 0, 0,
    sin_t,  cos_t, 0, 0,
    0,      0, 1, 0,
    0,      0, 0, 1));
  }

Transform MakeRotation(const double &i_angle, Vector3D_d i_axis)
  {
  bool can_be_normalized=i_axis.Normalize();
  ASSERT(can_be_normalized);

  double s = sin(i_angle);
  double c = cos(i_angle);
  double m[4][4];

  m[0][0] = i_axis[0] * i_axis[0] + (1.0 - i_axis[0] * i_axis[0]) * c;
  m[0][1] = i_axis[0] * i_axis[1] * (1.0 - c) - i_axis[2] * s;
  m[0][2] = i_axis[0] * i_axis[2] * (1.0 - c) + i_axis[1] * s;
  m[0][3] = 0.0;

  m[1][0] = i_axis[0] * i_axis[1] * (1.0 - c) + i_axis[2] * s;
  m[1][1] = i_axis[1] * i_axis[1] + (1.0 - i_axis[1] * i_axis[1]) * c;
  m[1][2] = i_axis[1] * i_axis[2] * (1.0 - c) - i_axis[0] * s;
  m[1][3] = 0.0;

  m[2][0] = i_axis[0] * i_axis[2] * (1.0 - c) - i_axis[1] * s;
  m[2][1] = i_axis[1] * i_axis[2] * (1.0 - c) + i_axis[0] * s;
  m[2][2] = i_axis[2] * i_axis[2] + (1.0 - i_axis[2] * i_axis[2]) * c;
  m[2][3] = 0.0;

  m[3][0] = 0.0;
  m[3][1] = 0.0;
  m[3][2] = 0.0;
  m[3][3] = 1.0;

  return Transform(Matrix4x4_d(m));
  }

Transform MakeMatchDirections(const Vector3D_d &i_source, const Vector3D_d &i_target)
  {
  ASSERT(i_source.IsNormalized() && i_target.IsNormalized());

  Vector3D_d rotation_axis = i_target^i_source;
  double length = rotation_axis.Length();
  if (length < DBL_EPS)
    {
    if (i_target*i_source>0)
      return Transform(); // Return identity transformation if the axis already match.
    else
      {
      // If axis point to opposite directions we rotate around an arbitrary direction.
      unsigned char smallest_axis_index=0;
      Vector3D_d smallest_axis(1,0,0);
      if (fabs(i_target[1]) < fabs(i_target[smallest_axis_index])) {smallest_axis_index=1;smallest_axis=Vector3D_d(0,1,0);}
      if (fabs(i_target[2]) < fabs(i_target[smallest_axis_index])) {smallest_axis_index=2;smallest_axis=Vector3D_d(0,0,1);}

      return MakeRotation( M_PI, (smallest_axis^i_target).Normalized() );
      }
    }
  else
    {
    rotation_axis /= length;
    return MakeRotation( -acos(i_target*i_source), rotation_axis);
    }
  }

Transform MakeLookAt(const Point3D_d &i_origin, Vector3D_d i_direction, const Vector3D_d &i_up)
  {
  double m[4][4];

  // Initialize fourth column of viewing matrix
  m[0][3] = i_origin[0];
  m[1][3] = i_origin[1];
  m[2][3] = i_origin[2];
  m[3][3] = 1;

  // Initialize first three columns of viewing matrix
  i_direction.Normalize();
  Vector3D_d right = (i_direction^i_up).Normalized();
  Vector3D_d newUp = right^i_direction;
  m[0][0] = right[0];
  m[1][0] = right[1];
  m[2][0] = right[2];
  m[3][0] = 0.0;
  m[0][1] = -newUp[0]; // minus comes from right-handedness so that Y axis actually points "down" in the camera space
  m[1][1] = -newUp[1];
  m[2][1] = -newUp[2];
  m[3][1] = 0.0;
  m[0][2] = i_direction[0];
  m[1][2] = i_direction[1];
  m[2][2] = i_direction[2];
  m[3][2] = 0.0;

  return Transform(Matrix4x4_d(m)).Inverted();
  }