#ifndef FILM_H
#define FILM_H

#include <vector>
#include "Common\Common.h"
#include <Math/Geometry.h>
#include <Math/Util.h>
#include <Math/Constants.h>
#include "FilmFilter.h"
#include "Spectrum.h"

/**
* Film holding the generated image.
* The contribution of each camera ray to the image is represented as a sample on the film.
* The final value of a film pixel is evaluated by filtering the nearby samples.
* @sa Camera, FilmFilter
*/
class Film
  {
  private:
    // Internal types.
    struct FilmPixel;

  public:
    /**
    * Creates an instance of Film with the specified resolution and FilmFilter implementation.
    */
    Film(size_t i_x_resolution, size_t i_y_resolution, shared_ptr<FilmFilter> ip_filter);

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
    void AddSample(const Point2D_d &i_image_point, const Spectrum_f &i_spectrum, float i_alpha);

    /**
    * Clears the film.
    * The method removes all samples from the film saved so far.
    */
    void ClearFilm();

    /**
    * Sets cropping window for the film. The image will be generated only inside that window.
    * @param i_begin Left lower corner of the crop window. Should be in [0;1]^2 region. Should be lesser than i_end in both dimensions.
    * @param i_end Right upper corner of the crop window. Should be in [0;1]^2 region. Should be higher than i_begin in both dimensions.
    */
    void SetCropWindow(const Point2D_d &i_begin, const Point2D_d &i_end);

    /**
    * Returns the window in the image plane where samples need to be generated. The window may be larger than the actual film resolution due to filter's width.
    * @param[out] o_begin Left lower corner of the sampling window.
    * @param[out] o_end Right upper corner of the sampling window (exclusive).
    */
    void GetSampleExtent(Point2D_i &o_begin, Point2D_i &o_end) const;

    /**
    * Returns the Spectrum and alpha values for the specified pixel.
    * @param i_image_point Coordinates of the pixel.
    * @param[out] o_spectrum Spectrum value of the pixel.
    * @param[out] o_alpha Alpha value of the pixel.
    * @param i_clamp_values If true, the Spectrum and alpha values will be clamped before returning.
    */
    void GetPixel(const Point2D_i &i_image_point, Spectrum_f &o_spectrum, float &o_alpha, bool i_clamp_values = true) const;

  private:
    // Not implemented, not a value type.
    Film();
    Film(const Film&);
    Film &operator=(const Film&);

  private:
    size_t m_x_resolution, m_y_resolution;
    shared_ptr<FilmFilter> mp_filter; // TBD: Cache filter values
    double m_filter_x_width, m_filter_y_width;

    Point2D_i m_film_window[2];

    std::vector< std::vector<FilmPixel> > m_pixels; // TBD: rewrite as blocked array
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

struct Film::FilmPixel
  {
  FilmPixel(): m_spectrum()
    {
    m_alpha = 0.f;
    m_weight_sum = 0.f;
    }

  Spectrum_f m_spectrum;
  float m_alpha, m_weight_sum;
  };

inline size_t Film::GetXResolution() const
  {
  return m_x_resolution;
  }

inline size_t Film::GetYResolution() const
  {
  return m_y_resolution;
  }

inline void Film::GetPixel(const Point2D_i &i_image_point, Spectrum_f &o_spectrum, float &o_alpha, bool i_clamp_values) const
  {
  ASSERT(i_image_point[0]>=0 && i_image_point[1]>=0 && i_image_point[0]<(int)m_x_resolution && i_image_point[1]<(int)m_y_resolution);
  const FilmPixel &pixel = m_pixels[i_image_point[1]][i_image_point[0]];

  if (pixel.m_weight_sum != 0.f)
    {
    float invWt = 1.f / pixel.m_weight_sum;
    o_spectrum=pixel.m_spectrum*invWt;
    o_alpha=pixel.m_alpha*invWt;
    if (i_clamp_values)
      {
      o_spectrum[0]=MathRoutines::Clamp(o_spectrum[0], 0.f, FLT_INF);
      o_spectrum[1]=MathRoutines::Clamp(o_spectrum[1], 0.f, FLT_INF);
      o_spectrum[2]=MathRoutines::Clamp(o_spectrum[2], 0.f, FLT_INF);
      o_alpha=MathRoutines::Clamp(o_alpha, 0.f, 1.f);
      }
    }
  else
    {
    o_spectrum[0]=o_spectrum[1]=o_spectrum[2]=0.f;
    o_alpha=0.f;
    }
  }

#endif // FILM_H