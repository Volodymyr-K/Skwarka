#include "RenderCanvas.h"
#include <QPainter>
#include <QImage>

#include <vector>
#include <iostream>

RenderCanvas::RenderCanvas(QQuickItem *ip_parent)
    : QQuickPaintedItem(ip_parent), mp_image(NULL)
  {
  }

void RenderCanvas::setImageData(size_t i_width, size_t i_height, std::vector<unsigned char> &&i_data)
  {
  m_data = std::move(i_data);
  mp_image.reset( new QImage(&m_data[0], i_width, i_height, QImage::Format_ARGB32) );

  emit imageDataUpdated();
  }

void RenderCanvas::paint(QPainter *ip_painter)
  {
  std::shared_ptr<QImage> p_image = mp_image;

  if (p_image != NULL)
    ip_painter->drawImage(0, 0, *p_image);
  }
