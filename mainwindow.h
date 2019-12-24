#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtCore/qglobal.h>
#if QT_VERSION >= 0x050000
#include <QtWidgets/QMainWindow>
#else
#include <QtGui/QMainWindow>
#endif

#include <QFileDialog>
#include <QKeyEvent>
#include "pagenum.h"
#include "mythread.h"
#include "labelwithimage.h"
#include "mygphoto2.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();
  void setPadChar(QChar padChar);
  QChar getPadChar();
  void setLen(int len);
  int getLen();
  int getSerial();
  void setSerial(int serial);
  QString toPaddedString(int pageNo);
  QPoint getGlsLPos();
  void setGlsLPos(QPoint pos);
  QPoint getGlsRPos();
  void setGlsRPos(QPoint pos);

  MyThread *mThread;
protected:
  void resetSerial();

public slots:
  void on_previewChanged();

private slots:
  void infoUpdate();

  void on_actionOpen_triggered();

  void on_actionSwitch_Cameras_triggered();

  void on_scanBtn_clicked();

  void on_actionArabic_triggered();

  void on_actionRoman_Upper_triggered();

  void on_actionRoman_Lower_triggered();

  void on_actionPadding_Character_triggered();

  void on_actionPadding_Length_triggered();

  void on_actionFile_Name_Prefix_triggered();

  void on_actionFile_Name_Suffix_triggered();

  void on_actionSync_triggered();

  void on_PageNoL_returnPressed();

  void on_PageNoR_returnPressed();

private:
  Ui::MainWindow *ui;

  LabelWithImage *camLimg, *camRimg;

  QString dirName;      // directory to store scan images
  int serial;           // serial page number for all pages
  int base;             // starting number for a session after reset
  int count;            // serial number within a session
  bool showPN;          // toggle to show page numbers
  PageNum pgL;          // page number structure for left camera
  PageNum pgR;          // page number structure for right camera
  QChar padChar;        // prefix padding character
  int len;              // total length of string format including padding
  QString prefix;       // prefix for output file name
  QString suffix;       // extension of output file name
  QPoint glsLPos;       // position of magnifying glass in widget
  QPoint glsRPos;       // ditto
};
#endif // MAINWINDOW_H
