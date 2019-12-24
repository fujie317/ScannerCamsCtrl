/*
 * capture.c
 *
 *  Created on: Feb 26, 2019
 *      Author: Fujie Zhang
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include "mygphoto2.h"

int capture (const char *filename, Camera *camera, GPContext *context) {
  int fd, retval;
  CameraFile *file;
  CameraFilePath camera_file_path;

  // take a shot
  retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, context);

  if (retval) {
      // do some error handling, probably return from this function
    }

  printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

  fd = open(filename, O_CREAT | O_WRONLY, 0644);
  // create new CameraFile object from a file descriptor
  retval = gp_file_new_from_fd(&file, fd);

  if (retval) {
      // error handling
    }

  // copy picture from camera
  retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name, GP_FILE_TYPE_NORMAL, file, context);

  if (retval) {
      // error handling
    }

  printf("Deleting image on camera\n");
  // remove picture from camera memory
  retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name, context);

  if (retval) {
      // error handling
    }

  // free CameraFile object
  gp_file_free(file);

  // Code from here waits for camera to complete everything.
  // Trying to take two successive captures without waiting
  // will result in the camera getting randomly stuck.
  int waittime = 2000;
  CameraEventType type;
  void *data;

  printf("Wait for events from camera\n");
  while(1) {
      retval = gp_camera_wait_for_event(camera, waittime, &type, &data, context);
      if(type == GP_EVENT_TIMEOUT) {
          break;
        }
      else if (type == GP_EVENT_CAPTURE_COMPLETE) {
          printf("Capture completed\n");
          waittime = 100;
        }
      else if (type != GP_EVENT_UNKNOWN) {
          printf("Unexpected event received from camera: %d\n", (int)type);
        }
      else{
          printf("Done capturing one image to computer!\n");
          break;
        }
    }

  return 0;
}



