/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef IMAGECANVAS_H
#define IMAGECANVAS_H

#include <QWidget>

class ImageCanvas : public QWidget
{
  Q_OBJECT
public:
  explicit ImageCanvas(QWidget *parent = nullptr);
  QSize size() const;
  inline void setSize(const QSize &newSize) { setSize(newSize.width(), newSize.height()); }
  void setSize(int width, int height);
  int zoom() const;
  void setZoom(int newZoom);

  const QImage &image() const;
  QImage &imageData() { return m_image; }

protected:
  QImage m_image;
  QSize m_size;
  int m_zoom;
  QPoint m_lastPoint;
signals:

  // QWidget interface
protected:
  void paintEvent(QPaintEvent *) override;

  // QWidget interface
protected:
  void mousePressEvent(QMouseEvent *event) override;
  void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // IMAGECANVAS_H
