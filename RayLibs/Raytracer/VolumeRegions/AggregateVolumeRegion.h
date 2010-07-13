#ifndef VOLUME_AGGREGATE_H
#define VOLUME_AGGREGATE_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/VolumeRegion.h>
#include <vector>

/**
* Implementation of the VolumeRegion that acts as a container for other volume regions.
* The class is used by Scene to represent all volume regions as a single object to make calls to.
* Current implementation stores all underlying volume regions in a vector and has linear complexity (by the number of regions) for most of the methods.
*/
class AggregateVolumeRegion: public VolumeRegion
  {
  public:
    /**
    * Creates AggregateVolumeRegion instance with specified volume regions.
    */
    AggregateVolumeRegion(const std::vector<intrusive_ptr<const VolumeRegion> > &i_volume_regions);

    /**
    * Returns bounding box for all volume regions.
    */
    BBox3D_d GetBounds() const;

    std::vector<intrusive_ptr<const VolumeRegion> > GetVolumeRegions() const;

    /**
    * Returns true if the ray intersects at least one volume region and computes ray parametric coordinates of the intersection region.
    * @param i_ray Input ray. Direction component should be normalized.
    * @param op_t_begin Parametric coordinate of the begin of the intersection region. Can be NULL.
    * @param op_t_end Parametric coordinate of the end of the intersection region. Can be NULL.
    * @return true if the ray intersects the volume region.
    */
    bool Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const;

    /**
    * Returns aggregated emission density of the volume region at the specified point.
    * The aggregated value is a sum of corresponding values of the underlying volume regions.
    */
    Spectrum_d Emission(const Point3D_d &i_point) const;

    /**
    * Returns aggregated absorption density of the volume region at the specified point.
    * The aggregated value is a sum of corresponding values of the underlying volume regions.
    */
    Spectrum_d Absorption(const Point3D_d &i_point) const;

    /**
    * Returns aggregated scattering density of the volume region at the specified point.
    * The aggregated value is a sum of corresponding values of the underlying volume regions.
    */
    Spectrum_d Scattering(const Point3D_d &i_point) const;

    /**
    * Returns aggregated attenuation density (which is the sum absorption and scattering) of the volume region at the specified point.
    * The aggregated value is a sum of corresponding values of the underlying volume regions.
    */
    Spectrum_d Attenuation(const Point3D_d &i_point) const;

    /**
    * Returns aggregated value of the phase function at the specified point for the specified incoming and outgoing directions.
    * The phase function for a fixed incoming direction defines probability distribution over the sphere for the outgoing direction
    * and thus should sum up to 1.0 when integrated over the sphere.
    * The aggregated value is a weighted sum (with weights equal to the normalized scattering luminances) of the phase functions of the underlying volume regions.
    * @param i_point Point in the volume region.
    * @param i_incoming Incoming direction, i.e. the direction of the light ray before the scattering. Should be normalized.
    * @param i_outgoing Outgoing direction, i.e. the direction of the light ray after the scattering. Should be normalized.
    * @return PDF value. Should be greater or equal than 0.0;
    */
    double Phase(const Point3D_d &i_point, const Vector3D_d &i_incoming, const Vector3D_d &i_outgoing) const;

    /**
    * Returns aggregated optical thickness of the volume region for the specified ray.
    * The method also takes two additional parameters for MonteCarlo integration.
    * @param i_ray Ray for which the optical thickness is to be computed. Ray direction should be normalized.
    * @param i_step Step size for the MonteCarlo integration. Should be greater than 0.0. Not used by this implementation.
    * @param i_offset_sample The sample value used for MonteCarlo integration to choose position in the segments for evaluating attenuation value.
    * Should be in [0;1) range. Not used by this implementation.
    * @return Optical thickness.
    */
    Spectrum_d OpticalThickness(const Ray &i_ray, double i_step, double i_offset_sample) const;

  private:
    std::vector<intrusive_ptr<const VolumeRegion> > m_volume_regions;

    BBox3D_d m_bounds;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct AggregateVolumeRegion to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const AggregateVolumeRegion *ip_volume, const unsigned int i_version)
  {
  std::vector<intrusive_ptr<const VolumeRegion> > volume_regions = ip_volume->GetVolumeRegions();

  i_ar << volume_regions;
  }

/**
* Constructs AggregateVolumeRegion with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, AggregateVolumeRegion *ip_volume, const unsigned int i_version)
  {
  std::vector<intrusive_ptr<const VolumeRegion> > volume_regions;

  i_ar >> volume_regions;

  ::new(ip_volume)AggregateVolumeRegion(volume_regions);
  }

/**
* Serializes AggregateVolumeRegion to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, AggregateVolumeRegion &i_volume, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<VolumeRegion>(i_volume);
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(AggregateVolumeRegion)

#endif // VOLUME_REGION_H