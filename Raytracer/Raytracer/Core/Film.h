#ifndef FILM_H
#define FILM_H

#include <Common\Common.h>
#include <Math/Point2D.h>
#include <Math/Constants.h>
#include "Spectrum.h"

/**
* An abstract class defining the contract for camera's film.
* The contribution of each camera ray to the image is represented as a sample on the film.
* @sa Camera
*/
class Film: public ReferenceCounted
  {
  public:
    /**
    * Returns number of pixels in X direction.
    */
    size_t GetXResolution() const;

    /**
    * Returns number of pixels in Y direction.
    */
    size_t GetYResolution() const;

    /**
    * Adds sample value to the film.
    */
    virtual void AddSample(const Point2D_d &i_image_point, const Spectrum_f &i_spectrum, float i_alpha) = 0;

    /**
    * Clears the film.
    * The method removes all samples from the film saved so far.
    */
    virtual void ClearFilm() = 0;

    /**
    * Gets the Spectrum and alpha values for the specified pixel.
    * @param i_image_point Coordinates of the pixel.
    * @param[out] o_spectrum Spectrum value of the pixel.
    * @param[out] o_alpha Alpha value of the pixel.
    * @param i_clamp_values If true, the Spectrum and alpha values will be clamped before returning.
    * @return true if the spectrum and alpha values were computed successfully and false otherwise (e.g. if there's no enough samples in the pixel's vicinity).
    */
    virtual bool GetPixel(const Point2D_i &i_image_point, Spectrum_f &o_spectrum, float &o_alpha, bool i_clamp_values = true) const = 0;

    /**
    * Returns the window in the image plane where samples need to be generated.
    * The window may differ from the actual film resolution (e.g. due to filtering sampling values).
    * @param[out] o_begin Left lower corner of the sampling window.
    * @param[out] o_end Right upper corner of the sampling window (exclusive).
    */
    virtual void GetSamplingExtent(Point2D_i &o_begin, Point2D_i &o_end) const = 0;

    virtual ~Film() {}

  protected:
    /**
    * Creates an instance of Film with the specified resolution.
    * @param i_x_resolution X resolution. Should be greater than 0.
    * @param i_y_resolution Y resolution. Should be greater than 0.
    */
    Film(size_t i_x_resolution, size_t i_y_resolution);

  private:
    // Not implemented, not a value type.
    Film();
    Film(const Film&);
    Film &operator=(const Film&);

  private:
    size_t m_x_resolution, m_y_resolution;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

inline Film::Film(size_t i_x_resolution, size_t i_y_resolution):
  m_x_resolution(i_x_resolution), m_y_resolution(i_y_resolution)
    {
    ASSERT(i_x_resolution>0 && i_y_resolution>0);
    }

inline size_t Film::GetXResolution() const
  {
  return m_x_resolution;
  }

inline size_t Film::GetYResolution() const
  {
  return m_y_resolution;
  }

#endif // FILM_H