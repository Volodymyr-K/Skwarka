#ifndef BOX_FILTER_H
#define BOX_FILTER_H

#include <Raytracer/Core/FilmFilter.h>

/**
* Simplest implementation of the FilmFilter interface returning a constant value everywhere inside the filter's width.
*/
class BoxFilter: public FilmFilter
  {
  public:
    BoxFilter(const double &i_x_width, const double &i_y_width);

    /**
    * Returns value 1.0 everywhere inside the filter's width.
    */
    double Evaluate(const double &i_x, const double &i_y) const;
  };

#endif // BOX_FILTER_H