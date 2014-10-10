/*
* Copyright (C) 2014 by Volodymyr Kachurovskyi <Volodymyr.Kachurovskyi@gmail.com>
*
* This file is part of Skwarka.
*
* Skwarka is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
*
* Skwarka is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License along with Skwarka.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONSECUTIVE_IMAGE_PIXELS_ORDER_TEST_H
#define CONSECUTIVE_IMAGE_PIXELS_ORDER_TEST_H

#include <cxxtest/TestSuite.h>
#include <Math/Point2D.h>
#include <UnitTests/TestHelpers/CustomValueTraits.h>
#include <Raytracer/Samplers/ConsecutiveImagePixelsOrder.h>

class ConsecutiveImagePixelsOrderTestSuite: public CxxTest::TestSuite
  {
  public:

    void test_ConsecutiveImagePixelsOrder_Constr()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(50,100));

      TS_ASSERT_EQUALS(pixels_order.GetTotalPixelsNum(), 50*100);
      }

    void test_ConsecutiveImagePixelsOrder_Order()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(-5,-5),Point2D_i(-5+100,-5+100));

      bool correct_order=true;
      size_t count=0;
      Point2D_i pixel;
      while(pixels_order.GetNextPixel(pixel))
        {
        if (pixel != Point2D_i(count%100,count/100)+Point2D_i(-5,-5))
          correct_order=false;
        ++count;
        }

      TS_ASSERT(correct_order);
      }

    void test_ConsecutiveImagePixelsOrder_ZeroCase()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(0,0));

      size_t count=0;
      Point2D_i pixel;
      while(pixels_order.GetNextPixel(pixel))
        ++count;

      TS_ASSERT_EQUALS(count, 0);
      }

    void test_ConsecutiveImagePixelsOrder_PixelsCount()
      {
      ConsecutiveImagePixelsOrder pixels_order;
      pixels_order.SetImageSize(Point2D_i(0,0),Point2D_i(50,100));

      size_t count=0;
      Point2D_i pixel;
      while(pixels_order.GetNextPixel(pixel))
        ++count;

      TS_ASSERT_EQUALS(count, 50*100);
      }

    void test_ConsecutiveImagePixelsOrder_Reset()
      {
      ConsecutiveImagePixelsOrder pixels_order;
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

  };

#endif // CONSECUTIVE_IMAGE_PIXELS_ORDER_TEST_H
