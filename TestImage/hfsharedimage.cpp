/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "hfsharedimage.h"
#include "sharedimage.h"
#include <QDateTime>
#include <QWinEventNotifier>
#include <QImage>
#include <QDebug>
HFSharedImage::HFSharedImage(bool provider, QObject *parent)
  : QObject{parent}, m_image(nullptr), m_wantedPixels(3000*2000), m_provider(provider), m_notifyHandle(nullptr)
{
  m_startTime=m_lastTime=-1;
  m_numFrames=0;
  m_fps=qQNaN();
  clearSend();
}

HFSharedImage::~HFSharedImage()
{
  clearNotifyHandle();
  if(m_image)
    sharedImageDestroy(m_image);
}

bool HFSharedImage::open(const QString &id)
{
  bool ret;
  if(m_image)
  {
    clearNotifyHandle();
    sharedImageDestroy(m_image);
    m_image=nullptr;
  }
  ret=sharedImageCreate(id.toUtf8(), &m_image, m_wantedPixels, m_provider);
  if(ret)
    setNotifyHandle();
  return ret;
}

quint32 HFSharedImage::wantedPixels() const
{
  return m_wantedPixels;
}

void HFSharedImage::setWantedPixels(quint32 newWantedPixels)
{
  m_wantedPixels = newWantedPixels;
}

bool HFSharedImage::sendImageStart()
{
  bool ret=false;
  clearSend();
  if(m_image)
  {
    ret=sharedImageOutBuffer(m_image, &m_sendImageData, &m_sendImagePixels);
    if(ret)
      frame();
  }
  return ret;
}

quint32 HFSharedImage::sendImageNumPixels()
{
  return m_sendImagePixels;
}

QImage HFSharedImage::sendImage(quint32 width, quint32 height)
{
  QImage ret;
  if(m_sendImageData && width && height && (!m_sendWidth || m_sendWidth==width) && (!m_sendHeight || m_sendHeight==height) && width*height<=m_sendImagePixels)
  {
    m_sendWidth=width;
    m_sendHeight=height;
    m_sendBytesPerLine=width*sizeof(quint32);
    ret=QImage((uchar *)m_sendImageData, width, height, m_sendBytesPerLine, QImage::Format_ARGB32);
  }
  return ret;
}

void *HFSharedImage::sendImageBuffer(quint32 width, quint32 height, quint32 bytesPerLine)
{
  void *ret=nullptr;
  if(m_sendImageData && width && height && (!m_sendWidth || m_sendWidth==width) && (!m_sendHeight || m_sendHeight==height) && (!m_sendBytesPerLine || m_sendBytesPerLine==bytesPerLine) && bytesPerLine*height<=m_sendImagePixels*sizeof(quint32) && bytesPerLine>=width*sizeof(quint32))
  {
    m_sendWidth=width;
    m_sendHeight=height;
    m_sendBytesPerLine=bytesPerLine;
    ret=m_sendImageData;
  }
  return ret;
}

bool HFSharedImage::sendEnd()
{
  bool ret=false;
  if(m_image)
  {
    SharedImageSetting setting;
    setting.width=m_sendWidth;
    setting.height=m_sendHeight;
    setting.bytesPerLine=m_sendBytesPerLine;
    ret=sharedImageSend(m_image, &setting);
  }
  m_sendWidth=m_sendHeight=m_sendBytesPerLine=m_sendImagePixels=0;
  m_sendImageData=nullptr;
  return ret;
}

bool HFSharedImage::receiveImage(QImage &buffer)
{
  bool ret=false;
  if(m_image)
  {
    void *data;
    const SharedImageSetting *setting;
    ret=sharedImageReceive(m_image, &data, &setting);
    if(ret)
    {
      frame();
      buffer=QImage((const uchar *)data, setting->width, setting->height, setting->bytesPerLine, QImage::Format_ARGB32);
    }
  }
  return ret;
}

void HFSharedImage::debug()
{
//  qDebug()<<m_image;
  //  sharedImageOutBuffer()
}

double HFSharedImage::fps()
{
  if(m_numFrames==0)
    m_fps=qQNaN();
  else
  {
    qint64 t=QDateTime::currentMSecsSinceEpoch();
    if(t-m_startTime>1000 || m_numFrames>10)
    {
      if(m_numFrames==1)
      {
        m_fps=1000./(t-m_startTime);
        if(t-m_startTime>5000)
          m_numFrames=0;
      }
      else
      {
        m_fps=(m_numFrames-1)*1000./(m_lastTime-m_startTime);
        m_startTime=m_lastTime;
        m_numFrames=1;
      }
    }
  }
  return m_fps;
}

void HFSharedImage::setNotifyHandle()
{
  if(m_image)
  {
#if defined(Q_OS_WIN32)
    auto handle=new QWinEventNotifier(sharedImageNotificationHandle(m_image));
    connect(handle, &QWinEventNotifier::activated, this, &HFSharedImage::notify);
    m_notifyHandle=handle;
#endif
  }
}

void HFSharedImage::clearNotifyHandle()
{
  if(m_notifyHandle)
  {
    #if defined(Q_OS_WIN32)
      delete (QWinEventNotifier *)m_notifyHandle;
    #else
      #error "Unhandled OS notification handle"
    #endif
  }
}

void HFSharedImage::clearSend()
{
  m_sendImageData=nullptr;
  m_sendImagePixels=m_sendWidth=m_sendHeight=m_sendBytesPerLine=0;
}

void HFSharedImage::frame()
{
  qint64 t=QDateTime::currentMSecsSinceEpoch();
  if(m_numFrames==0)
    m_startTime=t;
  m_lastTime=t;
  m_numFrames++;
//  qDebug()<<"Frame"<<this<<m_startTime-t<<m_numFrames;
}
