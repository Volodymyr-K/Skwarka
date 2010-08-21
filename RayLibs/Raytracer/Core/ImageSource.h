#ifndef IMAGE_SOURCE_H
#define IMAGE_SOURCE_H

#include <Common/Common.h>

/**
* An abstract class defining the contract for getting 2D array of values.
* Implementations of the class provide 2D array of values (i.e. image) of the specified type (see template parameter of the class).
* This allows for having a convenient way to convert 2D array of values of one type to another type.
* For example, the implementation can store image in RGB format where each component is defined by unsigend char and return the image in SpectrumCoef_f format.
* More importantly, the implementations are also used by MIPMap to serialize the input image in a more compact way (as in the above example with RGB image).
* 
* @warning The serialization logic for MIPMap depends on the assumption that the implementations of this interface are constant in the sense that the same
* instance always returns the same image. In other words implementations should not have mutable state or depend on external sources (e.g. files).
* Otherwise the deserialized MIPMap is not guaranteed to be the same that the serialized one.
*/
template <typename T>
class ImageSource: public ReferenceCounted
  {
  public:

    /**
    * Gets 2D array of values (image).
    */
    virtual void GetImage(std::vector<std::vector<T> > &o_image) const = 0;

    /**
    * Returns height of the image, i.e. the size of the outer vector which defines the image.
    */
    virtual size_t GetHeight() const = 0;

    /**
    * Returns width of the image, i.e. the size of the inner vector which defines the image.
    */
    virtual size_t GetWidth() const = 0;

    ~ImageSource();

  protected:
    ImageSource();

  public:
    // Not implemented, not a value type.
    ImageSource(const ImageSource&);
    ImageSource &operator=(const ImageSource&);
  };

/////////////////////////////////////////// IMPLEMENTATION ////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////

template <typename T>
ImageSource<T>::ImageSource()
  {
  }

template <typename T>
ImageSource<T>::~ImageSource()
  {
  }

/**
* Serializes ImageSource to/from the specified Archive. This method is used by the boost serialization framework.
*/
template<typename T, class Archive>
void serialize(Archive &i_ar, ImageSource<T> &i_image_source, const unsigned int i_version)
  {
  // Nothing to do here, everything must be serialized by the derived classes.

  // Just call the serialization for the base ReferenceCounted class.
  i_ar & boost::serialization::base_object<ReferenceCounted>(i_image_source);
  }

#endif // IMAGE_SOURCE_H