#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QRegExp>
#include <QInputDialog>
#include <QElapsedTimer>

#include <QMessageBox>
#include <QDebug>
#include <iostream>

#include <QTextStream>

// constructor of main window
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // set up two customized labels to display camera preview with a magnifying glass
    // for zone enlargement
    camLimg = new LabelWithImage();
    camLimg->setScaledContents (true);
    camLimg->setAlignment (Qt::AlignCenter | Qt::AlignHCenter);
    camLimg->setMinimumSize (480, 640);
    camLimg->setMaximumSize (960, 1280);

    camRimg = new LabelWithImage();
    camRimg->setScaledContents (true);
    camRimg->setAlignment (Qt::AlignCenter | Qt::AlignHCenter);
    camRimg->setMinimumSize (480, 640);
    camRimg->setMaximumSize (960, 1280);

    // add two labels displaying camera preview with magnifying glasses
    ui->camPreviews->addWidget (camLimg, 98, Qt::AlignCenter | Qt::AlignHCenter);
    ui->camPreviews->addWidget (camRimg, 98, Qt::AlignCenter | Qt::AlignHCenter);
    // setGlsLPos (QPoint(this->size ().width () / 2 - 20, this->size ().height () /2 - 20));
    // setGlsRPos (QPoint(this->size ().width () - 20, this->size ().height () - 20));

    dirName = QDir::currentPath();
    serial = 1;
    padChar = '0';
    prefix = "";
    suffix = "jpg";
    len = 8;

    resetSerial ();

    showPN = false;       // Do not show page numbers before starting number is set

    mThread = new MyThread(this);
    connect(mThread, SIGNAL(previewChanged()), this, SLOT(on_previewChanged()));
    mThread->start ();

    connect(mThread, SIGNAL (initialized()), this, SLOT(infoUpdate()));

}

// destructor of main window
MainWindow::~MainWindow()
{
    mThread->quit ();
    delete mThread;
    // delete ui->camL;
    // delete ui->camR;
    delete ui;
}

// getter of overall serial number of scanned pages
int MainWindow::getSerial(){
    return this->serial;
}

void MainWindow::infoUpdate (){
    QString status = "Leaf: %1 \t Capture Time: %2s \t Image Size: %3 pixels\t Aspect Ratio: %4 \t Note: %5";
    ui->statusbar->showMessage (status.arg(serial).arg (0.0).arg(mThread->imgSize).arg(mThread->imgAspectRatio).arg("None"));
    update ();
}

// setter of overall serial number of scanned pages
void MainWindow::setSerial(int serial){
    this->serial = serial;
}

// getter of length of base name (excluding prefix) of output image files
int MainWindow::getLen(){
    return this->len;
}

// setter of length of base name (excluding prefix) of output image files
void MainWindow::setLen(int len){
    this->len = len;
}

// getter of padding character of output image files
QChar MainWindow::getPadChar(){
    return this->padChar;
}

// setter for padding character of output image files
void MainWindow::setPadChar(QChar padChar){
    this->padChar = padChar;
}

// produce base file name (excluding prefix) of output image files
QString MainWindow::toPaddedString(int pageNo){
    return QString("%1").arg(pageNo, len, 10, padChar).append(".").append (suffix);
}

// get magnifying glass position for preview from left camera
QPoint MainWindow::getGlsLPos(){
    return this->glsLPos;
}

// set magnifying glass position for preview from left camera
void MainWindow::setGlsLPos(QPoint pos){
    this->glsLPos = pos;
}

// get magnifying glass position for preview from right camera
QPoint MainWindow::getGlsRPos(){
    return this->glsRPos;
}

// set magnifying glass position for preview from right camera
void MainWindow::setGlsRPos(QPoint pos){
    this->glsRPos = pos;
}

