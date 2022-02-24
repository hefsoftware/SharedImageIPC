/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#pragma once

#include <QObject>

struct SharedImage;

class HFSharedImage : public QObject
{
  Q_OBJECT
public:
  explicit HFSharedImage(bool provider, QObject *parent = nullptr);
  ~HFSharedImage();
  bool open(const QString &id);
  quint32 wantedPixels() const;
  void setWantedPixels(quint32 newWantedPixels);
  bool sendImageStart();
  quint32 sendImageNumPixels();
  QImage sendImage(quint32 width, quint32 height);
  void *sendImageBuffer(quint32 width, quint32 height, quint32 bytesPerLine);
  bool sendEnd();
  bool receiveImage(QImage &buffer);
  void debug();
  double fps();
signals:
  void notify();
protected:
  SharedImage *m_image;
  void setNotifyHandle();
  void clearNotifyHandle();
  void clearSend();
  void frame();
  void *m_sendImageData;
  quint32 m_sendImagePixels;
  quint32 m_wantedPixels;
  quint32 m_sendWidth;
  quint32 m_sendHeight;
  quint32 m_sendBytesPerLine;

  qint64 m_startTime;
  qint64 m_lastTime;
  double m_fps;
  int m_numFrames;

  bool m_provider;
  void *m_notifyHandle;
signals:

};
