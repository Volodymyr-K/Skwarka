#include "DiffuseAreaLightSource.h"
#include <Math/Constants.h>
#include <Math/SamplingRoutines.h>
#include <Math/Util.h>

DiffuseAreaLightSource::DiffuseAreaLightSource(const Spectrum_d &i_radiance, intrusive_ptr<const TriangleMesh> ip_mesh): AreaLightSource(ip_mesh),
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

Spectrum_d DiffuseAreaLightSource::SampleLighting(const Point3D_d &i_point, double i_triangle_sample, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const
  {
  // Check for an empty mesh.
  if (m_area_CDF.empty())
    {
    o_pdf=0.0;
    o_lighting_ray=Ray();
    return Spectrum_d(0.0);
    }

  Point3D_d sampled_point;
  Vector3D_d light_normal;
  _SampleArea(i_triangle_sample, i_sample, sampled_point, light_normal);

  Vector3D_d lighting_vector(sampled_point-i_point);
  double length = lighting_vector.Length();
  Vector3D_d lighting_direction = lighting_vector / length;
  o_lighting_ray = Ray(i_point, lighting_direction, 0.0, length);

  // Convert PDF from area-based to a solid angle-based one.
  double angle_cos = fabs(lighting_direction*light_normal);
  o_pdf = angle_cos>DBL_EPS ? (length*length) / (m_area * angle_cos) : 0.0;
  ASSERT(o_pdf>=0.0);

  // Check if the input point is on the lighting side of the triangle.
  if (light_normal*Vector3D_d(i_point-sampled_point) > 0.0)   
    return m_radiance;
  else
    return Spectrum_d(0.0);
  }

double DiffuseAreaLightSource::LightingPDF(const Ray &i_lighting_ray, size_t i_triangle_index) const
  {
  ASSERT(i_lighting_ray.m_direction.IsNormalized());
  ASSERT(i_triangle_index < mp_mesh->GetNumberOfTriangles());
  Vector3D_d light_normal = Convert<double>(mp_mesh->GetTriangleNormal(i_triangle_index) );

  double distance = i_lighting_ray.m_max_t-i_lighting_ray.m_min_t;
  double angle_cos = fabs(i_lighting_ray.m_direction*light_normal);
  double pdf = angle_cos>DBL_EPS ? (distance*distance) / (m_area * angle_cos) : 0.0;
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
  size_t sampled_index = MathRoutines::BinarySearchCDF(m_area_CDF.begin(), m_area_CDF.end(), i_triangle_sample) - m_area_CDF.begin();

  o_geometric_normal = Convert<double>(mp_mesh->GetTriangleNormal(sampled_index) );

  MeshTriangle mesh_triangle = mp_mesh->GetTriangle(sampled_index);
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