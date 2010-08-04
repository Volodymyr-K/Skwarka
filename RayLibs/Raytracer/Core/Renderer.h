#ifndef RENDERER_H
#define RENDERER_H

#include <Common/Common.h>
#include "Camera.h"
#include "Film.h"
#include "tbb/tick_count.h"

class DisplayUpdateCallback;

/**
* A central abstract class defining the contract for scene renderers.
* Renderers are responsible to render the image seen from a camera onto the camera's film.
* Renderer also provides display updating functionality which allows periodically updating display with image rendered so far.
*/
class Renderer: public ReferenceCounted
  {
  public:
    /**
    * Renders the scene for the specified camera.
    * The rendered image will be saved to the camera's film. The film is cleared before rendering, so the previous image will be lost.
    * @param ip_camera Camera in the scene for which the image is to be rendered.
    */
    virtual void Render(intrusive_ptr<const Camera> ip_camera, bool i_low_thread_priority = false) const = 0;

    /**
    * Sets callback that will be called periodically to update display with the currently rendered image.
    * The callback will be called by Render() method periodically with the specified period (in seconds).
    * @param ip_display_update_callback Pointer to the callback. If NULL, the callback won't be called.
    * @param i_update_period Time period in seconds. Should be greater than zero.
    */
    void SetDisplayUpdateCallback(intrusive_ptr<DisplayUpdateCallback> ip_display_update_callback, double i_update_period = 1.0);

  protected:
    /**
    * Creates Renderer for the specified scene.
    */
    Renderer();

    /**
    * A helper method for the derived classes that calls DisplayUpdateCallback if the specified time period has already passed since the last call or if i_force_update is true.
    * Implementations need to call this method periodically to ensure that the display is updated frequently enough.
    * @param ip_film Film with the rendered image. Should not be NULL.
    * @param i_force_update If true the callback will be called regardless of whether the time period has passed.
    *
    * @warning Implementations need to make sure that this method is not called concurrently because DisplayUpdateCallbacks are not necessarily thread-safe.
    */
    void _UpdateDisplay(intrusive_ptr<const Film> ip_film, bool i_force_update = false) const;

  private:
    // Not implemented, not a value type.
    Renderer(const Renderer&);
    Renderer &operator=(const Renderer&);

  private:
    intrusive_ptr<DisplayUpdateCallback> mp_display_update_callback;
    double m_update_period;

    // Time when the DisplayUpdateCallback was called last time (or the time of class construction if it was never called).
    mutable tbb::tick_count m_last_display_update;
  };

/**
* This is a callback interface for updating display with the image rendered so far.
* The callbacks are called by Renderer::Render() method periodically to keep the display updated with the currently rendered image.
*/
class DisplayUpdateCallback: public ReferenceCounted
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