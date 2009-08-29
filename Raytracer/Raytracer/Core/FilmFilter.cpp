#include "FilmFilter.h"

FilmFilter::FilmFilter(const double &i_x_width, const double &i_y_width):
m_x_width(i_x_width), m_y_width(i_y_width)
  {
  }

double FilmFilter::GetXWidth() const
  {
  return m_x_width;
  }

double FilmFilter::GetYWidth() const
  {
  return m_y_width;
  }