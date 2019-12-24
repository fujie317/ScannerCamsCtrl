#include "labelwithimage.h"

LabelWithImage::LabelWithImage(QWidget* parent, Qt::WindowFlags f)
  : QLabel(parent,f)
  , m_MouseButtonTime()
  , m_pixmapWidth(0)
  , m_pixmapHeight(0)
  , m_MagGlass( this )
{
}

LabelWithImage::LabelWithImage(const QString& text, QWidget* parent, Qt::WindowFlags f)
  : QLabel(text, parent,f)
  , m_MouseButtonTime()
  , m_pixmapWidth(0)
  , m_pixmapHeight(0)
  , m_MagGlass( this )
{
}

LabelWithImage::LabelWithImage(const QImage& image, int width, int height, QWidget* parent, Qt::WindowFlags f)
  : QLabel(parent,f)
  , m_MouseButtonTime()
  , m_pixmapWidth(0)
  , m_pixmapHeight(0)
  , m_MagGlass( this )
{
  this->setImage( image, width, height );
}

void LabelWithImage::setImage(const QImage& image,
                              int width, int height,
                              Qt::AspectRatioMode aspectMode,
                              Qt::TransformationMode mode )
{
  this->m_Image = image;
  this->setPixmap_internal( QPixmap::fromImage( m_Image ).scaled(width,height,aspectMode,mode));
}

void LabelWithImage::setPixmap(const QPixmap& pixmap)
{
  this->m_Image = pixmap.toImage();
  this->setPixmap_internal(pixmap);
}

QPoint LabelWithImage::getGlassPos(){
  return glassPos;
}

void LabelWithImage::setglassPos(QPoint pos){
  this->glassPos = pos;
}

void LabelWithImage::setPixmap_internal(const QPixmap& pixmap)
{
  m_pixmapWidth = pixmap.width();
  m_pixmapHeight = pixmap.height();

  updateMargins_internal();
  QLabel::setPixmap(pixmap);
}

void LabelWithImage::resizeEvent(QResizeEvent* event)
{
  // qDebug() << __PRETTY_FUNCTION__ << "ResizeEvent=" << event;
  if(m_MagGlass.isActive())
    m_MagGlass.setup( m_Image, event->size(), this->size());

  this->updateMargins_internal();
  QLabel::resizeEvent(event);

  setglassPos (m_MagGlass.getPos ());
}

void LabelWithImage::mousePressEvent(QMouseEvent* event)
{
  Qt::MouseButton Button = event->button();
  //Qt::MouseButtons Buttons = event->buttons();
  QPoint MouseXY = event->pos();

  int TimeBetween = m_MouseButtonTime.restart();
  bool DoubleClicked = (TimeBetween<500/*ms*/) ? true:false;

  // qDebug() << __PRETTY_FUNCTION__ << "Button" << Button << "/" << Buttons << "at" << MouseXY << ((DoubleClicked)? "double":"single") << "clicked within" << TimeBetween << "ms";
  if( Button == Qt::RightButton )
    {
      m_MagGlass.activate();
      m_MagGlass.setup( m_Image, m_Image.size(), this->size(), MouseXY );
      m_MagGlass.Show( event->pos() );

      if( DoubleClicked )
        m_MagGlass.resetZoom();
      return;
    }

  setglassPos (m_MagGlass.getPos ());
}

void LabelWithImage::mouseMoveEvent(QMouseEvent* event)
{
  // event->pos() reports the position of the mouse cursor, relative to this widget.
  // If you want to show a tooltip immediately, while the mouse is moving (e.g., to get the mouse coordinates with QMouseEvent::pos() and show them as a tooltip),
  // you must first enable mouse tracking as described above. Then, to ensure that the tooltip is updated immediately, you must call QToolTip::showText()
  // instead of setToolTip() in your implementation of mouseMoveEvent().
  //QPoint Where = event->pos();
  //Qt::MouseButton Button = event->button();
  // qDebug() << __PRETTY_FUNCTION__ << "(" << objectName() << ") Mouse with Button" << Button << "at" << Where << "moved";
  if(m_MagGlass.isActive())
    {
      m_MagGlass.Move( event->pos() );
    }
  setglassPos (m_MagGlass.getPos ());
}

void LabelWithImage::mouseReleaseEvent(QMouseEvent* event)
{
  if( event->button() == Qt::RightButton )
    {
    //  m_MagGlass.deactivate();
    //  m_MagGlass.Hide();
    }
}

void LabelWithImage::wheelEvent(QWheelEvent* event)
{
  m_MagGlass.reportZoom( event->delta() );
}

