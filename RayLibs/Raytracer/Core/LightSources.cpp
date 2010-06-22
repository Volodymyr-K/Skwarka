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
