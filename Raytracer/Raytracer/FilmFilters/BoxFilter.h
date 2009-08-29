#ifndef BOX_FILTER_H
#define BOX_FILTER_H

#include <Raytracer\Core\FilmFilter.h>

class BoxFilter: public FilmFilter
  {
  public:
    BoxFilter(const double &i_x_width, const double &i_y_width);

    double Evaluate(const double &i_x, const double &i_y) const;
  };

#endif // BOX_FILTER_H