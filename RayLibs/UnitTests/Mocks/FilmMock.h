#ifndef FILM_MOCK_H
#define FILM_MOCK_H

#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/Spectrum.h>
#include <Math/Point2D.h>

/*
Film mock implementation.
Always generates the same Spectrum value for all pixels.
*/
class FilmMock: public Film
  {
  public:
    FilmMock(size_t i_x_resolution, size_t i_y_resolution): Film(i_x_resolution, i_y_resolution)
      {
      }

    virtual void AddSample(const Point2D_d &i_image_point, const Spectrum_d &i_spectrum)
      {
      }

    virtual void ClearFilm()
      {
      }

    virtual bool GetPixel(const Point2D_i &i_image_point, Spectrum_d &o_spectrum, bool i_clamp_values = true) const
      {
      o_spectrum=Spectrum_d(1.0,1.0,1.0);
      return true;
      }

    virtual void GetSamplingExtent(Point2D_i &o_begin, Point2D_i &o_end) const
      {
      o_begin=Point2D_i(0,0);
      o_end=Point2D_i(GetXResolution(),GetYResolution());
      }
  };

#endif // FILM_MOCK_H