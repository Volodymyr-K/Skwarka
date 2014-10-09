#include "FilmFilter.h"
#include <Common/Common.h>

FilmFilter::FilmFilter(double i_x_width, double i_y_width):
m_x_width(i_x_width), m_y_width(i_y_width)
  {
  ASSERT(i_x_width>0.0);
  ASSERT(i_y_width>0.0);
  }

double FilmFilter::GetXWidth() const
  {
  return m_x_width;
  }

double FilmFilter::GetYWidth() const
  {
  return m_y_width;
  }