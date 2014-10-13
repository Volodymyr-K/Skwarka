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
* Implementation of the VolumeRegion with absorption and scattering being proportional to the density of the media particles.
* The density of the media particles is defined by a regular 3D grid.
* The phase function does not depend on the point coordinates and is defined by the PhaseFunction implementation.
*/
class GridDensityVolumeRegion: public DensityVolumeRegion
  {
  public:
    /**
    * Creates GridDensityVolumeRegion instance with specified base absorption, scattering, bounding box and the 3D array defining the density values.
    */
    GridDensityVolumeRegion(const BBox3D_d &i_bounds, SpectrumCoef_d &i_base_absorption, SpectrumCoef_d &i_base_scattering,
      intrusive_ptr<const PhaseFunction> ip_phase_function, const std::vector<std::vector<std::vector<float>>> &i_densities);

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
    GridDensityVolumeRegion() {}; // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    BBox3D_d m_bounds;
    double m_inv_extent_x, m_inv_extent_y, m_inv_extent_z;

    size_t m_size_x, m_size_y, m_size_z;

    std::vector<std::vector<std::vector<float>>> m_densities;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void GridDensityVolumeRegion::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<DensityVolumeRegion>(*this);
  i_ar & m_bounds;
  i_ar & m_inv_extent_x;
  i_ar & m_inv_extent_y;
  i_ar & m_inv_extent_z;
  i_ar & m_size_x;
  i_ar & m_size_y;
  i_ar & m_size_z;
  i_ar & m_densities;
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT_KEY(GridDensityVolumeRegion)

#endif // GRID_DENSITY_VOLUME_REGION_H