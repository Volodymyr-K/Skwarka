#ifndef UNIFORM_IMAGE_PIXELS_ORDER_H
#define UNIFORM_IMAGE_PIXELS_ORDER_H

#include <Math/Point2D.h>
#include <Raytracer/Core/Sampler.h>

/**
* ImagePixelsOrder strategy implementation that produces image pixels uniformly across the whole image.
* The pixels are ordered so that at each moment all the pixels produced so far are uniformly distributed across the image space.
* The algorithm implicitly constructs a binary tree over the image space so that each node is a sub-rectangle of the image space. The root of the tree is
* the whole image and the leaves are individual pixels. A path from the root to a leaf can be represented by a sequence of bits, or equivalently, as an integer.
* For each node in the path the corresponding bit defines the next child node in the path. Each internal node is always divided by the axis that has larger extent.
* The sequence of bits is represented as an unsigned integer variable (see m_path_bitmask member field). The 0-th bit corresponds to the root's child, the 1-st bit corresponds
* to the child of the root's child and so on. All we need to do to move to the next pixel is to increment the integer by 1.
*/
class UniformImagePixelsOrder: public ImagePixelsOrder
  {
  public:
    UniformImagePixelsOrder();

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
    Point2D_i m_image_begin, m_image_end;

    size_t m_size_x, m_size_y;
    size_t m_path_bitmask, m_next_pixel_index;
  };

#endif // HIERARCHICAL_IMAGE_PIXELS_ORDER_H
