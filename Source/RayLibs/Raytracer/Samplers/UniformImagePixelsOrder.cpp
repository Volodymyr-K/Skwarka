#include "UniformImagePixelsOrder.h"
#include <Common/Common.h>

UniformImagePixelsOrder::UniformImagePixelsOrder(): ImagePixelsOrder()
  {
  }

void UniformImagePixelsOrder::SetImageSize(const Point2D_i &i_image_begin, const Point2D_i &i_image_end)
  {
  ASSERT(i_image_end[0]>=i_image_begin[0]);
  ASSERT(i_image_end[1]>=i_image_begin[1]);

  m_image_begin=i_image_begin;
  m_image_end=i_image_end;

  // Reset the path bit mask and next pixel index.
  m_path_bitmask=0;
  m_next_pixel_index=0;

  m_size_x=std::max(0,m_image_end[0]-m_image_begin[0]);
  m_size_y=std::max(0,m_image_end[1]-m_image_begin[1]);
  }

size_t UniformImagePixelsOrder::GetTotalPixelsNum() const
  {
  return m_size_x*m_size_y;
  }

void UniformImagePixelsOrder::Reset()
  {
  m_path_bitmask=0;
  m_next_pixel_index=0;
  }

bool UniformImagePixelsOrder::GetNextPixel(Point2D_i &o_image_pixel)
  {
  if (m_next_pixel_index++>=m_size_x*m_size_y)
    return false;

  // The outer loop is used to iterate through the image pixels until we find a pixel that has not been visited yet.
  while(true)
    {
    Point2D_i lower=Point2D_i(0,0), upper=Point2D_i(m_size_x,m_size_y);
    bool already_visited=false;

    // The loop through the bits of the unsigned integer value.
    for(unsigned char i=0;i<32;++i)
      {
      size_t local_size_x=upper[0]-lower[0], local_size_y=upper[1]-lower[1];

      // If we reached a leaf we break from the loop.
      if (local_size_x==1 && local_size_y==1)
        {
        // It is possible that we visited this leaf before.
        // It can happen if m_path_bitmask has more bits set.
        if (m_path_bitmask>>i) already_visited=true;

        break;
        }

      // We subdivide the axis that has larger extent.
      if (local_size_x>=local_size_y)
        {
        int mid_x=(lower[0]+upper[0])/2;
        if (m_path_bitmask & (1<<i)) upper[0]=mid_x; else lower[0]=mid_x;
        }
      else
        {
        int mid_y=(lower[1]+upper[1])/2;
        if (m_path_bitmask & (1<<i)) upper[1]=mid_y; else lower[1]=mid_y;
        }
      }

    // Move the path bit mask to the next pixel.
    ++m_path_bitmask;

    if (already_visited==false)
      {
      o_image_pixel = m_image_begin+lower;
      return true;
      }
    }

  ASSERT(0 && "This code should never be executed, the method should return eariler."); 
  return false;
  }