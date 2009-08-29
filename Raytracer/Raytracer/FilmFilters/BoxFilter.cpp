#include "BoxFilter.h"

BoxFilter::BoxFilter(const double &i_x_width, const double &i_y_width): FilmFilter(i_x_width, i_y_width)
  {
  }

double BoxFilter::Evaluate(const double &i_x, const double &i_y) const
  {
  return 1.0;
  }