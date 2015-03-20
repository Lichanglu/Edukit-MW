#ifndef _APP_UPDATE_H__
#define _APP_UPDATE_H__

//#define PRINTF printf

int openGPIO();
int closeGPIO(int *fd);
int updatefpga(int fd,int fpgaNo,const char *file);
int updatekernel(const char *file);
int updateuboot(const char *file);

#endif

