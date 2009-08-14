#include "Film.h"

Film::Film(size_t i_x_resolution, size_t i_y_resolution, shared_ptr<FilmFilter> ip_filter):
m_x_resolution(i_x_resolution), m_y_resolution(i_y_resolution), mp_filter(ip_filter)
  {
  m_film_window[0] = Point2D_i(0, 0);
  m_film_window[1] = Point2D_i(m_x_resolution-1, m_y_resolution-1);

  m_pixels.assign(i_y_resolution, std::vector<FilmPixel>(i_x_resolution, FilmPixel()));
  }

void Film::SetCropWindow(const Point2D_d &i_start, const Point2D_d &i_end)
  {
  if (i_start[0]<0.0 || i_start[1]<0.0 || i_end[0]>1.0 || i_end[1]>1.0)
    {
    Log::Warning("Crop window coordinates are out of range. Skipping.");
    return;
    }

  if (i_start[0]>=i_end[0] || i_start[1]>=i_end[1])
    {
    Log::Warning("Crop window coordinates are invalid. Skipping.");
    return;
    }

  m_film_window[0] = Convert<int>(Point2D_d(ceil(m_x_resolution*i_start[0]), ceil(m_y_resolution*i_start[1])));
  m_film_window[1] = Convert<int>(Point2D_d(floor(m_x_resolution*i_end[0]), floor(m_y_resolution*i_end[1])));
  }

void Film::GetSampleExtent(Point2D_i &o_start, Point2D_i &o_end) const
  {
  o_start = Convert<int>(Convert<double>(m_film_window[0]) + Point2D_d(0.5-mp_filter->GetXWidth(), 0.5-mp_filter->GetYWidth()));
  o_end = Convert<int>(Convert<double>(m_film_window[1]) + Point2D_d(0.5+mp_filter->GetXWidth(), 0.5+mp_filter->GetYWidth()));
  }