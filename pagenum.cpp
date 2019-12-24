#include "pagenum.h"
#include <QVariant>

PageNum::PageNum()
{
  this->format = PageFormat::ARABIC;
  this->value = 1;
  this->strVal = "1";
}

// Convert arabic number to Roman numeral
QString PageNum::toRoman()
{
  if(value <= 0)
    return "";
  int num[] = {1,4,5,9,10,40,50,90,100,400,500,900,1000};
  QString sym[] = {"I","IV","V","IX","X","XL","L","XC","C","CD","D","CM","M"};
  int i=12;
  QString rslt = "";
  while(value>0)
    {
      int div = value/num[i];
      value %= num[i];
      while(div--)
        {
          rslt += sym[i];
        }
      i--;
    }

  return rslt;
}

enum PageFormat PageNum::getFormat(){
  return this->format;
}

void PageNum::setFormat(enum PageFormat format){
  this->format = format;
}

int PageNum::getValue(){
  return this->value;
}

void PageNum::setValue(int value){
  this->value = value;
  this->strVal = QStringLiteral("%1").arg(value);
}

QString PageNum::getStrVal(){
  QString sv;
  if(format == ARABIC)
    sv = strVal;
  else if(format == ROMAN_UPPER)
    sv = toRoman ();
  else
    sv = toRoman ().toLower ();

  return sv;
}

void PageNum::setStrVal(QString strVal){
  this->strVal = strVal;
  this->value = strVal.toInt ();
}

PageNum PageNum::operator++(){
  ++(this->value);
  this->strVal = QStringLiteral("%1").arg(this->value);
  return *this;
}