// slot to receive camera preview change signals (update onscreen previews)
void MainWindow::on_previewChanged(){
    MagnificationGlass *glsptr;
    QPoint glsPos;
    if(mThread->count > 0){
        camLimg->setImage (mThread->previewPixmapL.toImage ()
                           , mThread->previewPixmapL.width ()
                           , mThread->previewPixmapL.height ()
                           ,Qt::KeepAspectRatio, Qt::FastTransformation);
        glsptr = &(camLimg->m_MagGlass);

        if(glsptr->isActive ()){
            setGlsLPos (glsptr->getPos ());
            glsPos = getGlsLPos ();
            //  QTextStream out(stderr);
            //  out << getGlsLPos ().x ()<< " " << getGlsLPos ().y() <<"\n";
            glsptr->Hide ();
            glsptr->setup (mThread->previewPixmapL.toImage ()
                           , mThread->previewPixmapL.toImage().size ()
                           , camLimg->size (), glsPos);
            glsptr->Show (glsPos);

        }
    }
    if(mThread->count > 1){
        camRimg->setImage (mThread->previewPixmapR.toImage ()
                           , mThread->previewPixmapR.width ()
                           , mThread->previewPixmapR.height ()
                           ,Qt::KeepAspectRatio, Qt::FastTransformation);
        glsptr = &(camRimg->m_MagGlass);

        if(glsptr->isActive ()){
            setGlsRPos (glsptr->getPos ());
            glsPos = getGlsRPos ();
            glsptr->Hide ();
            glsptr->setup (mThread->previewPixmapR.toImage ()
                           , mThread->previewPixmapR.toImage().size ()
                           , camRimg->size (), glsPos);
            glsptr->Show (glsPos);
        }
    }

    update ();
}

// specify directory for storing output image files
void MainWindow::on_actionOpen_triggered()
{
    QString dir = QFileDialog::getExistingDirectory (this, tr("Open Directory"),
                                                     qApp->applicationDirPath(),
                                                     QFileDialog::ShowDirsOnly |
                                                     QFileDialog::DontResolveSymlinks);
    if(dir != nullptr && dir.compare (""))
        dirName = dir;

    resetSerial ();
    pgR.setValue (base);
    pgL.setValue (base + 1);

    ui->PageNoR->setText (pgR.getStrVal ());
    ui->PageNoL->setText (pgL.getStrVal ());
    update ();
}

// swap camera preview display onscreen positions (left/right)
void MainWindow::on_actionSwitch_Cameras_triggered()
{
    Camera *tmp;
    mThread->mutex.lock ();
    if(mThread->count > 1){
        tmp = mThread->cams[0];
        mThread->cams[0] = mThread->cams[1];
        mThread->cams[1] = tmp;
    }
    mThread->mutex.unlock ();
}

// action to trigger cameras' shutters
void MainWindow::on_scanBtn_clicked()
{
    mThread->mutex.lock ();
    qint64 time = 0;
    QElapsedTimer timer;
    timer.start ();
    for (int i = 0; i < mThread->count; i++) {
        capture((dirName + "/" + prefix + toPaddedString (serial++)).toStdString ().c_str (),
                mThread->cams[i], mThread->context);
        if(i){
            pgL.setValue (base + count++);
            ui->PageNoL->setText (pgL.getStrVal ());
        }
        else{
            pgR.setValue (base + count++);
            ui->PageNoR->setText (pgR.getStrVal ());
        }
    }
    time += timer.elapsed ();
    QString status = "Leaf: %1 \t Capture Time: %2s \t Image Size: %3 pixels\t Aspect Ratio: %4 \t Note: %5";
    ui->statusbar->showMessage (status.arg(serial).arg (time/2000.0).arg(mThread->imgSize).arg(mThread->imgAspectRatio).arg("None"));
    update ();
    mThread->mutex.unlock ();
}

// update widget sizes after window resizing
void LabelWithImage::updateMargins_internal(void)
{
    if( (m_pixmapWidth>0) && (m_pixmapHeight>0) )
    {
        int current_w = this->width();
        int current_h = this->height();

        if( (current_w>0) && (current_h>0) )
        {
            if( (current_w * m_pixmapHeight) > (current_h * m_pixmapWidth) )
            {
                int mix = (current_w - (m_pixmapWidth * current_h / m_pixmapHeight)) / 2;
                setContentsMargins( mix, 0, mix, 0 );
            }
            else
            {
                int miy = (current_h - (m_pixmapHeight * current_w / m_pixmapWidth)) / 2;
                setContentsMargins( 0, miy, 0, miy );
            }
        } // if (current_w > 0 && current_h > 0)
    } // if( m_pixmapWidth > 0 && m_pixmapHeight <= 0 )
}

