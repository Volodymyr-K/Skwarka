#include "ImageFilm.h"
#include <Math/Util.h>

ImageFilm::ImageFilm(size_t i_x_resolution, size_t i_y_resolution, shared_ptr<FilmFilter> ip_filter):
Film(i_x_resolution, i_y_resolution), m_x_resolution(i_x_resolution), m_y_resolution(i_y_resolution), mp_filter(ip_filter)
  {
  ASSERT(mp_filter != NULL);
  m_filter_x_width=mp_filter->GetXWidth();
  m_filter_y_width=mp_filter->GetYWidth();
  ASSERT(m_filter_x_width > 0.0 && m_filter_y_width > 0.0);

  m_crop_window_begin = Point2D_i(0, 0);
  m_crop_window_end = Point2D_i(m_x_resolution, m_y_resolution);

  m_pixels.assign(m_y_resolution, std::vector<ImageFilmPixel>(m_x_resolution, ImageFilmPixel()));
  }

void ImageFilm::AddSample(const Point2D_d &i_image_point, const Spectrum_f &i_spectrum, float i_alpha)
  {
  double image_x = i_image_point[0] - 0.5;
  double image_y = i_image_point[1] - 0.5;
  int x0 = (int)ceil (image_x - m_filter_x_width);
  int x1 = (int)floor(image_x + m_filter_x_width);
  int y0 = (int)ceil (image_y - m_filter_y_width);
  int y1 = (int)floor(image_y + m_filter_y_width);
  x0 = std::max(x0, m_crop_window_begin[0]);
  x1 = std::min(x1, m_crop_window_end[0]-1);
  y0 = std::max(y0, m_crop_window_begin[1]);
  y1 = std::min(y1, m_crop_window_end[1]-1);

  if (x1<x0 || y1<y0)
    return;

  // Loop over filter support and add sample to pixel arrays.
  for (int y = y0; y <= y1; ++y)
    for (int x = x0; x <= x1; ++x)
      {
      ImageFilmPixel &pixel = m_pixels[y][x];

      float filter_weight = (float)mp_filter->Evaluate(x-image_x, y-image_y);

      pixel.m_spectrum.AddWeighted(i_spectrum, filter_weight);
      pixel.m_alpha += i_alpha * filter_weight;
      pixel.m_weight_sum += filter_weight;
      }
  }

void ImageFilm::ClearFilm()
  {
  for (size_t y = 0; y < m_y_resolution; ++y)
    m_pixels[y].assign(m_x_resolution, ImageFilmPixel());
  }

bool ImageFilm::GetPixel(const Point2D_i &i_image_point, Spectrum_f &o_spectrum, float &o_alpha, bool i_clamp_values) const
  {
  ASSERT(i_image_point[0]>=0 && i_image_point[1]>=0 && i_image_point[0]<(int)m_x_resolution && i_image_point[1]<(int)m_y_resolution);

  // Check if the specified pixel is inside the crop window and return false if it is not.
  if (i_image_point[0]<m_crop_window_begin[0] || i_image_point[1]<m_crop_window_begin[1] || i_image_point[0]>=m_crop_window_end[0] || i_image_point[1]>=m_crop_window_end[1])
    return false;

  const ImageFilmPixel &pixel = m_pixels[i_image_point[1]][i_image_point[0]];

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
    return true;
    }
  else
    return false;
  }

void ImageFilm::GetSamplingExtent(Point2D_i &o_begin, Point2D_i &o_end) const
  {
  Point2D_d begin = Convert<double>(m_crop_window_begin) + Point2D_d(0.5-m_filter_x_width, 0.5-m_filter_y_width);
  Point2D_d end = Convert<double>(m_crop_window_end) + Point2D_d(0.5+m_filter_x_width, 0.5+m_filter_y_width);

  o_begin = Point2D_i( (int)floor(begin[0]), (int)floor(begin[1]) );
  o_end = Point2D_i( (int)floor(end[0]), (int)floor(end[1]) );
  }

void ImageFilm::SetCropWindow(const Point2D_i &i_begin, const Point2D_i &i_end)
  {
  ASSERT(i_begin[0]>=0 && i_begin[1]>=0 && i_end[0]<=(int)m_x_resolution && i_end[1]<=(int)m_y_resolution && "Crop window coordinates are out of range.");

  // Check if crop window coordinates are invalid.
  if (i_begin[0]>i_end[0] || i_begin[1]>i_end[1])
    {
    ASSERT(0 && "Crop window coordinates are invalid. Skipping");
    return;
    }

  m_crop_window_begin = Point2D_i(std::max(0,i_begin[0]), std::max(0,i_begin[1]));
  m_crop_window_end = Point2D_i(std::min((int)m_x_resolution,i_end[0]), std::min((int)m_y_resolution,i_end[1]));
  }