#ifndef FILM_FILTER_MOCK_H
#define FILM_FILTER_MOCK_H

#include <Raytracer/Core/FilmFilter.h>

/*
FilmFilter mock implementation.
Always generates the same value (just like box filter).
*/
class FilmFilterMock: public FilmFilter
  {
  public:
    FilmFilterMock(double i_x_width, double i_y_width) : FilmFilter(i_x_width, i_y_width)
      {
      }

    double Evaluate(double i_x, double i_y) const
      {
      if (i_x<-GetXWidth() || i_x>GetXWidth() || i_y<-GetYWidth() || i_y>GetYWidth())
        return 0.0;

      return 1.0;
      }
  };

#endif // FILM_FILTER_MOCK_H