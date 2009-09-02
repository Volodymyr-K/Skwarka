#include "BoxFilter.h"

BoxFilter::BoxFilter(const double &i_x_width, const double &i_y_width): FilmFilter(i_x_width, i_y_width)
  {
  }

double BoxFilter::Evaluate(const double &i_x, const double &i_y) const
  {
  if (i_x<-GetXWidth() || i_x>GetXWidth() || i_y<-GetYWidth() || i_y>GetYWidth())
    return 0.0;

  return 1.0;
  }