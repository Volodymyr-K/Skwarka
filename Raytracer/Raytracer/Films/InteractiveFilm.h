#ifndef INTERACTIVE_FILM_H
#define INTERACTIVE_FILM_H

#include <Common\Common.h>
#include <Raytracer/Core/Film.h>
#include <Raytracer/Core/FilmFilter.h>
#include "ImageFilm.h"
#include <Raytracer/Core/Spectrum.h>
#include <Math/Point2D.h>
#include <vector>

/**
* Film implementation that approximates the whole image from all the samples added so far.
* The implementation is useful for displaying the generated image in "real time" while new samples are get added to the film.
* When only few samples are added to the film the image looks coarse but it gets finer as more samples are added to the film.
* The class constructs several images which can be considered as layers. The lowest layer is the resulting image with the specified resolution. 
* The next layer is the image with half the resolution (or other fixed fraction) of the original image, and so on. The last layer is an image of one pixel size.
* The class uses ImageFilm as the underlying implementation for these layer images. When a sample is added to the film it is added to all layers.
* Similarly, when a pixel is read from the film it is read from the lowest layer first. If the pixel can not be computed for the layer, it is read from the next layer and so on.
* An important point is that though each next layer is a fraction size of the previous layer, the filter width is constant. Thus a sample added to a higher layer contributes to more
* pixels of the resulting image. The layers can be considered as different films positioned at different distance from the lens.
* @sa ImageFilm, FilmFilter
*/
class InteractiveFilm: public Film
  {
  public:
    /**
    * Creates an instance of InteractiveFilm with the specified resolution and FilmFilter implementation.
    * @param i_x_resolution X resolution. Should be greater than 0.
    * @param i_y_resolution Y resolution. Should be greater than 0.
    * @param ip_filter FilmFilter to be used for filtering pixel samples.
    */
    InteractiveFilm(size_t i_x_resolution, size_t i_y_resolution, intrusive_ptr<const FilmFilter> ip_filter);

    /**
    * Adds sample value to the film.
    */
    virtual void AddSample(const Point2D_d &i_image_point, const Spectrum_f &i_spectrum);

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
    virtual bool GetPixel(const Point2D_i &i_image_point, Spectrum_f &o_spectrum, bool i_clamp_values = true) const;

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

  private:
    size_t m_x_resolution, m_y_resolution;
    Point2D_i m_crop_window_begin, m_crop_window_end;

    // Vector of image layers.
    std::vector<intrusive_ptr<ImageFilm> > m_image_films;

    // Defines the size factor between consecutive layers.
    // Smaller factor results in a better approximation but takes more processing time when adding samples.
    static const unsigned int FRACTION_FACTOR = 4;
  };

#endif // INTERACTIVE_FILM_H