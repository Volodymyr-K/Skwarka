#include "DiffuseAreaLightSource.h"
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>

DiffuseAreaLightSource::DiffuseAreaLightSource(const Spectrum_d &i_radiance, intrusive_ptr<TriangleMesh> ip_mesh): AreaLightSource(),
m_radiance(i_radiance), mp_mesh(ip_mesh)
  {
  ASSERT(InRange(i_radiance,0.0,DBL_INF));
  ASSERT(ip_mesh);

  size_t number_of_triangles = ip_mesh->GetNumberOfTriangles();
  m_area_CDF.assign(number_of_triangles, 0.0);
  for(size_t i=0;i<number_of_triangles;++i)
    {
    MeshTriangle mesh_triangle = ip_mesh->GetTriangle(i);
    Triangle3D_f triangle(
      ip_mesh->GetVertex(mesh_triangle.m_vertices[0]), 
      ip_mesh->GetVertex(mesh_triangle.m_vertices[1]),
      ip_mesh->GetVertex(mesh_triangle.m_vertices[2]));

    m_area_CDF[i]=triangle.GetArea();
    if (i>0) m_area_CDF[i] += m_area_CDF[i-1];
    }

  if (number_of_triangles>0)
    m_area=m_area_CDF.back();
  else
    m_area=0.0;

  ASSERT(m_area>=0.0);
  if (m_area>0.0)
    {
    double inv_area=1.0/m_area;
    for(size_t i=0;i<number_of_triangles;++i)
      m_area_CDF[i] *= inv_area;
    }
  }

Spectrum_d DiffuseAreaLightSource::Radiance(const DifferentialGeometry &i_dg, size_t i_triangle_index, const Vector3D_d &i_light_direction) const
  {
  ASSERT(i_light_direction.IsNormalized());
  ASSERT(i_dg.m_geometric_normal.IsNormalized());

  if (i_light_direction*i_dg.m_geometric_normal > 0.0)
    return m_radiance;
  else
    return Spectrum_d(0.0);
  }

Spectrum_d DiffuseAreaLightSource::Power() const
  {
  return (M_PI*m_area)*m_radiance;
  }

Spectrum_d DiffuseAreaLightSource::SampleLighting(const Point3D_d &i_point, double i_triangle_sample, const Point2D_d &i_sample, Vector3D_d &o_lighting_vector, double &o_pdf) const
  {
  // Check for an empty mesh.
  if (m_area_CDF.empty())
    {
    o_pdf=0.0;
    o_lighting_vector=Vector3D_d();
    return Spectrum_d(0.0);
    }

  Point3D_d sampled_point;
  Vector3D_d light_normal;
  _SampleArea(i_triangle_sample, i_sample, sampled_point, light_normal);

  o_lighting_vector=Vector3D_d(sampled_point-i_point);

  // Convert PDF from area-based to solid angle-based.
  double length_sqr=Vector3D_d(sampled_point-i_point).LengthSqr();
  o_pdf = length_sqr / (m_area * fabs(o_lighting_vector.Normalized()*light_normal));
  ASSERT(o_pdf>=0.0);

  // Check if the input point is on the lighting side of the triangle.
  if (light_normal*Vector3D_d(i_point-sampled_point) > 0.0)   
    return m_radiance;
  else
    return Spectrum_d(0.0);
  }

double DiffuseAreaLightSource::LightingPDF(const Vector3D_d &i_lighting_vector, size_t i_triangle_index) const
  {
  ASSERT(i_triangle_index < mp_mesh->GetNumberOfTriangles());
  Vector3D_d light_normal = Convert<double>(mp_mesh->GetTriangleNormal(i_triangle_index) );

  double pdf = i_lighting_vector.LengthSqr() / (m_area * fabs(i_lighting_vector.Normalized()*light_normal));
  ASSERT(pdf>=0.0);

  return pdf;
  }

Spectrum_d DiffuseAreaLightSource::SamplePhoton(double i_triangle_sample, const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const
  {
  ASSERT(i_direction_sample[0]>=0.0 && i_direction_sample[0]<1.0 && i_direction_sample[1]>=0.0 && i_direction_sample[1]<1.0);

  // Check for an empty mesh.
  if (m_area_CDF.empty())
    {
    o_pdf=0.0;
    o_photon_ray=Ray();
    return Spectrum_d(0.0);
    }

  Point3D_d sampled_point;
  Vector3D_d light_normal;
  _SampleArea(i_triangle_sample, i_position_sample, sampled_point, light_normal);

  o_photon_ray = Ray(sampled_point, SamplingRoutines::UniformSphereSampling(i_direction_sample));

  // Flip photon ray direction if needed.
  if (o_photon_ray.m_direction * light_normal < 0.0)
    o_photon_ray.m_direction *= -1.0;

  o_pdf = SamplingRoutines::UniformHemispherePDF() / m_area;

  return m_radiance;
  }

void DiffuseAreaLightSource::_SampleArea(double i_triangle_sample, const Point2D_d &i_sample, Point3D_d &o_sampled_point, Vector3D_d &o_geometric_normal) const
  {
  ASSERT(i_triangle_sample>=0.0 && i_triangle_sample<1.0);
  ASSERT(i_sample[0]>=0.0 && i_sample[0]<1.0 && i_sample[1]>=0.0 && i_sample[1]<1.0);
  ASSERT(m_area_CDF.empty() == false);

  // Binary search for the sampled triangle.
  size_t l=0, r=m_area_CDF.size()-1;
  while(l<r)
    {
    size_t m = (l+r)/2;
    if (m_area_CDF[m]<i_triangle_sample)
      l=m+1;
    else
      r=m;
    }

  o_geometric_normal = Convert<double>(mp_mesh->GetTriangleNormal(l) );

  MeshTriangle mesh_triangle = mp_mesh->GetTriangle(l);
  Point3D_d vertices[3] = {
    Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[0]) ), 
    Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[1]) ),
    Convert<double>( mp_mesh->GetVertex(mesh_triangle.m_vertices[2]) )
    };

  double b1,b2;
  SamplingRoutines::UniformTriangleSampling(i_sample,b1,b2);
  ASSERT(b1>=0.0 && b2>=0.0 && b1+b2<=1.0);

  o_sampled_point = b1*vertices[0] + b2*vertices[1] + (1.0-b1-b2)*vertices[2];
  }