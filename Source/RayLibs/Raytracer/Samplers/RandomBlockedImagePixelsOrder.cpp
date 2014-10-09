#include "RandomBlockedImagePixelsOrder.h"
#include <Common/Common.h>
#include <Math/SamplingRoutines.h>

RandomBlockedImagePixelsOrder::RandomBlockedImagePixelsOrder(): ImagePixelsOrder()
  {
  m_next_pixel_index = 0;
  }

void RandomBlockedImagePixelsOrder::SetImageSize(const Point2D_i &i_image_begin, const Point2D_i &i_image_end)
  {
  ASSERT(i_image_end[0]>=i_image_begin[0]);
  ASSERT(i_image_end[1]>=i_image_begin[1]);

  m_image_begin=i_image_begin;
  m_image_end=i_image_end;

  // Reset the path bit mask and next pixel index.
  m_next_pixel_index=0;

  size_t size_x=std::max(0, m_image_end[0]-m_image_begin[0]);
  size_t size_y=std::max(0, m_image_end[1]-m_image_begin[1]);

  std::vector<Point2D_i> blocks;
  for(int x=m_image_begin[0]; x<m_image_end[0]; x+=BLOCK_SIZE)
    for(int y=m_image_begin[1]; y<m_image_end[1]; y+=BLOCK_SIZE)
      blocks.push_back(Point2D_i(x,y));

  SamplingRoutines::Shuffle(blocks.begin(), blocks.size());

  for(size_t i=0;i<blocks.size();++i)
    {
    int x=blocks[i][0], y=blocks[i][1];
    for(int dy=0;dy<BLOCK_SIZE;++dy)
      for(int dx=0;dx<BLOCK_SIZE;++dx)
        if (x+dx<m_image_end[0] && y+dy<m_image_end[1])
          m_pixels.push_back(Point2D_i(x+dx,y+dy));
    }
  }

size_t RandomBlockedImagePixelsOrder::GetTotalPixelsNum() const
  {
  size_t ret = (m_image_end[0]-m_image_begin[0])*(m_image_end[1]-m_image_begin[1]);
  ASSERT(m_pixels.size() == ret);

  return ret;
  }

void RandomBlockedImagePixelsOrder::Reset()
  {
  m_next_pixel_index=0;
  }

bool RandomBlockedImagePixelsOrder::GetNextPixel(Point2D_i &o_image_pixel)
  {
  if (m_next_pixel_index >= m_pixels.size())
    return false;

  o_image_pixel=m_pixels[m_next_pixel_index++];
  return true;
  }
