#ifndef LABELWITHIMAGE_H
#define LABELWITHIMAGE_H

#include <QtCore/qglobal.h>
#include <QLabel>
#include <QImage>
#include <QPixmap>
#include <QTime>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QWheelEvent>

#include "MagnificationGlass.h"

class LabelWithImage : public QLabel
{
  Q_OBJECT

public:
  explicit LabelWithImage( QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags() );
  explicit LabelWithImage( const QString &text, QWidget *parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags() );
  explicit LabelWithImage( const QImage &image, int width, int height, QWidget* parent=nullptr, Qt::WindowFlags f=Qt::WindowFlags() );
  virtual ~LabelWithImage() {}

  void setImage( const QImage& image, int width, int height, Qt::AspectRatioMode aspectMode = Qt::KeepAspectRatio, Qt::TransformationMode mode = Qt::SmoothTransformation );
  void setPixmap(const QPixmap& pixmap );
  QPoint getGlassPos();
  void setglassPos(QPoint pos);
  MagnificationGlass  m_MagGlass;

protected:
  void resizeEvent(       QResizeEvent *event );
  void mousePressEvent(   QMouseEvent  *event );
  void mouseMoveEvent(    QMouseEvent  *event );
  void mouseReleaseEvent( QMouseEvent  *event );
  void wheelEvent(        QWheelEvent  *event );

private:
  void setPixmap_internal( const QPixmap& pixmap );
  void updateMargins_internal(void);

private:
  QTime               m_MouseButtonTime;
  QImage              m_Image;
  int                 m_pixmapWidth;
  int                 m_pixmapHeight;
  /* ------ This is what the demo is about: ------*/
  QPoint glassPos;
};

#endif // LABELWITHIMAGE_H
