#ifndef _LOGC_H
#define _LOGC_H

#include <stdio.h>

#define MAX_LOG_LEN    1024

extern FILE *pFile;

void log_create(char *filename);
void log_write(const char *fmt, ...);

#endif
