#ifndef MYGPHOTO2_H
#define MYGPHOTO2_H

#include <gphoto2/gphoto2-camera.h>
#include <gphoto2/gphoto2-file.h>

#ifdef __cplusplus
extern "C" {
#endif

extern int autodetect (CameraList *list, GPContext *context); //autodetect.c
extern int open_camera (Camera ** camera, const char *model, const char *port, GPContext *context); //autodetect.c

extern GPContext* create_context(void); //context.c

extern int get_config_value_string (Camera *, const char *, char **, GPContext *); //config.c
extern int get_config_value_double (Camera *, const char *, double *, GPContext *); //config.c
extern int set_config_value_string (Camera *, const char *, const char *, GPContext *); //config.c

extern void _errordumper(GPLogLevel level, const char *domain, const char *str, void *data);  //auxiliary.c
extern void error_func (GPContext *context, const char *format, va_list args, void *data);  //auxiliary.c
extern void message_func (GPContext *context, const char *format, va_list args, void *data);  //auxiliary.c

extern int camera_preview(Camera *camera, GPContext *context, CameraFile *file, const char **data, long unsigned int *size); //preview.c
extern int capture (const char *filename, Camera *camera, GPContext *context);

#if !defined (O_BINARY)
        /*To have portable binary open() on *nix and on Windows */
        #define O_BINARY 0
#endif

#ifdef __cplusplus
};
#endif

#endif // MYGPHOTO2_H
