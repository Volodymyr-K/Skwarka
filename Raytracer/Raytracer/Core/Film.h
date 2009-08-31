#ifndef FILM_H
#define FILM_H

#include <vector>
#include "Common\Common.h"
#include <Math\Geometry.h>
#include <Math\Util.h>
#include <Math\Constants.h>
#include "FilmFilter.h"
#include "Spectrum.h"

class Film
  {
  private:
    // Internal type
    struct FilmPixel;

  public:
    Film(size_t i_x_resolution, size_t i_y_resolution, shared_ptr<FilmFilter> ip_filter);

    size_t GetXResolution() const;

    size_t GetYResolution() const;

    void AddSample(const Point2D_d &i_image_point, const Spectrum_f &i_spectrum, float i_alpha);

    void SetCropWindow(const Point2D_d &i_begin, const Point2D_d &i_end);

    void GetSampleExtent(Point2D_i &o_begin, Point2D_i &o_end) const;

    void GetPixel(size_t i_x, size_t i_y, Spectrum_f &o_spectrum, float &o_alfa, bool i_clamp_values = true) const;

  private:
    // not implemented
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

inline void Film::GetPixel(size_t i_x, size_t i_y, Spectrum_f &o_spectrum, float &o_alfa, bool i_clamp_values) const
  {
  ASSERT(i_x<m_x_resolution && i_y<m_y_resolution);
  const FilmPixel &pixel = m_pixels[i_y][i_x];

  if (pixel.m_weight_sum != 0.f)
    {
    float invWt = 1.f / pixel.m_weight_sum;
    o_spectrum=pixel.m_spectrum*invWt;
    o_alfa=pixel.m_alpha*invWt;
    if (i_clamp_values)
      {
      o_spectrum[0]=Clamp(o_spectrum[0], 0.f, FLT_INF);
      o_spectrum[1]=Clamp(o_spectrum[1], 0.f, FLT_INF);
      o_spectrum[2]=Clamp(o_spectrum[2], 0.f, FLT_INF);
      o_alfa=Clamp(o_alfa, 0.f, 1.f);
      }
    }
  }

#endif // FILM_H