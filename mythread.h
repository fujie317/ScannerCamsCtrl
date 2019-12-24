#ifndef MYTHREAD_H
#define MYTHREAD_H

#include <QtCore/qglobal.h>
#include <QThread>
#include <QPixmap>
#include <QImage>
#include <QMutex>

#include "mygphoto2.h"

class MyThread : public QThread
{
  Q_OBJECT
public:
  explicit MyThread(QObject *parent = nullptr);
  void run();
  QPixmap previewPixmapL, previewPixmapR;
  int count;
  GPContext *context;
  Camera **cams;

  QMutex mutex;

  QString imgSize;
  QString imgAspectRatio;
  QString imgQuality;
  QString isoSpeed;
  QString tmp;

protected:
  QString formatImgSize(QString sz,     // image size string read from camera
                        QString ar      // image size aspect ratio read from camera
                        );

signals:
  // To communicate with Gui Thread
  // we need to emit a signal
  void previewChanged();
  void initialized();

public slots:
};

#endif // MYTHREAD_H
