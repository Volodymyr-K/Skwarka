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

    FilmFilter() {} // Empty default constructor for the boost serialization framework.

  private:
    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    // Not implemented, not a value type.
    FilmFilter(const FilmFilter&);
    FilmFilter &operator=(const FilmFilter&);

  private:
    double m_x_width, m_y_width;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template<class Archive>
void FilmFilter::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<ReferenceCounted>(*this);
  i_ar & m_x_width;
  i_ar & m_y_width;
  }

#endif // FILM_FILTER_H