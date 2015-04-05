#ifndef RENDER_CANVAS_H
#define RENDER_CANVAS_H

#include <vector>
#include <memory>

#include <QtQuick/QQuickPaintedItem>
#include <QColor>

class RenderCanvas : public QQuickPaintedItem
{
    Q_OBJECT

public:
    RenderCanvas(QQuickItem *ip_parent = 0);

    void paint(QPainter *ip_painter);

    void setImageData(size_t i_width, size_t i_height, std::vector<unsigned char> &&i_data);

signals:
    void imageDataUpdated();

private:
    std::vector<unsigned char> m_data;
    std::shared_ptr<QImage> mp_image;
};

#endif // RENDER_CANVAS_H

