/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#ifndef IMAGEPROVIDER_H
#define IMAGEPROVIDER_H

#include <QWidget>

namespace Ui {
  class ImageProvider;
}
class HFSharedImage;
class ImageProvider : public QWidget
{
  Q_OBJECT

public:
  explicit ImageProvider(QWidget *parent = nullptr);
  ~ImageProvider();

public slots:
  void connect(bool connectValue);
private slots:
  void on_horRes_valueChanged(int arg1);
  void on_verRes_valueChanged(int arg1);

  void on_defaultRes_currentIndexChanged(int index);

  void on_zoom_currentIndexChanged(int);
  void on_connect_clicked(bool checked);

  void onImageNotify();
  void on_eventDriven_clicked(bool checked);

  void on_clear_clicked();

private:
  void changedResSpin();
  bool m_changingRes;
  Ui::ImageProvider *ui;
protected:
  HFSharedImage *m_image;

  // QObject interface
protected:
  void timerEvent(QTimerEvent *) override;
  void trySendImage();
};

#endif // IMAGEPROVIDER_H