// set format of display page number to arabic
void MainWindow::on_actionArabic_triggered()
{
    pgL.setFormat (PageFormat::ARABIC);
    pgR.setFormat (PageFormat::ARABIC);
    ui->PageNoL->setText (pgL.getStrVal ());
    update ();
}

// set format of display page number to upper Roman
void MainWindow::on_actionRoman_Upper_triggered()
{
    pgL.setFormat (PageFormat::ROMAN_UPPER);
    pgR.setFormat (PageFormat::ROMAN_UPPER);
    ui->PageNoL->setText (pgL.toRoman ());
    update ();
}

// set format of display page numbers to lower case Roman
void MainWindow::on_actionRoman_Lower_triggered()
{
    pgL.setFormat (PageFormat::ROMAN_LOWER);
    pgR.setFormat (PageFormat::ROMAN_LOWER);
    ui->PageNoL->setText (pgL.toRoman ().toLower ());
    update ();
}

// change padding character of output image file names
void MainWindow::on_actionPadding_Character_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Output File Name Padding Character:"), QLineEdit::Normal,
                                         QDir::home().dirName(), &ok);
    if(ok)
        padChar = text[0];
}

// change total length of base name (excluding prefix) of output image file names
void MainWindow::on_actionPadding_Length_triggered()
{
    bool ok;
    int len = QInputDialog::getInt(this, tr("QInputDialog::getInt()"),
                                   tr("Output File Name Base Length:"),
                                   8, 1, 32, 1, &ok);
    if(ok)
        this->len = len;
}

// change prefix of output image file names
void MainWindow::on_actionFile_Name_Prefix_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Output File Name Prefix Text:"), QLineEdit::Normal,
                                         "0", &ok);
    if(ok)
        prefix = text;
}

// change extension of output image file names
void MainWindow::on_actionFile_Name_Suffix_triggered()
{
    bool ok;
    QString text = QInputDialog::getText(this,
                                         tr("QInputDialog::getText()"),
                                         tr("Output File Name Extension (w/o leading dot):"),
                                         QLineEdit::Normal,
                                         "jpg", &ok);
    if(ok)
        suffix = text;
}

// sync page number formats of two facing pages
void MainWindow::on_actionSync_triggered()
{
    ui->PageNoR->setText (pgR.getStrVal ());

    update ();
}

// reset left camera start page number
void MainWindow::on_PageNoL_returnPressed()
{
    pgL.setStrVal (ui->PageNoL->text ());
    ui->PageNoL->setText (pgL.getStrVal ());
    ++pgL;
    base = pgL.getValue ();
    count = 0;
    update ();
}

//reset right camera start page number
void MainWindow::on_PageNoR_returnPressed()
{
    pgR.setStrVal (ui->PageNoR->text ());
    ui->PageNoR->setText (pgR.getStrVal ());
    pgL.setStrVal (ui->PageNoR->text ());
    ++pgL;
    base = pgL.getValue () + 1;
    count = 0;
    ui->PageNoL->setText (pgL.getStrVal ());
    update ();
}

void MainWindow::resetSerial(){
    serial = 1;
    //regex pattern for base part of image file names
    QString re = "^" + prefix + "(" + padChar + "*)[1-9][0-9]*$";
    QRegExp regex(re);
    QDir fldr(dirName);
    fldr.setFilter (QDir::Files);   // list only files, no directories
    fldr.setSorting(QDir::Name | QDir::Reversed);
    QFileInfoList list = fldr.entryInfoList();
    int sz = list.size ();
    if(sz > 0){
        int i = 0;
        QString baseName = list.at(i).baseName ();
        while(regex.indexIn (baseName, prefix.length ()) == -1 && ++i < sz){
           // QMessageBox::information (this, "info", baseName);
            baseName = list.at(i).baseName ();
        }
        if(i < sz){
            //QMessageBox::information (this, "info", baseName);
            int fst = prefix.length () + regex.cap(1).length ();
            serial = baseName.mid(fst, prefix.length () + len - fst).toInt () + 1;
        }
    }

    base = serial;
    count = 2;
}
