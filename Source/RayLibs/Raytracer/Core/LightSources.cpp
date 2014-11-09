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

#include "LightSources.h"

////////////////////////////////////////// DeltaLightSource ///////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

DeltaLightSource::DeltaLightSource()
  {
  }

DeltaLightSource::~DeltaLightSource()
  {
  }

//////////////////////////////////////// InfiniteLightSource //////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

InfiniteLightSource::InfiniteLightSource()
  {
  }

InfiniteLightSource::~InfiniteLightSource()
  {
  }

Spectrum_d InfiniteLightSource::SampleLighting(const Vector3D_d &i_normal, const Point2D_d &i_sample, Vector3D_d &o_lighting_direction, double &o_pdf) const
  {
  return SampleLighting(i_sample, o_lighting_direction, o_pdf);
  }

double InfiniteLightSource::LightingPDF(const Vector3D_d &i_normal, const Vector3D_d &i_lighting_direction) const
  {
  return LightingPDF(i_lighting_direction);
  }

////////////////////////////////////////// AreaLightSource ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

AreaLightSource::AreaLightSource(intrusive_ptr<const TriangleMesh> ip_mesh): mp_mesh(ip_mesh)
  {
  ASSERT(ip_mesh);
  }

AreaLightSource::~AreaLightSource()
  {
  }

intrusive_ptr<const TriangleMesh> AreaLightSource::GetTriangleMesh() const
  {
  return mp_mesh;
  }

const TriangleMesh *AreaLightSource::GetTriangleMesh_RawPtr() const
  {
  return mp_mesh.get();
  }
