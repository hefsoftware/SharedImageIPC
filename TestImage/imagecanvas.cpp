/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "imagecanvas.h"
#include <QPainter>
#include <QMouseEvent>
ImageCanvas::ImageCanvas(QWidget *parent)
  : QWidget{parent}, m_size(0, 0), m_zoom(1)
{
  m_image=QImage(1,1, QImage::Format_ARGB32);
  m_image.fill(Qt::transparent);
  setSize(QSize(100, 100));
}

QSize ImageCanvas::size() const
{
  return m_size;
}

void ImageCanvas::setSize(int width, int height)
{
  int wWidth=(width+m_zoom-1)/m_zoom;
  int wHeight=(height+m_zoom-1)/m_zoom;
  resize(wWidth, wHeight);
  m_size = QSize(width, height);
  m_image=m_image.scaled(width, height);
}

int ImageCanvas::zoom() const
{
  return m_zoom;
}

void ImageCanvas::setZoom(int newZoom)
{
  m_zoom = newZoom;
}

const QImage &ImageCanvas::image() const
{
  return m_image;
}


void ImageCanvas::paintEvent(QPaintEvent *)
{
  QPainter paint(this);
  QLinearGradient linearGrad(QPointF(0, 0), QPointF(width(), height()));
  linearGrad.setColorAt(0, QColor(128,128,128));
  linearGrad.setColorAt(1, QColor(192,192,192));
  QBrush b(linearGrad);
  paint.fillRect(0, 0, width(), height(), b);
  paint.drawImage(QRect(0, 0, width(), height()), m_image, QRect(0, 0, m_size.width(), m_size.height()));
}


void ImageCanvas::mousePressEvent(QMouseEvent *event)
{
  if(event->button()==Qt::LeftButton)
  {
    m_lastPoint=event->pos()*m_zoom;
    QPainter paint(&m_image);
    paint.setPen(QPen(Qt::black, 5));
    paint.drawPoint(m_lastPoint);
    update();
  }
}

void ImageCanvas::mouseMoveEvent(QMouseEvent *event)
{
  if(event->buttons()&Qt::LeftButton)
  {
    auto p=event->pos()*m_zoom;
    QPainter paint(&m_image);
    paint.setPen(QPen(Qt::black, 5));
    paint.drawLine(m_lastPoint, p);
    m_lastPoint=p;
    update();
  }
}
