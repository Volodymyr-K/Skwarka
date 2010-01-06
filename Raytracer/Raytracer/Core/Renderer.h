#ifndef RENDERER_H
#define RENDERER_H

#include <Common/Common.h>
#include <Common/MemoryPool.h>
#include <Math/Geometry.h>
#include "Spectrum.h"
#include "Scene.h"
#include "Sample.h"
#include "Camera.h"
#include "tbb/tick_count.h"

class DisplayUpdateCallback;

/**
* A central abstract class defining the contract for scene renderers.
* Renderers are responsible to render the image seen from a camera onto the camera's film. They also provide method to compute scene radiance and transmittance for a given ray.
*/
class Renderer: public ReferenceCounted
  {
  public:
    /**
    * Returns associated rendered scene.
    */
    intrusive_ptr<const Scene> GetScene() const;

    /**
    * Renders the scene for the specified camera.
    * The rendered image will be saved to the camera's film. The film is cleared before rendering, so the previous image will be lost.
    * @param ip_camera Camera in the scene for which the image is to be rendered.
    */
    virtual void Render(intrusive_ptr<const Camera> ip_camera) const = 0;

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

    /**
    * Sets callback that will be called periodically to update display with the currently rendered image.
    * The callback will be called by Render() method periodically with the specified period (in seconds).
    * @param ip_display_update_callback Pointer to the callback. If NULL, the callback won't be called.
    * @param i_update_period Time period in seconds. Should be greater than zero.
    */
    void SetDisplayUpdateCallback(DisplayUpdateCallback *ip_display_update_callback, double i_update_period = 1.0);

  protected:
    /**
    * Creates Renderer for the specified scene.
    */
    Renderer(intrusive_ptr<const Scene> ip_scene);

    /**
    * A helper method for the derived classes that calls DisplayUpdateCallback if the specified time period has already passed since the last call or if i_force_update is true.
    * Implementations needs to call this method periodically to ensure that the display is updated frequently enough.
    * @param ip_film Film with the rendered image. Should not be NULL.
    * @param i_force_update If true the callback will be called regardless of whether the time period has passed.
    */
    void _UpdateDisplay(intrusive_ptr<const Film> ip_film, bool i_force_update = false) const;

  private:
    // Not implemented, not a value type.
    Renderer(const Renderer&);
    Renderer &operator=(const Renderer&);

  private:
    intrusive_ptr<const Scene> mp_scene;

    DisplayUpdateCallback *mp_display_update_callback;
    double m_update_period;

    // Time when the DisplayUpdateCallback was called last time (or the time of class construction if it was never called).
    mutable tbb::tick_count m_last_display_update;
  };

/**
* This is a callback interface for updating display with the currently rendered image.
* The callbacks are called by Renderer::Render() method periodically to keep the display updated with the currently rendered image.
*/
class DisplayUpdateCallback
  {
  public:
    /**
    * Updates display with the image from the specified film.
    */
    virtual void Update(intrusive_ptr<const Film> ip_film) = 0;

  protected:
    DisplayUpdateCallback();

  private:
    // Not implemented, not a value type.
    DisplayUpdateCallback(const DisplayUpdateCallback&);
    DisplayUpdateCallback &operator=(const DisplayUpdateCallback&);
  };

#endif // RENDERER_H