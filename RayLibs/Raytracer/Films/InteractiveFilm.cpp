#include "InteractiveFilm.h"
#include <Math/MathRoutines.h>

InteractiveFilm::InteractiveFilm(size_t i_x_resolution, size_t i_y_resolution, intrusive_ptr<const FilmFilter> ip_filter):
Film(i_x_resolution, i_y_resolution), m_x_resolution(i_x_resolution), m_y_resolution(i_y_resolution), mp_filter(ip_filter)
  {
  ASSERT(i_x_resolution>0 && i_y_resolution>0);
  ASSERT(ip_filter != NULL);

  m_crop_window_begin = Point2D_i(0, 0);
  m_crop_window_end = Point2D_i(m_x_resolution, m_y_resolution);

  if (i_x_resolution==0 || i_y_resolution==0) return;

  size_t layer_x_resolution=m_x_resolution, layer_y_resolution=m_y_resolution;
  while(true)
    {
    m_image_films.push_back( intrusive_ptr<ImageFilm>(new ImageFilm(layer_x_resolution, layer_y_resolution, ip_filter)) );

    // Break from the loop after we reached the highest layer which is one pixel sized.
    if (layer_x_resolution==1 && layer_y_resolution==1) break;
    layer_x_resolution = (layer_x_resolution+FRACTION_FACTOR-1)/FRACTION_FACTOR;
    layer_y_resolution = (layer_y_resolution+FRACTION_FACTOR-1)/FRACTION_FACTOR;
    }

  }

intrusive_ptr<const FilmFilter> InteractiveFilm::GetFilmFilter() const
  {
  return mp_filter;
  }

void InteractiveFilm::AddSample(const Point2D_d &i_image_point, const Spectrum_d &i_spectrum)
  {
  Point2D_d image_point(i_image_point);

  // Add sample to all layers, the pixel coordinates are divided by the fraction factor for each next layer.
  for(size_t i=0;i<m_image_films.size();++i)
    {
    m_image_films[i]->AddSample(image_point, i_spectrum);
    image_point/=FRACTION_FACTOR;
    }
  }

void InteractiveFilm::ClearFilm()
  {
  for(size_t i=0;i<m_image_films.size();++i)
    m_image_films[i]->ClearFilm();
  }

bool InteractiveFilm::GetPixel(const Point2D_i &i_image_point, Spectrum_d &o_spectrum, bool i_clamp_values) const
  {
  ASSERT(i_image_point[0]>=0 && i_image_point[1]>=0 && i_image_point[0]<(int)m_x_resolution && i_image_point[1]<(int)m_y_resolution);

  if (m_image_films.empty())
    return false;

  // Check if the specified pixel is inside the crop window and return false if it is not.
  if (i_image_point[0]<m_crop_window_begin[0] || i_image_point[1]<m_crop_window_begin[1] || i_image_point[0]>=m_crop_window_end[0] || i_image_point[1]>=m_crop_window_end[1])
    return false;

  // Read pixel value from the layers until it is read successfully.
  Point2D_i image_point(i_image_point);
  for(size_t i=0;i<m_image_films.size();++i)
    {
    if (m_image_films[i]->GetPixel(image_point, o_spectrum, i_clamp_values))
      return true;

    image_point/=FRACTION_FACTOR;
    }

  return false;
  }

void InteractiveFilm::GetSamplingExtent(Point2D_i &o_begin, Point2D_i &o_end) const
  {
  if (m_image_films.empty()==false)
    m_image_films[0]->GetSamplingExtent(o_begin, o_end);
  else
    {
    o_begin=Point2D_i(0,0);
    o_end=Point2D_i(0,0);
    }
  }

void InteractiveFilm::SetCropWindow(const Point2D_i &i_begin, const Point2D_i &i_end)
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

  Point2D_i begin(i_begin), end(i_end);
  for(size_t i=0;i<m_image_films.size();++i)
    {
    m_image_films[i]->SetCropWindow(begin, end);

    // While dividing, the begin point is rounded down but the end point is rounded up.
    // This is so because the end point is an exclusive bound and we don't want to exclude more pixels than requested.
    begin/=FRACTION_FACTOR;
    end=(end+Point2D_i(FRACTION_FACTOR-1,FRACTION_FACTOR-1))/FRACTION_FACTOR;
    }
  }

void InteractiveFilm::GetCropWindow(Point2D_i &o_begin, Point2D_i &o_end) const
  {
  o_begin = m_crop_window_begin;
  o_end = m_crop_window_end;
  }