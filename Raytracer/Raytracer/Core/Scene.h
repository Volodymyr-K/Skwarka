#ifndef SCENE_H
#define SCENE_H

#include <Common/Common.h>
#include <Math/Geometry.h>
#include "Primitive.h"
#include "LightSources.h"
#include "TriangleTree.h"
#include <vector>

// TBD: Add volume regions, update docs and unit tests

/**
* Describes the geometrical, scattering and lighting properties of the scene to be rendered.
* The class encapsulates all primitives, volume regions and lights in the scene.
* It also constructs accelerating structure for primitives and volume regions and provides methods to compute ray intersections.
*/
class Scene: public ReferenceCounted
  {
  public:
    /**
    * Constructs Scene instance with specified primitives and light sources.
    */
    Scene(const std::vector<intrusive_ptr<Primitive> > &i_primitives, const LightSources &i_light_sources);

    /**
    * Returns all primitives in the scene.
    * @warning For the sake of performance this method returns a constant reference to a member field.
    * The calling code should never utilize the reference after the Scene is destroyed.
    */
    const std::vector<intrusive_ptr<Primitive> > &GetPrimitives() const;

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
    std::vector<intrusive_ptr<Primitive> > m_primitives;

    TriangleTree m_tree;

    LightSources m_light_sources;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Scene::Scene(const std::vector<intrusive_ptr<Primitive> > &i_primitives, const LightSources &i_light_sources):
m_primitives(i_primitives), m_light_sources(i_light_sources), m_tree(i_primitives)
  {
  }

inline const std::vector<intrusive_ptr<Primitive> > &Scene::GetPrimitives() const
  {
  return m_primitives;
  }

inline const LightSources &Scene::GetLightSources() const
  {
  return m_light_sources;
  }

inline BBox3D_d Scene::GetWorldBounds() const
  {
  // TBD: union with volume regions
  return m_tree.GetWorldBounds();
  }

inline bool Scene::Intersect(const RayDifferential &i_ray, Intersection &o_intersection, double *o_t) const
  {
  return m_tree.Intersect(i_ray, o_intersection, o_t);
  }

inline bool Scene::IntersectTest(const Ray &i_ray) const
  {
  return m_tree.IntersectTest(i_ray);
  }

#endif // SCENE_H