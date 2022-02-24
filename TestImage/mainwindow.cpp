/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imageprovider.h"
#include "imageviewer.h"
#include "hfsharedimage.h"
#include <QDebug>
MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent)
  , ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ImageProvider *prov=new ImageProvider;
  ui->mdi->addSubWindow(prov);
  prov->show();

  ImageViewer *view=new ImageViewer;
  ui->mdi->addSubWindow(view);
  view->show();
  // 640x480 3000fps
  // 800x600 2400fps
  // 1024x768 1600fps
}

MainWindow::~MainWindow()
{
  delete ui;
}

