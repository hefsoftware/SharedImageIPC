/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "imagewidget.h"
#include <QPainter>
#include <QDebug>

ImageWidget::ImageWidget(QWidget *parent)
  : QWidget{parent}
{

}

QImage &ImageWidget::image()
{
  return m_image;
}

void ImageWidget::setImage(const QImage &newImage)
{
  m_image = newImage;
}


void ImageWidget::paintEvent(QPaintEvent *)
{
  QPainter p(this);
  if(!m_image.isNull())
  {
    auto rW=width()*1./m_image.width();
    auto rH=height()*1./m_image.height();
    int w, h;
    if(rW<rH)
    {
      w=width();
      h=m_image.height()*w/m_image.width();
    }
    else
    {
      h=height();
      w=m_image.width()*h/m_image.height();
    }
    int x=(width()-w)/2, y=(height()-h)/2;
    p.drawRect(x, y, w-1, h-1);
    p.drawImage(QRect(x, y, w, h), m_image, QRect(0,0, m_image.width(), m_image.height()));
  }
}
