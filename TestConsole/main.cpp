/*
 * This file is part of SharedImageIPC.
 *
 * (c) Marzocchi Alessandro
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include <QCoreApplication>
#include "sharedmem.h"
#include <QThread>
#include <QDebug>
#include "windows.h"
#include <QTime>
#include <QCoreApplication>
void thread(bool server);

void delay( int ms )
{
    QTime endtime = QTime::currentTime().addMSecs( ms );
    do{
      QCoreApplication::processEvents( QEventLoop::AllEvents, 100 );
    }
    while( QTime::currentTime() < endtime );
}

int main(int argc, char *argv[])
{
  auto t1=QThread::create(thread, 0);
  auto t2=QThread::create(thread, 1);
  t2->start();
  t1->start();
  t1->wait();
  t2->wait();
  qDebug()<<"Main program done";
}

void thread(bool server)
{
  SharedMemInfo info {0, server?400u:300u, 0, server?50u:70u, 0, 640u*480u*4u, server?5u:6u};
  SharedMemory *shared=nullptr;
  int n;
  n=sharedMemCreate("TestName", &info, &shared, 0, server);
  const SharedMemInfo *infoReal=sharedMemInfo(shared);
  if(infoReal)
  {
    printf("%d] call succesfull %d %d\n", server, sharedMemMustInitialize(shared), sharedMemIsInitialized(shared));
    fflush(stdout);
    printf("%d] header: %d numPages: %d pageHeader: %d page: %d\n", server, info.headerSize, info.numPages, info.pageHeaderSize, info.pageSize);
    fflush(stdout);

    if(sharedMemMustInitialize(shared))
    {
      printf("%d] Loooong initialization started...\n", server); fflush(stdout);
      delay(7000);
      printf("%d] Initialization ended...\n", server); fflush(stdout);
      sharedMemEndInitialization(shared);
    }
    else if(!sharedMemIsInitialized(shared))
    {
      do
      {
        printf("%d] Waiting for shared object to be initialized...\n", server); fflush(stdout);
        sharedMemWaitNotify(shared, 15000);
      } while(!sharedMemIsInitialized(shared));
      printf("%d] Initialization now complete...\n", server); fflush(stdout);
    }
    if(server)
    {
      int32_t free=sharedMemGetFreePage(shared, 0);
      if(free>=0)
      {
        printf("%d] Got free page: %d\n", server, free); fflush(stdout);
        strcpy((char *)sharedMemPageData(shared, free), "Hello, world!");
        delay(2500);
        sharedMemSendData(shared, free);
        printf("%d] Forward free page: %d\n", server, free); fflush(stdout);
      }
    }
    else
    {
      int32_t data;
      while((data=sharedMemGetDataPage(shared, 0))<0) {
        printf("%d] Waiting data page\n", server); fflush(stdout);
        sharedMemWaitNotify(shared, 500);
      };
      printf("%d] Got a data page: %d %s\n", server, data, (char *)sharedMemPageData(shared, data)); fflush(stdout);
    }
  }
  else
    qDebug()<<"Failed"<<n;
}
