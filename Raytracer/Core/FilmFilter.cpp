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

BoxFilter::BoxFilter(const double &i_x_width, const double &i_y_width):
    FilmFilter(i_x_width, i_y_width)
  {
  }

double BoxFilter::Evaluate(const double &i_x, const double &i_y) const
  {
  return 1.0;
  }