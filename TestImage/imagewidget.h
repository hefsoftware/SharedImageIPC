/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef IMAGEWIDGET_H
#define IMAGEWIDGET_H

#include <QWidget>

class ImageWidget : public QWidget
{
  Q_OBJECT
public:
  explicit ImageWidget(QWidget *parent = nullptr);
  QImage &image();

  void setImage(const QImage &newImage);

protected:
  QImage m_image;

  // QWidget interface
protected:
  void paintEvent(QPaintEvent *) override;
};

#endif // IMAGEWIDGET_H
