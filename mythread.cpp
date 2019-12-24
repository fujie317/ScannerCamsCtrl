#include "mythread.h"
#include <QDebug>
#include "mygphoto2.h"
#include <QtMath>

MyThread::MyThread(QObject *parent) :
    QThread(parent)
{
}

// run() will be called when a thread starts
void MyThread::run()
{
    CameraList *list;
    int ret, i;
    unsigned long retval;

    context = create_context (); /* see context.c */

    // set callbacks for camera messages, eror_func cast not needed when compiled as pure C, required for C++
    gp_context_set_error_func(context, (GPContextErrorFunc)error_func, nullptr);
    gp_context_set_message_func(context, (GPContextStatusFunc)message_func, nullptr);  // ditto


    ret = gp_list_new (&list);
    if (ret < GP_OK) return;

    printf("Camera init. Can take more than 10 seconds depending on the "
           "memory card's contents (remove card from camera to speed up).\n");
    //This call will auto detect cameras
    count = autodetect (list, context);
    if (count < GP_OK) {
        fprintf(stderr, "No cameras detected.\n");
        return;
    }

    /* Now open all cameras we autodected for usage */
    printf("Number of cameras: %d\n", count);
    cams = static_cast<Camera **>(calloc (sizeof (Camera*),static_cast<size_t>(count)));

    /* Now call a simple function in each of those cameras. */
    const char *name, *port;

    CameraFile *file;
    const char *data;
    unsigned long int size;
    unsigned char *udata;

    imgSize = "";
    imgAspectRatio = "";
    tmp = "";
    isoSpeed = "";
    imgQuality = "";

    char **str;
    // double value;

    for (i = 0; i < count; i++) {
        gp_list_get_name  (list, i, &name);
        gp_list_get_value (list, i, &port);
        printf("Camera %d, name=%s, port=%s\n", i+1, name, port);
        fflush(stdout);
        ret = open_camera (&cams[i], name, port, context);
        if (ret < GP_OK){
            fprintf(stderr,"Camera %s on port %s failed to open\n", name, port);
            fflush(stderr);
        }

        get_config_value_string(cams[i], "Aspect Ratio", str, context);
           // delete *str;

        if(imgAspectRatio.length () == 0){
            imgAspectRatio.append(*str);
        }
        else if (imgAspectRatio.compare (*str)){
            imgAspectRatio.append('/');
            imgAspectRatio.append(*str);
        }

        tmp.clear ();
        tmp.append (*str);
        delete *str;

        get_config_value_string (cams[i], "Image Size", str, context);
        if(imgSize.length () == 0){
            imgSize.append(formatImgSize (*str, tmp));
        }
        else if (imgSize.compare (formatImgSize (*str, tmp))){
            imgSize.append('/');
            imgSize.append(formatImgSize (*str, tmp));
        }
        delete *str;

//        get_config_value_string(cams[i], "ISO Speed", str, context);
//        if(isoSpeed.length () == 0){
//            isoSpeed.append(*str);
//        }
//        else if (isoSpeed.compare (*str)){
//            isoSpeed.append('/');
//            isoSpeed.append(*str);
//        }
//        delete *str;

//        get_config_value_string(cams[i], "Image Quality", str, context);
//        if(imgQuality.length () == 0){
//            imgQuality.append(*str);
//        }
//        else if (imgQuality.compare (*str)){
//            imgQuality.append('/');
//            imgQuality.append(*str);
//        }
//        delete *str;

        /*
        // not working
        get_config_value_double(cams[i], "Zoom", &value, context);
        printf("Zoom: %lf for Camera %d\n", value, i);
    */
    }

    emit(initialized ());       // inform main window
    ret = gp_file_new(&file);
    if (ret != GP_OK) {
        fprintf(stderr,"gp_file_new: %d\n", ret);
        exit(1);
    }

    while(true){
        mutex.lock ();
        for(int i = 0; i < count; ++i){
            retval = static_cast<unsigned long>(camera_preview(cams[i], context, file, &data, &size));

            if (retval < size) {
                fprintf(stderr,"gp_camera_capture_preview failed with return value: %lu\n", retval);
                exit(1);
            }

            udata = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * size));
            for(unsigned long int j = 0; j < size; ++j){
                udata[j] = static_cast<unsigned char>(data[j]);
            }

            QImage image = QImage::fromData(udata, static_cast<int>(size));

            // Rotate images by 90 degrees for displaying on screen
            QPoint center = image.rect().center();
            QMatrix matrix;
            matrix.translate(center.x(), center.y());
            if(i == 0){
                matrix.rotate(-90);
                QImage rotatedImg = image.transformed(matrix);
                previewPixmapL = QPixmap::fromImage(rotatedImg);
            }
            if(i == 1){
                matrix.rotate(90);
                QImage rotatedImg = image.transformed(matrix);
                previewPixmapR = QPixmap::fromImage(rotatedImg);
            }

            free(udata);
        }
        emit previewChanged ();
        mutex.unlock ();
        QThread::msleep (200);
    }
    //gp_file_unref(file);
}

QString MyThread::formatImgSize(QString sz, QString ar){
    int width = 0;
    int height = 0;
    int pixelWidth = 0;
    int pixelHeight = 0;
    QStringList list = ar.split(":");
    if(list.size () > 1){
        width = list[0].toInt ();
        height = list[1].toInt ();

        if(!sz.compare ("Large")){
            pixelWidth = static_cast<int>(qSqrt(24000000 / height * width));
            pixelHeight = static_cast<int>(qSqrt(24000000 / width * height));
        }
        else if(!sz.compare ("Medium")){
            pixelWidth = static_cast<int>(qSqrt(12000000 / height * width));
            pixelHeight = static_cast<int>(qSqrt(12000000 / width * height));
        }
        else if(!sz.compare ("Small")){
            pixelWidth = static_cast<int>(qSqrt(6000000 / height * width));
            pixelHeight = static_cast<int>(qSqrt(6000000 / width * height));
        }
    }

    QString imgSize = "%1 X %2";

    return imgSize.arg (pixelWidth).arg(pixelHeight);
}
