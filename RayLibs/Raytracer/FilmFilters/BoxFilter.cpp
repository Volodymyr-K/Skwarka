#include "BoxFilter.h"

BoxFilter::BoxFilter(double i_x_width, double i_y_width): FilmFilter(i_x_width, i_y_width)
  {
  }

double BoxFilter::Evaluate(double i_x, double i_y) const
  {
  if (i_x<-GetXWidth() || i_x>GetXWidth() || i_y<-GetYWidth() || i_y>GetYWidth())
    return 0.0;

  return 1.0;
  }