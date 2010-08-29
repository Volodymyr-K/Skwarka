#ifndef IMAGE_FILM_H
#define IMAGE_FILM_H

#include <Common\Common.h>
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/FilmFilter.h>
#include <Raytracer/Core/Spectrum.h>
#include <Math/BlockedArray.h>
#include <Math/Point2D.h>

/**
* Film implementation that stores the resulting image as a two-dimensional array.
* The final value of each film pixel is evaluated by filtering the nearby samples with a pluggable filter implementation.
* @sa FilmFilter
*/
class ImageFilm: public Film
  {
  public:
    /**
    * Creates an instance of ImageFilm with the specified resolution and FilmFilter implementation.
    * @param i_x_resolution X resolution. Should be greater than 0.
    * @param i_y_resolution Y resolution. Should be greater than 0.
    * @param ip_filter FilmFilter to be used for filtering pixel samples.
    */
    ImageFilm(size_t i_x_resolution, size_t i_y_resolution, intrusive_ptr<const FilmFilter> ip_filter);

    /**
    * Adds sample value to the film.
    */
    virtual void AddSample(const Point2D_d &i_image_point, const Spectrum_d &i_spectrum);

    /**
    * Clears the film.
    * The method removes all samples from the film saved so far.
    */
    virtual void ClearFilm();

    /**
    * Gets the Spectrum value for the specified pixel.
    * The method returns true on success and false if no samples contribute to the specified pixel or if the specified pixel is out of the cropping window.
    * @param i_image_point Coordinates of the pixel.
    * @param[out] o_spectrum Spectrum value of the pixel.
    * @param i_clamp_values If true, the Spectrum value will be clamped before returning.
    * @return true if the spectrum value were computed successfully and false if there is no samples contributing to the pixel.
    */
    virtual bool GetPixel(const Point2D_i &i_image_point, Spectrum_d &o_spectrum, bool i_clamp_values = true) const;

    /**
    * Returns the window in the image plane where samples need to be generated.
    * The window may be larger than the actual film resolution due to filter's width.
    * @param[out] o_begin Left lower corner of the sampling window.
    * @param[out] o_end Right upper corner of the sampling window (exclusive).
    */
    virtual void GetSamplingExtent(Point2D_i &o_begin, Point2D_i &o_end) const;

    /**
    * Sets cropping window for the film. The image will be generated only inside that window.
    * @param i_begin Left lower corner of the crop window. Should be in [0;m_x_resolution] x [0;m_y_resolution] range. Should be lesser or equal than i_end in both dimensions.
    * @param i_end Right upper corner of the crop window. Should be in [0;m_x_resolution] x [0;m_y_resolution] range. Should be higher or equal than i_begin in both dimensions.
    */
    void SetCropWindow(const Point2D_i &i_begin, const Point2D_i &i_end);

    /**
    * Gets cropping window for the film.
    * @param o_begin out Left lower corner of the crop window. Will be in [0;m_x_resolution] x [0;m_y_resolution] range. Will be lesser or equal than o_end in both dimensions.
    * @param o_end out Right upper corner of the crop window. Will be in [0;m_x_resolution] x [0;m_y_resolution] range. Will be higher or equal than o_begin in both dimensions.
    */
    void GetCropWindow(Point2D_i &o_begin, Point2D_i &o_end) const;

  private:
    // Internal types.
    struct ImageFilmPixel;

  private:
    ImageFilm(); // Empty default constructor for the boost serialization framework.

    // Needed for the boost serialization framework.  
    friend class boost::serialization::access;

    /**
    * Serializes to/from the specified Archive. This method is used by the boost serialization framework.
    */
    template<class Archive>
    void serialize(Archive &i_ar, const unsigned int i_version);

  private:
    size_t m_x_resolution, m_y_resolution;
    double m_filter_x_width, m_filter_y_width;

    // TBD: Cache filter values
    intrusive_ptr<const FilmFilter> mp_filter;
    BlockedArray<ImageFilmPixel> m_pixels;
    
    Point2D_i m_crop_window_begin, m_crop_window_end;
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

struct ImageFilm::ImageFilmPixel
  {
  ImageFilmPixel(): m_spectrum()
    {
    m_weight_sum = 0.0;
    }

  /**
  * Serializes ImageFilm::ImageFilmPixel to/from the specified Archive. This method is used by the boost serialization framework.
  */
  template<class Archive>
  void serialize(Archive &i_ar, const unsigned int i_version)
    {
    i_ar & m_spectrum;
    i_ar & m_weight_sum;
    }

  Spectrum_d m_spectrum;
  double m_weight_sum;
  };

// Don't store class info for ImageFilm::ImageFilmPixel.
BOOST_CLASS_IMPLEMENTATION(ImageFilm::ImageFilmPixel, boost::serialization::object_serializable)

template<class Archive>
void ImageFilm::serialize(Archive &i_ar, const unsigned int i_version)
  {
  i_ar & boost::serialization::base_object<Film>(*this);
  i_ar & m_x_resolution;
  i_ar & m_y_resolution;
  i_ar & m_filter_x_width;
  i_ar & m_filter_y_width;
  i_ar & mp_filter;
  i_ar & m_pixels;
  i_ar & m_crop_window_begin;
  i_ar & m_crop_window_end;
  }

// Register the derived class in the boost serialization framework.
#include <boost/serialization/export.hpp>
BOOST_CLASS_EXPORT(ImageFilm)

#endif // IMAGE_FILM_H