#ifndef GRID_DENSITY_VOLUME_REGION_H
#define GRID_DENSITY_VOLUME_REGION_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include <Math/MathRoutines.h>
#include <Raytracer/Core/Spectrum.h>
#include <Raytracer/Core/VolumeRegion.h>
#include <Raytracer/Core/PhaseFunction.h>
#include <vector>

/**
* Implementation of the VolumeRegion with emission, absorption and scattering being proportional to the density of the media particles.
* The density of the media particles is defined by a regular 3D grid.
* The phase function does not depend on the point coordinates and is defined by the PhaseFunction implementation.
*/
class GridDensityVolumeRegion: public DensityVolumeRegion
  {
  public:
    /**
    * Creates GridDensityVolumeRegion instance with specified base emission, absorption, scattering, bounding box and the 3D array defining the density values.
    */
    GridDensityVolumeRegion(const BBox3D_d &i_bounds, Spectrum_d &i_base_emission, Spectrum_d &i_base_absorption, Spectrum_d &i_base_scattering,
      intrusive_ptr<const PhaseFunction> ip_phase_function, const std::vector<std::vector<std::vector<double> > > &i_densities);

    /**
    * Returns true if the ray intersects volume region and computes ray parametric coordinates of the intersection region.
    * @param i_ray Input ray. Direction component should be normalized.
    * @param op_t_begin Parametric coordinate of the begin of the intersection region. Can be NULL.
    * @param op_t_end Parametric coordinate of the end of the intersection region. Can be NULL.
    * @return true if the ray intersects the volume region.
    */
    bool Intersect(Ray i_ray, double *op_t_begin, double *op_t_end) const;

  private:
    /**
    * Private virtual function for that returns density of the media particles at the specified point.
    * The density value is computed by interpolating the values of the densities 3D grid.
    */
    double _Density(const Point3D_d &i_point) const;

  private:
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes GridDensityVolumeRegion to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    BBox3D_d m_bounds;
    double m_inv_extent_x, m_inv_extent_y, m_inv_extent_z;

    size_t m_size_x, m_size_y, m_size_z;

    std::vector<std::vector<std::vector<double> > > m_densities;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct GridDensityVolumeRegion to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const GridDensityVolumeRegion *ip_volume, const unsigned int i_version)
  {
  BBox3D_d bounds = ip_volume->GetBounds();
  Spectrum_d base_emission = ip_volume->GetBaseEmission();
  Spectrum_d base_absorption = ip_volume->GetBaseAbsorption();
  Spectrum_d base_scattering = ip_volume->GetBaseScattering();
  intrusive_ptr<const PhaseFunction> p_phase_function = ip_volume->GetPhaseFunction();

  i_ar << bounds;
  i_ar << base_emission;
  i_ar << base_absorption;
  i_ar << base_scattering;
  i_ar << p_phase_function;
  }

/**
* Constructs GridDensityVolumeRegion with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, GridDensityVolumeRegion *ip_volume, const unsigned int i_version)
  {
  BBox3D_d bounds;
  Spectrum_d base_emission, base_absorption, base_scattering;
  intrusive_ptr<const PhaseFunction> p_phase_function;

  i_ar >> bounds;
  i_ar >> base_emission;
  i_ar >> base_absorption;
  i_ar >> base_scattering;
  i_ar >> p_phase_function;

  // Initialize GridDensityVolumeRegion with some dummy densities, they will be serialized later in serialize() method.
  std::vector<std::vector<std::vector<double> > > densities(1, std::vector<std::vector<double> >(1, std::vector<double>(1)));
  ::new(ip_volume)GridDensityVolumeRegion(bounds, base_emission, base_absorption, base_scattering, p_phase_function, densities);
  }

template<class Archive>
void GridDensityVolumeRegion::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<DensityVolumeRegion>(*this);
  i_ar & m_size_x;
  i_ar & m_size_y;
  i_ar & m_size_z;
  i_ar & m_densities;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(GridDensityVolumeRegion)

#endif // GRID_DENSITY_VOLUME_REGION_H