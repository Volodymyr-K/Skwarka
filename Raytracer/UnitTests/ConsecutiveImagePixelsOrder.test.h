#ifndef CONSECUTIVE_IMAGE_PIXELS_ORDER_TEST_H
#define CONSECUTIVE_IMAGE_PIXELS_ORDER_TEST_H

#include <cxxtest/TestSuite.h>
#include <Math/Point2D.h>
#include "CustomValueTraits.h"
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>

class ConsecutiveImagePixelsOrderTestSuite: public CxxTest::TestSuite
  {
  public:

    void test_ConsecutiveImagePixelsOrder_Constr()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(100,100));

      TS_ASSERT_EQUALS(pixels_order.GetTotalPixelsNum(), 100*100);
      }

    void test_ConsecutiveImagePixelsOrder_PixelsCount()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(100,100));

      size_t count=0;
      Point2D_i pixel;
      while(pixels_order.GetNextPixel(pixel))
        ++count;

      TS_ASSERT_EQUALS(count, 100*100);
      }

    void test_ConsecutiveImagePixelsOrder_Reset()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(100,100));

      Point2D_i pixel;
      pixels_order.GetNextPixel(pixel);
    
      // Reset the order.
      pixels_order.Reset();

      size_t count=0;
      while(pixels_order.GetNextPixel(pixel))
        ++count;

      TS_ASSERT_EQUALS(count, 100*100);
      }

  };

#endif // CONSECUTIVE_IMAGE_PIXELS_ORDER_TEST_H
