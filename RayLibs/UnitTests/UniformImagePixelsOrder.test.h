#ifndef UNIFORM_IMAGE_PIXELS_ORDER_TEST_H
#define UNIFORM_IMAGE_PIXELS_ORDER_TEST_H

#include <cxxtest/TestSuite.h>
#include <Math/Point2D.h>
#include "CustomValueTraits.h"
#include <Raytracer/Samplers/UniformImagePixelsOrder.h>
#include <vector>

class UniformImagePixelsOrderTestSuite: public CxxTest::TestSuite
  {
  public:

    void test_UniformImagePixelsOrder_Constr()
      {
      UniformImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(100,100));

      TS_ASSERT_EQUALS(pixels_order.GetTotalPixelsNum(), 100*100);
      }

    void test_UniformImagePixelsOrder_PixelsCount()
      {
      UniformImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(50,100));

      size_t count=0;
      Point2D_i pixel;
      while(pixels_order.GetNextPixel(pixel))
        ++count;

      TS_ASSERT_EQUALS(count, 50*100);
      }

    void test_UniformImagePixelsOrder_Reset()
      {
      UniformImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(50,100));

      Point2D_i pixel;
      pixels_order.GetNextPixel(pixel);

      // Reset the order.
      pixels_order.Reset();

      size_t count=0;
      while(pixels_order.GetNextPixel(pixel))
        ++count;

      TS_ASSERT_EQUALS(count, 50*100);
      }

    void test_UniformImagePixelsOrder_RangeAndCoverage()
      {
      bool correct=true;
      for(size_t x=0;x<=16;++x)
        for(size_t y=0;y<=16;++y)
          if (CheckRangeAndCoverage(x,y)==false)
            {
            TS_FAIL("Failed.");
            return;
            }
      }

  private:
    bool CheckRangeAndCoverage(int size_x, int size_y)
      {
      UniformImagePixelsOrder pixels_order;

      Point2D_i begin(-2,-1), size(size_x,size_y);
      pixels_order.SetImageSize(begin, begin+size);

      bool correct=true;
      size_t count=0;
      Point2D_i pixel;

      std::vector<std::vector<bool> > visited(size[0], std::vector<bool>(size[1],false));
      while(pixels_order.GetNextPixel(pixel))
        {
        if (pixel[0]<begin[0] || pixel[1]<begin[1] || pixel[0]>=begin[0]+size[0] || pixel[1]>=begin[1]+size[1])
          correct=false;

        if (visited[pixel[0]-begin[0]][pixel[1]-begin[1]])
          correct=false;
        else
          visited[pixel[0]-begin[0]][pixel[1]-begin[1]]=true;

        ++count;
        }

      return (count==size[0]*size[1] && correct);
      }

  };

#endif // UNIFORM_IMAGE_PIXELS_ORDER_TEST_H
