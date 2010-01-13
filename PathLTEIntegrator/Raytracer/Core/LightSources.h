#ifndef LIGHT_SOURCES_H
#define LIGHT_SOURCES_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "TriangleMesh.h"
#include "Spectrum.h"

/**
* An abstract base class defining the contract for all delta light sources.
* A delta light source is a singular light source, so that every point in the scene is illuminated by the source only along a single direction.
* @sa InfiniteLightSource, AreaLightSource
*/
class DeltaLightSource: public ReferenceCounted
  { 
  public:
    /**
    * Returns the total power of the light source, i.e. the light flux.
    * The method can return approximation of the real flux rather than the exact value.
    */
    virtual Spectrum_d Power() const = 0;

    /**
    * Returns the light source radiance at the specified point.
    * @param i_point Lighted point.
    * @param[out] o_lighting_ray Ray to the light source from the specified point.
    * Can point to an infinity (e.g. in case of a parallel light sources). The direction component of the ray should be normalized.
    * @return Radiance value.
    */
    virtual Spectrum_d Lighting(const Point3D_d &i_point, Ray &o_lighting_ray) const = 0;

    /**
    * Samples outgoing light ray.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray. The direction component of the ray should be normalized.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SamplePhoton(const Point2D_d &i_sample, Ray &o_photon_ray, double &o_pdf) const = 0;

    virtual ~DeltaLightSource();

  protected:
    DeltaLightSource();

  private:
    // Not implemented, not a value type.
    DeltaLightSource(const DeltaLightSource&);
    DeltaLightSource &operator=(const DeltaLightSource&);
  };

/**
* An abstract base class defining the contract for all infinite light sources.
* An infinite light source is a light source describing a light coming from the outside of the scene from different directions.
* An example of infinite light source is the light coming from the sky from the entire hemisphere.
* @sa DeltaLightSource, AreaLightSource
*/
class InfiniteLightSource: public ReferenceCounted
  { 
  public:
    /**
    * Returns the light source radiance for the specified camera ray.
    * @param i_ray The ray pointing to the light source.
    * @return Light radiance.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray) const = 0;

    /**
    * Returns the total power of the light source, i.e. the light flux.
    * The method can return approximation of the real flux rather than the exact value.
    */
    virtual Spectrum_d Power() const = 0;

    /**
    * Samples direct lighting at the specified point.
    * @param i_point Lighted point.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_lighting_ray Ray to the light source from the specified point. The direction component of the ray should be normalized. The ray is unbounded.
    * @param[out] o_pdf PDF value for the sampled direction. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SampleLighting(const Point3D_d &i_point, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const = 0;

    /**
    * Returns PDF value for the specified point and lighting direction.
    * Implementation of this method should be consistent with implementation of SampleLighting() method so that they both use the same sampling distribution.
    * @param i_point Lighted point.
    * @param i_lighting_direction Direction to the light source from the specified point. The vector is normalized.
    * @return PDF value for the sampled direction. The returned value should be greater or equal than zero.
    */
    virtual double LightingPDF(const Point3D_d &i_point, const Vector3D_d &i_lighting_direction) const = 0;

    /**
    * Samples direct lighting at the specified point with the specified surface normal.
    * Default implementation just calls SampleLighting() method without the normal information.
    * @param i_point Lighted point.
    * @param i_normal Surface normal vector at the specified point. Should be normalized.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_lighting_ray Ray to the light source from the specified point. The direction component of the ray should be normalized. The ray is unbounded.
    * @param[out] o_pdf PDF value for the sampled direction. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SampleLighting(const Point3D_d &i_point, const Vector3D_d &i_normal, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const;

    /**
    * Returns PDF value for the specified point, surface normal vector and lighting direction.
    * Implementation of this method should be consistent with implementation of SampleLighting() method so that they both use the same sampling distribution.
    * Default implementation just calls LightingPDF() method without the normal information.
    * @param i_point Lighted point.
    * @param i_normal Surface normal vector at the specified point. Should be normalized.
    * @param i_lighting_direction Direction to the light source from the specified point. The vector is normalized.
    * @return PDF value for the sampled direction. The returned value should be greater or equal than zero.
    */
    virtual double LightingPDF(const Point3D_d &i_point, const Vector3D_d &i_normal, const Vector3D_d &i_lighting_direction) const;

    /**
    * Samples outgoing light ray.
    * @param i_position_sample 2D sample used to sample photon ray origin. Should be in [0;1)^2 range.
    * @param i_direction_sample 2D sample used to sample photon ray direction. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray. The direction component of the ray should be normalized.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SamplePhoton(const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const = 0;

    /**
    * Returns irradiance value at a surface point with the specified normal assuming there's no objects in the scene blocking the light.
    * Only light comping from the positive hemisphere (with respect to the specified normal) is considered.
    * The method can return an approximation rather than the exact value.
    * @param i_normal Surface normal. Should be normalized.
    * @return Irradiance value.
    */
    virtual Spectrum_d Irradiance(const Vector3D_d &i_normal) const = 0;

    /**
    * Returns irradiance value at a point assuming there's no objects in the scene blocking the light.
    * The method does not take the surface normal so it accounts for the light coming from the entire sphere.
    * Strictly speaking, the returned value is not an irradiance because it is not multiplied by the cosine factor.
    * The method can return an approximation rather than the exact value.
    * @return Irradiance value.
    */
    virtual Spectrum_d Irradiance() const = 0;

    virtual ~InfiniteLightSource();

  protected:
    InfiniteLightSource();

  private:
    // Not implemented, not a value type.
    InfiniteLightSource(const InfiniteLightSource&);
    InfiniteLightSource &operator=(const InfiniteLightSource&);
  };

/**
* An abstract class for light emitting properties of triangle mesh.
*/
class AreaLightSource: public ReferenceCounted
  { 
  public:
    /**
    * Returns a pointer to the TriangleMesh the area light is associated with.
    */
    intrusive_ptr<const TriangleMesh> GetTriangleMesh() const;

    /**
    * Returns a raw pointer to the TriangleMesh the area light is associated with.
    * @warning The calling code should never utilize the pointer after the AreaLightSource is destroyed.
    */
    const TriangleMesh *GetTriangleMesh_RawPtr() const;

    /**
    * Returns the light source radiance for the specified point on a mesh and specified outgoing direction.
    * @param i_dg DifferentialGeometry object defining the surface point.
    * @param i_triangle_index Index of the mesh triangle for which the light radiance is to be computed.
    * @param i_light_direction Direction of the light. Should be normalized.
    * @return Light radiance.
    */
    virtual Spectrum_d Radiance(const DifferentialGeometry &i_dg, size_t i_triangle_index, const Vector3D_d &i_light_direction) const = 0;

    /**
    * Returns the total power of the light source, i.e. the light flux.
    */
    virtual Spectrum_d Power() const = 0;

    /**
    * Samples direct lighting at the specified point.
    * The method samples points on the light source surface.
    * The returned PDF value however is given with respect to the solid angle subtended by the area light at the lighted point.
    * @param i_point Lighted point.
    * @param i_triangle_sample 1D sample value used to select the light source triangle. Should be in [0;1) range.
    * @param i_sample 2D sample. Should be in [0;1)^2 range.
    * @param[out] o_lighting_ray Ray to the sampled light source point from the specified lighted point. The direction component of the ray should be normalized.
    * @param[out] o_pdf PDF value with respect to the solid angle. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SampleLighting(const Point3D_d &i_point, double i_triangle_sample, const Point2D_d &i_sample, Ray &o_lighting_ray, double &o_pdf) const = 0;

    /**
    * Returns PDF value for the lighting ray.
    * The PDF value is given with respect to the solid angle subtended by the area light at the lighted point.
    * @param i_lighting_ray Ray to the sampled light source point from the lighted point. The direction component of the ray should be normalized.
    * @param i_triangle_index The index of the light source triangle.
    * @return PDF value. The returned value should be greater or equal than zero.
    */
    virtual double LightingPDF(const Ray &i_lighting_ray, size_t i_triangle_index) const = 0;

    /**
    * Samples outgoing light ray.
    * @param i_triangle_sample 1D sample value used to select the light source triangle. Should be in [0;1) range.
    * @param i_position_sample 2D sample used to sample photon ray origin. Should be in [0;1)^2 range.
    * @param i_direction_sample 2D sample used to sample photon ray direction. Should be in [0;1)^2 range.
    * @param[out] o_photon_ray Sampled ray. The direction component of the ray should be normalized.
    * @param[out] o_pdf PDF value for the sampled light ray. The returned value should be greater or equal than zero.
    * @return Radiance value.
    */
    virtual Spectrum_d SamplePhoton(double i_triangle_sample, const Point2D_d &i_position_sample, const Point2D_d &i_direction_sample, Ray &o_photon_ray, double &o_pdf) const = 0;

    virtual ~AreaLightSource();

  protected:
    /**
    * Creates AreaLightSource instance with the specified triangle mesh.
    */
    AreaLightSource(intrusive_ptr<const TriangleMesh> ip_mesh);

  private:
    // Not implemented, not a value type.
    AreaLightSource(const AreaLightSource&);
    AreaLightSource &operator=(const AreaLightSource&);

  private:
    intrusive_ptr<const TriangleMesh> mp_mesh;
  };

/**
* A convenient structure encapsulating all light sources in the scene.
*/
struct LightSources
  {
  std::vector<intrusive_ptr<const DeltaLightSource> > m_delta_light_sources;

  std::vector<intrusive_ptr<const InfiniteLightSource> > m_infinitiy_light_sources;

  std::vector<intrusive_ptr<const AreaLightSource> > m_area_light_sources;
  };

#endif // LIGHT_SOURCES_H