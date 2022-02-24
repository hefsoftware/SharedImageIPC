/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef IMAGEVIEWER_H
#define IMAGEVIEWER_H

#include <QWidget>

namespace Ui {
  class ImageViewer;
}
class HFSharedImage;

class ImageViewer : public QWidget
{
  Q_OBJECT

public:
  explicit ImageViewer(QWidget *parent = nullptr);
  ~ImageViewer();
  void connect(bool connect);
private:
  Ui::ImageViewer *ui;
protected:
  HFSharedImage *m_image;
private slots:
  void on_connect_clicked(bool checked);

  // QObject interface
  void on_eventDriven_clicked(bool checked);
  void onImageNotify();

protected:
  void tryReceive();
  void timerEvent(QTimerEvent *) override;
};

#endif // IMAGEVIEWER_H
