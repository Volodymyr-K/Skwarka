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

#ifndef SCENE_H
#define SCENE_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Primitive.h"
#include "LightSources.h"
#include "TriangleAccelerator.h"
#include "VolumeRegion.h"
#include <vector>

/**
* Describes the geometrical, scattering and lighting properties of the scene to be rendered.
* The class encapsulates all primitives, volume region and lights in the scene.
* It also constructs accelerating structure for primitives and volume regions and provides methods to compute ray intersections.
*/
class Scene: public ReferenceCounted
  {
  public:
    /**
    * Constructs Scene instance with specified primitives, volume region and lights. Volume region can be NULL.
    */
    Scene(const std::vector<intrusive_ptr<const Primitive>> &i_primitives, intrusive_ptr<const VolumeRegion> ip_volume_region, const LightSources &i_light_sources);

    /**
    * Returns all primitives in the scene.
    * @warning For the sake of performance this method returns a constant reference to a member field.
    * The calling code should never utilize the reference after the Scene is destroyed.
    */
    const std::vector<intrusive_ptr<const Primitive>> &GetPrimitives() const;

    /**
    * Returns a pointer to the VolumeRegion of the scene.
    */
    intrusive_ptr<const VolumeRegion> GetVolumeRegion() const;

    /**
    * Returns a pointer to the VolumeRegion of the scene.
    * @warning The calling code should never utilize the pointer after the Scene is destroyed.
    */
    const VolumeRegion *GetVolumeRegion_RawPtr() const;

    /**
    * Returns all light sources in the scene.
    * @warning For the sake of performance this method returns a constant reference to a member field.
    * The calling code should never utilize the reference after the Scene is destroyed.
    */
    const LightSources &GetLightSources() const;

    /**
    * Returns world bounds of all primitives and volume regions in the scene.
    */
    BBox3D_d GetWorldBounds() const;

    /**
    * Computes the intersection of the specified ray with the nearest primitive in the scene.
    * @param i_ray Intersecting ray.
    * @param[out] o_intersection Resulting intersection object.
    * @param[out] o_t If not null, it will be set to the ray t parameter corresponding to the intersection point.
    * If the ray does not intersect any primitive in the scene it will be set to infinity.
    * @return true if the specified ray intersects any primitive in the scene and false otherwise.
    */
    bool Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t=NULL) const;

    /**
    * Returns true if the specified ray intersects any primitive in the scene.
    * Unlike the Intersect() method this method does not look for the nearest intersection and therefore is usually faster.
    * @param i_ray Intersecting ray.
    * @return true if the specified ray intersects any primitive in the scene and false otherwise.
    */
    bool IntersectTest(const Ray &i_ray) const;

  private:
    // Not implemented, not a value type.
    Scene(const Scene&);
    Scene &operator=(const Scene&);

  private:
    std::vector<intrusive_ptr<const Primitive>> m_primitives;
    
    intrusive_ptr<const VolumeRegion> mp_volume_region;

    BBox3D_d m_bounds;

    TriangleAccelerator m_triangle_accelerator;

    LightSources m_light_sources;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Scene::Scene(const std::vector<intrusive_ptr<const Primitive>> &i_primitives, intrusive_ptr<const VolumeRegion> ip_volume_region, const LightSources &i_light_sources):
m_primitives(i_primitives), mp_volume_region(ip_volume_region), m_light_sources(i_light_sources), m_triangle_accelerator(i_primitives)
  {
  m_bounds = m_triangle_accelerator.GetWorldBounds();
  if (ip_volume_region)
    m_bounds.Unite(ip_volume_region->GetBounds());
  }

inline const std::vector<intrusive_ptr<const Primitive>> &Scene::GetPrimitives() const
  {
  return m_primitives;
  }

inline intrusive_ptr<const VolumeRegion> Scene::GetVolumeRegion() const
  {
  return mp_volume_region;
  }

inline const VolumeRegion *Scene::GetVolumeRegion_RawPtr() const
  {
  return mp_volume_region.get();
  }

inline const LightSources &Scene::GetLightSources() const
  {
  return m_light_sources;
  }

inline BBox3D_d Scene::GetWorldBounds() const
  {
  return m_bounds;
  }

inline bool Scene::Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t) const
  {
  return m_triangle_accelerator.Intersect(i_ray, o_intersection, o_t);
  }

inline bool Scene::IntersectTest(const Ray &i_ray) const
  {
  return m_triangle_accelerator.IntersectTest(i_ray);
  }

/**
* Saves the data which is needed to construct Scene to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const Scene *ip_scene, const unsigned int i_version)
  {
  std::vector<intrusive_ptr<const Primitive>> primitives = ip_scene->GetPrimitives();
  intrusive_ptr<const VolumeRegion> p_volume_region = ip_scene->GetVolumeRegion();
  LightSources light_sources = ip_scene->GetLightSources();

  i_ar << primitives;
  i_ar << p_volume_region;
  i_ar << light_sources;
  }

/**
* Constructs Scene with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, Scene *ip_scene, const unsigned int i_version)
  {
  std::vector<intrusive_ptr<const Primitive>> primitives;
  intrusive_ptr<const VolumeRegion> p_volume_region;
  LightSources light_sources;

  i_ar >> primitives;
  i_ar >> p_volume_region;
  i_ar >> light_sources;

  ::new(ip_scene)Scene(primitives, p_volume_region, light_sources);
  }

/**
* Serializes Scene to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, Scene &i_scene, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_scene);
  }

#endif // SCENE_H