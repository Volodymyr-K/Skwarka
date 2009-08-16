#include "Film.h"
#include <utility>

Film::Film(size_t i_x_resolution, size_t i_y_resolution, shared_ptr<FilmFilter> ip_filter):
m_x_resolution(i_x_resolution), m_y_resolution(i_y_resolution), mp_filter(ip_filter)
  {
  ASSERT(mp_filter != NULL);
  m_filter_x_width=mp_filter->GetXWidth();
  m_filter_y_width=mp_filter->GetYWidth();
  ASSERT(m_filter_x_width > 0.0 && m_filter_y_width > 0.0);

  m_film_window[0] = Point2D_i(0, 0);
  m_film_window[1] = Point2D_i(m_x_resolution-1, m_y_resolution-1);

  m_pixels.assign(i_y_resolution, std::vector<FilmPixel>(i_x_resolution, FilmPixel()));
  }

void Film::AddSample(const Point2D_d &i_image_point, const Spectrum_f &i_spectrum, float i_alpha)
  {
  double image_x = i_image_point[0] - 0.5;
  double image_y = i_image_point[1] - 0.5;
  int x0 = (int)ceil (image_x - m_filter_x_width);
  int x1 = (int)floor(image_x + m_filter_x_width);
  int y0 = (int)ceil (image_y - m_filter_y_width);
  int y1 = (int)floor(image_y + m_filter_y_width);
  x0 = std::max(x0, m_film_window[0][0]);
  x1 = std::min(x1, m_film_window[1][0]);
  y0 = std::max(y0, m_film_window[0][1]);
  y1 = std::min(y1, m_film_window[1][1]);

  if (x1<x0 || y1<y0)
    return;

  // Loop over filter support and add sample to pixel arrays
  for (int y = y0; y <= y1; ++y)
    for (int x = x0; x <= x1; ++x)
      {
      FilmPixel &pixel = m_pixels[y][x];

      float filter_weight = (float)mp_filter->Evaluate(x-image_x, y-image_y);

      pixel.m_spectrum.AddWeighted(i_spectrum, filter_weight);
      pixel.m_alpha += i_alpha * filter_weight;
      pixel.m_weight_sum += filter_weight;
      }
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
  m_film_window[1] = Convert<int>(Point2D_d(ceil(m_x_resolution*i_end[0]), ceil(m_y_resolution*i_end[1])));
  }

void Film::GetSampleExtent(Point2D_i &o_start, Point2D_i &o_end) const
  {
  o_start = Convert<int>(Convert<double>(m_film_window[0]) + Point2D_d(0.5-m_filter_x_width, 0.5-m_filter_y_width));
  o_end = Convert<int>(Convert<double>(m_film_window[1]) + Point2D_d(0.5+m_filter_x_width, 0.5+m_filter_y_width));
  }