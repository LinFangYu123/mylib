#include <time.h>
#include "logc.h"
#include <stdlib.h>

void log_create(char *filename){
	pFile = fopen(filename,"a+");
	if(pFile == NULL){
		perror("fopen");
		exit(1);
	}
	log_close();
}

void log_write(const char *fmt, ...)
{
	pFile = fopen(filename,"a+");
	va_list args;
 
	va_start(args, fmt);
	vlog_write(fmt, args);
	va_end(args);
	log_close();
}

static void vlog_write(const char* fmt, va_list args)
{
	char buf[MAX_LOG_LEN+1] = {0};
	char time[128] = {0};
	get_current_time(time, sizeof(time) - 1);
	vsnprintf(buf, sizeof(buf), fmt, args);
	fprintf(pFile, "[%s] %s", time, buf);
	fflush(pFile);
}

static void get_current_time(char *buf, int len)
{
	time_t timep;
	struct tm timer;
	
	timep = time(NULL);
	localtime_r(&timep,&timer);
	
	snprintf(buf, len,"%d/%d/%d %d:%d:%d", \
		(1900 + timer.tm_year), (1 + timer.tm_mon), timer.tm_mday, \
		timer.tm_hour, timer.tm_min, timer.tm_sec);
}
 
static void log_close(){
	if(pFile!=NULL){
		fclose(pFile);
		pFile = NULL;
	}
}