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

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
* Saves the data which is needed to construct BoxFilter to the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void save_construct_data(Archive &i_ar, const BoxFilter *ip_filter, const unsigned int i_version)
  {
  double x_width = ip_filter->GetXWidth();
  double y_width = ip_filter->GetYWidth();

  i_ar << x_width;
  i_ar << y_width;
  }

/**
* Constructs BoxFilter with the data from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void load_construct_data(Archive &i_ar, BoxFilter *ip_filter, const unsigned int i_version)
  {
  double x_width, y_width;
  i_ar >> x_width;
  i_ar >> y_width;
  ::new(ip_filter)BoxFilter(x_width, y_width);
  }

/**
* Serializes BoxFilter to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<class Archive>
void serialize(Archive &i_ar, BoxFilter &i_filter, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<FilmFilter>(i_filter);
  }

// Register the derived class in the boost serialization framework.
BOOST_CLASS_EXPORT(BoxFilter)

#endif // BOX_FILTER_H