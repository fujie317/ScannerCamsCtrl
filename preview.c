/*
 * auxiliary.c
 *
 *  Created on: Feb 28, 2019
 *      Author: Fujie Zhang
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "mygphoto2.h"

int camera_preview(Camera *camera, GPContext *context, CameraFile *file, const char **data, long unsigned int *size){
	int retval;

	retval = gp_camera_capture_preview(camera, file, context);
	if (retval != GP_OK) {
		fprintf(stderr,"gp_camera_capture_preview failed with return value: %d\n", retval);
		exit(1);
	}

	gp_file_set_mime_type(file, GP_MIME_JPEG);  //set mime type of image file
	gp_file_get_data_and_size(file, data, size);

	usleep(40000); //force 20 milliseconds between calls

	return *size;
}
