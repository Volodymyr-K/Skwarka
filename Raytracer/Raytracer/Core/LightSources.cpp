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

Spectrum_d InfiniteLightSource::SampleLighting(const Point3D_d &i_point, const Vector3D_d &i_normal, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const
  {
  return SampleLighting(i_point, i_sample, o_lighting_ray, o_pdf);
  }

double InfiniteLightSource::LightingPDF(const Point3D_d &i_point, const Vector3D_d &i_normal, const Vector3D_d &i_lighting_direction) const
  {
  return LightingPDF(i_point, i_lighting_direction);
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
