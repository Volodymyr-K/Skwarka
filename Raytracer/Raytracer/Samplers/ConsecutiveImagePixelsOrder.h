#ifndef CONSECUTIVE_IMAGE_PIXELS_ORDER_H
#define CONSECUTIVE_IMAGE_PIXELS_ORDER_H

#include <Math/Point2D.h>
#include <Raytracer/Core/Sampler.h>

/**
* ImagePixelsOrder strategy implementation that implements a consecutive image pixels order.
* Pixels are ordered from top to bottom and from left to right.
*/
class ConsecutiveImagePixelsOrder: public ImagePixelsOrder
  {
  public:
    ConsecutiveImagePixelsOrder();

    /**
    * Sets image size.
    * @param i_image_begin Left lower corner of the sampling window.
    * @param i_image_end Right upper corner of the sampling image (exclusive).
    */
    virtual void SetImageSize(const Point2D_i &i_image_begin, const Point2D_i &i_image_end);

    /**
    * Returns total number of image pixels.
    */
    virtual size_t GetTotalPixelsNum() const;

    /**
    * Resets the pixel order.
    * The next call to GetNextPixel() will get the first image pixel in the sense of the order defined by the strategy.
    */
    virtual void Reset();

    /**
    * Gets the next image pixel.
    * param[out] o_image_point Next image point.
    * return true if the next image pixel was successfully get and false if there's no more pixels.
    */
    virtual bool GetNextPixel(Point2D_i &o_image_pixel);

  private:
    Point2D_i m_image_begin, m_image_end, m_next_pixel;
  };

#endif // CONSECUTIVE_IMAGE_PIXELS_ORDER_H
