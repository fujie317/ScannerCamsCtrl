#ifndef PAGENUM_H
#define PAGENUM_H

#include <QtCore/qglobal.h>
#include <QString>

enum PageFormat
{
  ARABIC = 0,
  ROMAN_UPPER = 1,
  ROMAN_LOWER = 2
};

class PageNum
{
public:
  PageNum();
  QString toRoman();
  void setFormat(enum PageFormat format);
  enum PageFormat getFormat();
  void setValue(int value);
  int getValue();
  void setStrVal(QString strVal);
  QString getStrVal();
  PageNum operator++();
private:
  enum PageFormat format;
  int value;
  QString strVal;       // string form of value w/o padding
};

#endif // PAGENUM_H
