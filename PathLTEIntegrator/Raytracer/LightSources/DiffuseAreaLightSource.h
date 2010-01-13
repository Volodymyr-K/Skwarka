#ifndef DIFFUSE_AREA_LIGHT_SOURCE_H
#define DIFFUSE_AREA_LIGHT_SOURCE_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/LightSources.h>

/**
* Represents light source properties of a triangle mesh.
* Each surface point has a constant radiance value for all directions on the hemisphere.
* Each triangle radiate light only on its positive side, i.e. the side where the triangle normal points to.
*/
class DiffuseAreaLightSource: public AreaLightSource
  { 
  public:
    /**
    * Creates DiffuseAreaLightSource instance with the specified radiance value and triangle mesh.
    */
    DiffuseAreaLightSource(const Spectrum_d &i_radiance, intrusive_ptr<const TriangleMesh> ip_mesh);

    /**
    * Returns the light source radiance for the specified point on a mesh and specified outgoing direction.
    * @param i_dg DifferentialGeometry object defining the surface point.
    * @param i_triangle_index Index of the mesh triangle for which the light radiance is to be computed.
    * @param i_light_direction Direction of the light. Should be normalized.
    * @return Light radiance.
    */
    virtual Spectrum_d Radiance(const DifferentialGeometry &i_dg, size_t i_triangle_index, const Vector3D_d &i_light_direction) const;

    /**
    * Returns the total power of the light source, i.e. the light flux.
    */
    virtual Spectrum_d Power() const;

    /**
    * Samples direct lighting at the specified point.
    * The method samples points on the light source surface with respect to the area light surface area, but the returned PDF value is given with respect to the
    * solid angle subtended by the area light at the lighted point.
    * @param i_point Lighted point.
    * @param i_triangle_sample 1D sample value used to select the light source triangle. Should be in [0;1) range.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_lighting_ray Ray to the sampled light source point from the specified lighted point. The direction component of the ray should be normalized.
    * @param[out] o_pdf PDF value with respect to the solid angle. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SampleLighting(const Point3D_d &i_point, double i_triangle_sample, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const;

    /**
    * Returns PDF value for the lighting ray.
    * The PDF value is given with respect to the solid angle subtended by the area light at the lighted point.
    * @param i_lighting_ray Ray to the sampled light source point from the lighted point. The direction component of the ray should be normalized.
    * @param i_triangle_index The index of the light source triangle.
    * @return PDF value. The returned value should be greater or equal than zero.
    */
    virtual double LightingPDF(const Ray &i_lighting_ray, size_t i_triangle_index) const;

    /**
    * Samples outgoing light ray.
    * @param i_triangle_sample 1D sample value used to select the light source triangle. Should be in [0;1) range.
    * @param i_position_sample 2D sample used to sample photon ray origin. Should be in [0;1)^2 range.
    * @param i_direction_sample 2D sample used to sample photon ray direction. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SamplePhoton(double i_triangle_sample, const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const;

  private:
    /**
    * This is a helper method used to sample a point on the light source area uniformly with respect to the surface area.
    * The method also returns geometric normal of the sampled triangle.
    */
    void _SampleArea(double i_triangle_sample, const Point2D_d &i_sample, Point3D_d &o_sampled_point, Vector3D_d &o_geometric_normal) const;

  private:
    Spectrum_d m_radiance;

    intrusive_ptr<const TriangleMesh> mp_mesh;

    /**
    * Total area of the triangle mesh.
    */
    double m_area;

    /**
    * Cumulative density function used to sample surface points on the triangle mesh uniformly with respect to the surface area.
    */
    std::vector<double> m_area_CDF;
  };

#endif // DIFFUSE_AREA_LIGHT_SOURCE_H