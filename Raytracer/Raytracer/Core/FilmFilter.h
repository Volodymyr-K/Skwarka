#ifndef FILM_FILTER_H
#define FILM_FILTER_H

/**
* An abstract class defining the contract for film filters.
* A film filter is a scalar function defined on 2D. The values of the function define the weights of the samples for a given pixel.
* Film filter has an X and Y widths, defining the region of the function domain where it can take non-zero values.
* @sa Film
*/
class FilmFilter
  {
  public:  
    /**
    * Returns the filter value for the specified X and Y coordinates.
    */
    virtual double Evaluate(const double &i_x, const double &i_y) const = 0;

    /**
    * Returns the maximum X coordinate for which the function is not zero.
    */
    double GetXWidth() const;

    /**
    * Returns the maximum Y coordinate for which the function is not zero.
    */
    double GetYWidth() const;

    virtual ~FilmFilter() {}

  protected:
    FilmFilter(const double &i_x_width, const double &i_y_width);

  private:
    // Not implemented, not a value type.
    FilmFilter();
    FilmFilter(const FilmFilter&);
    FilmFilter &operator=(const FilmFilter&);

  private:
    double m_x_width, m_y_width;
  };

#endif // FILM_FILTER_H