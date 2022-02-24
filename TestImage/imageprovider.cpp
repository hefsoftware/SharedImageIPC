/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "imageprovider.h"
#include "ui_imageprovider.h"
#include <QDateTime>
#include <QVariant>
#include "hfsharedimage.h"
#include <QPainter>
ImageProvider::ImageProvider(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ImageProvider), m_image(nullptr)
{
  m_changingRes=true;
  ui->setupUi(this);
  typedef QList<QVariant> L;
  ui->zoom->addItem(tr("1x"), 1);
  ui->zoom->addItem(tr("2x"), 2);
  ui->zoom->addItem(tr("4x"), 4);
  ui->zoom->addItem(tr("5x"), 5);
  ui->zoom->addItem(tr("10x"), 10);
  ui->zoom->addItem(tr("20x"), 20);
  ui->zoom->setCurrentIndex(3);
  ui->defaultRes->addItem(tr("640x480"), L({640, 480}));
  ui->defaultRes->addItem(tr("800x600"), L({800, 600}));
  ui->defaultRes->addItem(tr("1024x768"), L({1024, 768}));
  ui->defaultRes->addItem(tr("720p"), L({1280, 720}));
  ui->defaultRes->addItem(tr("1080p"), L({1920, 1080}));
  ui->defaultRes->addItem(tr("1440p"), L({2560, 1440}));
  ui->defaultRes->addItem(tr("4k"), L({3840, 2160}));
  m_changingRes=false;
  changedResSpin();
  connect(false);
  startTimer(200); // 200
}

ImageProvider::~ImageProvider()
{
  delete ui;
  delete m_image;
}

void ImageProvider::connect(bool connectValue)
{
  bool connected=false;
  if(connectValue)
  {
    if(m_image)
      connected=true;
    else
    {
      m_image=new HFSharedImage(true);
      connected=m_image->open(ui->id->text().toUtf8());
      if(connected)
      {
        ui->message->clear();
        QObject::connect(m_image, &HFSharedImage::notify, this, &ImageProvider::onImageNotify);
      }
      else
      {
        delete m_image;
        m_image=nullptr;
        ui->message->setText(tr("Failed to connect"));
      }
    }
  }
  else
  {
    if(m_image)
    {
      delete m_image;
      m_image=nullptr;
    }
    connected=false;
    ui->message->clear();
  }
  ui->connect->setText(connected?tr("Disconnect"):tr("Connect"));
  ui->id->setEnabled(!connected);
  ui->connect->setChecked(connected);
}

void ImageProvider::on_horRes_valueChanged(int)
{
  if(!m_changingRes)
    changedResSpin();
}


void ImageProvider::on_verRes_valueChanged(int)
{
  if(!m_changingRes)
    changedResSpin();
}

void ImageProvider::changedResSpin()
{
  m_changingRes=true;
  int customIndex=-1, selectedIndex=-1;
  for(int i=0;i<ui->defaultRes->count();i++)
  {
    auto v=ui->defaultRes->itemData(i);
    if(v.typeId()==QMetaType::QVariantList)
    {
      auto l=v.toList();
      if(l.size()!=2)
        continue;
      if(ui->horRes->value()==l[0].toInt() && ui->verRes->value()==l[1].toInt())
      {
        selectedIndex=i;
        break;
      }
    }
    else
      customIndex=i;
  }
  if(selectedIndex<0)
  {
    if(customIndex<0)
    {
      ui->defaultRes->insertItem(0, tr("Custom"));
      ui->defaultRes->setCurrentIndex(0);
    }
    else
      ui->defaultRes->setCurrentIndex(customIndex);
  }
  else
  {
    ui->defaultRes->setCurrentIndex(selectedIndex);
    if(customIndex>=0)
      ui->defaultRes->removeItem(customIndex);
  }
  ui->provider->setZoom(ui->zoom->currentData().toInt());
  ui->provider->setSize(ui->horRes->value(), ui->verRes->value());
  m_changingRes=false;
}


void ImageProvider::on_defaultRes_currentIndexChanged(int index)
{
  if(!m_changingRes && index>=0)
  {
    m_changingRes=true;
    auto v=ui->defaultRes->itemData(index);
    if(v.typeId()==QMetaType::QVariantList)
    {
      auto l=v.toList();
      if(l.size()==2)
      {
        ui->horRes->setValue(l[0].toInt());
        ui->verRes->setValue(l[1].toInt());
        for(int i=0;i<ui->defaultRes->count();i++)
        {
          if(ui->defaultRes->itemData(i).typeId()!=QMetaType::QVariantList)
          {
            ui->defaultRes->removeItem(i);
            break;
          }
        }
      }
    }
    ui->provider->setSize(ui->horRes->value(), ui->verRes->value());
    m_changingRes=false;
  }
}


void ImageProvider::on_zoom_currentIndexChanged(int)
{
  if(!m_changingRes)
  {
    m_changingRes=true;
    changedResSpin();
    m_changingRes=false;
  }
}


void ImageProvider::on_connect_clicked(bool checked)
{
  connect(checked);
}

void ImageProvider::onImageNotify()
{
  if(ui->eventDriven->isChecked())
    trySendImage();
}



void ImageProvider::timerEvent(QTimerEvent *)
{
  if(!ui->eventDriven->isChecked())
    trySendImage();
  if(m_image)
  {
    double fps=m_image?m_image->fps():qQNaN();
    if(qIsNaN(fps))
      ui->message->clear();
    else
      ui->message->setText(tr("%1 fps").arg(fps, 0, 'f', 1));
  }
}

void ImageProvider::trySendImage()
{
  if(m_image && m_image->sendImageStart())
  {
    uint32_t numPixels=m_image->sendImageNumPixels();

    auto &img=ui->provider->image();
    auto bytes=img.bytesPerLine()*img.height();
    if(bytes<=numPixels*sizeof(uint32_t))
    {
      auto buffer=m_image->sendImageBuffer(img.width(), img.height(), img.bytesPerLine());
      if(!buffer)
      {
        qDebug()<<"BUG!";
        buffer=m_image->sendImageBuffer(img.width(), img.height(), img.bytesPerLine());
        qDebug()<<"BUG!"<<buffer;
      }
      if(ui->dynamic->isChecked())
      {
        QPainter p(&ui->provider->imageData());
        double pos=(QDateTime::currentMSecsSinceEpoch()%1000)/1000.;
        auto vat=[pos](double p){return qAbs(1-fmod((2+p-pos)*2., 2.));};
        auto colat=[vat](double p){double v=vat(p); return QColor(v*255, 0, 255-255*v);};
        QLinearGradient linearGrad(QPointF(0, 0), QPointF(img.width(), 0));
        linearGrad.setColorAt(0, colat(0));
        linearGrad.setColorAt(pos, colat(pos));
        linearGrad.setColorAt(fmod(pos+0.5, 1), colat(pos+0.5));
        linearGrad.setColorAt(1, colat(1));
        p.setBrush(linearGrad);
        p.drawRect(0,0, img.width(), 50);
        p.end();
        ui->provider->update();
      }
      memcpy(buffer, img.bits(), bytes);
      m_image->sendEnd();
    }
  }
}

void ImageProvider::on_eventDriven_clicked(bool checked)
{
  if(checked)
    trySendImage(); // Maybe we discarded an event
}


void ImageProvider::on_clear_clicked()
{
  ui->provider->imageData().fill(Qt::transparent);
  ui->provider->update();
}

