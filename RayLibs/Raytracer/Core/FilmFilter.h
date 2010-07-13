#ifndef FILM_FILTER_H
#define FILM_FILTER_H

#include <Common/Common.h>

/**
* An abstract class defining the contract for image film filters.
* A film filter is a scalar function defined on 2D. The values of the function define the weights of the samples for a given pixel.
* Film filter has an X and Y widths, defining the region of the function domain where it can take non-zero values.
* @sa ImageFilm
*/
class FilmFilter: public ReferenceCounted
  {
  public:  
    /**
    * Returns the filter value for the specified X and Y coordinates.
    */
    virtual double Evaluate(double i_x, double i_y) const = 0;

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
    FilmFilter(double i_x_width, double i_y_width);

  private:
    // Not implemented, not a value type.
    FilmFilter();
    FilmFilter(const FilmFilter&);
    FilmFilter &operator=(const FilmFilter&);

  private:
    double m_x_width, m_y_width;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Serializes FilmFilter to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, FilmFilter &i_filter, const unsigned int i_version)
  {
  /*
  Nothing to do here, everything must be serialized by the derived classes.

  We can't serialize the member fields here because there's no default constructor for the class
  and save_construct_data/load_construct_data functions can't be used either
  because it is impossible to create an instance of the abstract class.
  */

  // Just call the serialization for the base ReferenceCounted class.
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_filter);
  }

#endif // FILM_FILTER_H