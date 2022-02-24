/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "imageviewer.h"
#include "ui_imageviewer.h"
#include "hfsharedimage.h"
#include <QMdiSubWindow>
ImageViewer::ImageViewer(QWidget *parent) :
  QWidget(parent),
  ui(new Ui::ImageViewer), m_image(nullptr)
{
  ui->setupUi(this);
  connect(false);
  startTimer(25); // 25
}

ImageViewer::~ImageViewer()
{
  delete ui;
  delete m_image;
}

void ImageViewer::connect(bool connect)
{
  bool connected=false;
  if(connect)
  {
    if(m_image)
      connected=true;
    else
    {
      m_image=new HFSharedImage(false);
      ui->message->show();
      bool result=m_image->open(ui->id->text().toUtf8());
      if(!result)
      {
        ui->message->setText(tr("Failed to connect"));
        delete m_image;
        m_image=nullptr;
      }
      else
      {
        QObject::connect(m_image, &HFSharedImage::notify, this, &ImageViewer::onImageNotify);
        ui->message->clear();
      }
      connected=result;
    }
  }
  else
  {
    if(m_image)
    {
      ui->view->setImage(QImage());
      ui->view->update();
      delete m_image;
      m_image=nullptr;
    }
    connected=false;
    ui->message->hide();
  }
  ui->connect->setText(connected?tr("Disconnect"):tr("Connect"));
  ui->id->setEnabled(!connected);
  ui->connect->setChecked(connected);
  ui->view->setVisible(connected);
  QWidget *w=this;
  if(dynamic_cast<QMdiSubWindow *>(w->parentWidget()))
    w=w->parentWidget();
  auto sz=w->size(), minSz=w->minimumSizeHint();
  if(sz.width()<minSz.width() || sz.height()<minSz.height())
  {
    w->resize(qMax(sz.width(), minSz.width()), qMax(sz.height(), minSz.height()));
  }
}

void ImageViewer::on_connect_clicked(bool checked)
{
  connect(checked);
}



void ImageViewer::timerEvent(QTimerEvent *)
{
  if(!ui->eventDriven->isChecked())
    tryReceive();
  if(m_image)
  {
    double fps=m_image?m_image->fps():qQNaN();
    if(qIsNaN(fps))
      ui->message->clear();
    else
      ui->message->setText(tr("%1 fps").arg(fps, 0, 'f', 1));
  }
}

void ImageViewer::on_eventDriven_clicked(bool checked)
{
  if(checked)
    tryReceive(); // Maybe we discarded an event
}

void ImageViewer::onImageNotify()
{
  if(ui->eventDriven->isChecked())
    tryReceive();
}

void ImageViewer::tryReceive()
{
  QImage buffer;
  if(m_image && m_image->receiveImage(buffer))
  {
    ui->view->setImage(buffer);
    ui->view->update();
  }
}

