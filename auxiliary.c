/*
 * auxiliary.c
 *
 *  Created on: Feb 28, 2019
 *      Author: Fujie Zhang
 */


#include <stdio.h>
#include "mygphoto2.h"

void _errordumper(GPLogLevel level, const char *domain, const char *str, void *data) {
  printf("%s (data %p)\n", str,data);
}

void error_func (GPContext *context, const char *format, va_list args, void *data) {
  fprintf  (stderr, "*** Context error ***\n");
  vfprintf (stderr, format, args);
  fprintf  (stderr, "\n");
}

void message_func (GPContext *context, const char *format, va_list args, void *data) {
  vprintf (format, args);
  printf ("\n");
}
