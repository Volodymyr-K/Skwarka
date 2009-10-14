#ifndef RENDERER_H
#define RENDERER_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Scene.h"
#include "Sample.h"
#include "Camera.h"

/**
* A central abstract class defining the contract for scene renderers.
* Renderers are responsible to render the image seen from a camra onto the camera's film. They also provide method to compute scene radiance and transmittance for a given ray.
*/
class Renderer: public ReferenceCounted
  {
  public:
    /**
    * Returns associated rendered scene.
    */
    intrusive_ptr<Scene> GetScene() const;

    /**
    * Renders the scene for the specified camera.
    * The rendered image will be saved to the camera's film. The film is cleared before rendering, so the previous image will be lost.
    * @param ip_camera Camera in the scene for which the image is to be rendered.
    */
    virtual void Render(intrusive_ptr<Camera> ip_camera) const = 0;

    /**
    * Computes radiance for the specified ray.
    * @param i_ray Ray for which the radiance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @param i_pool Memory pool object that is used for allocating temporary objects.
    * @return Resulting radiance value.
    */
    virtual Spectrum_d Radiance(const RayDifferential &i_ray, const Sample *ip_sample, MemoryPool &i_pool) const = 0;

    /**
    * Computes media transmittance for the specified ray.
    * This method accounts for the media transmittance only and does not account for primitives intersected by the ray.
    * @param i_ray Ray for which the transmittance is to be computed. The direction component of the ray should be normalized.
    * @param ip_sample Sample instance containing requested samples sequences.
    * Can be null, in this case this is up to the implementation how to generate sample values.
    * @return Resulting transmittance value. Each spectrum component will be in [0;1] range.
    */
    virtual Spectrum_d Transmittance(const Ray &i_ray, const Sample *ip_sample) const = 0;

  protected:
    /**
    * Creates Renderer for the specified scene.
    */
    Renderer(intrusive_ptr<Scene> ip_scene);

  private:
    // Not implemented, not a value type.
    Renderer(const Renderer&);
    Renderer &operator=(const Renderer&);

  private:
    intrusive_ptr<Scene> mp_scene;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Renderer::Renderer(intrusive_ptr<Scene> ip_scene):
mp_scene(ip_scene)
  {
  ASSERT(ip_scene);
  }

inline intrusive_ptr<Scene> Renderer::GetScene() const
  {
  return mp_scene;
  }

#endif // RENDERER_H