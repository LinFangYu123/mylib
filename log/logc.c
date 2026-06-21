#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "logc.h"

static char *log_filename = NULL;
FILE *pFile = NULL;

/* 前置声明 */
static void get_current_time(char *buf, int32_t len);
static void log_close(void);

void log_create(char *filename)
{
    if (log_filename)
    {
        free(log_filename);
    }
    log_filename = strdup(filename);
    if (!log_filename)
    {
        perror("strdup");
        exit(1);
    }

    pFile = fopen(filename, "a+");

    if (pFile == NULL)
    {
        perror("fopen");
        exit(1);
    }

    log_close();

    return;
}

static void vlog_write(const char* fmt, va_list args)
{
    char buf[MAX_LOG_LEN + 1] = {0};
    char time[128] = {0};

    get_current_time(time, sizeof(time) - 1);
    vsnprintf(buf, sizeof(buf), fmt, args);
    fprintf(pFile, "[%s] %s", time, buf);
    fflush(pFile);

    return;
}

void log_write(const char *fmt, ...)
{
    if (!log_filename)
    {
        return;
    }

    pFile = fopen(log_filename, "a+");
    if (!pFile)
    {
        return;
    }

    va_list args;

    va_start(args, fmt);
    vlog_write(fmt, args);
    va_end(args);
    log_close();

    return;
}

static void get_current_time(char *buf, int32_t len)
{
    time_t timep;
    struct tm timer;

    timep = time(NULL);
    localtime_r(&timep, &timer);

    snprintf(buf, len, "%d/%d/%d %d:%d:%d",
        (1900 + timer.tm_year), (1 + timer.tm_mon), timer.tm_mday,
        timer.tm_hour, timer.tm_min, timer.tm_sec);

    return;
}

static void log_close()
{
    if (pFile != NULL)
    {
        fclose(pFile);
        pFile = NULL;
    }

    return;
}
