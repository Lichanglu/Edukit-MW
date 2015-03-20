#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/types.h>
#include <linux/rtc.h>
#include <errno.h>
#include "reach_update.h"

//#define KERNELFILENAME		"/opt/dvr_rdk/ti816x_2.8/update/uImage"
//#define FPGAFILENAME		"/opt/dvr_rdk/ti816x_2.8/update/fpga.bin"
typedef struct _R_GPIO_data_ {
	unsigned int gpio_num;
	unsigned int gpio_value;
} R_GPIO_data;

#define FLASH_ENABLE_1	39
#define FPGA_PRO_1	2
#define FPGA_DONE_1	3

#define FLASH_ENABLE_2	40
#define FPGA_PRO_2	0
#define FPGA_DONE_2	1

#define SET_GPIO	(0x55555555)
#define GET_GPIO	(0xAAAAAAAA)

static int SetGPIOBitValue(int fd, int bit);
static int ClearGPIOBitValue(int fd, int bit);
static int GetGPIOBitValue(int fd, int bit);
static int set_gpio_bit(int bit, int fd);
static int clear_gpio_bit(int bit, int fd);
static int get_gpio_bit(int bit, int fd);
//static int set_is_update_fpga(int status);

int openGPIO()
{
	int fd;
	fd = open("/dev/Rgpio",O_RDWR | O_SYNC);
	return fd;
}

int closeGPIO(int *fd)
{
	if(fd == NULL)
	{
		return -1;
	}
	if(*fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
	
	return 0;
}

/*set GPIO port Bit value*/
static int SetGPIOBitValue(int fd, int bit)
{
	R_GPIO_data op;
	int ret ;

	op.gpio_value = 1 ;
	op.gpio_num = bit;
	ret = ioctl(fd, SET_GPIO, &op);

	if(ret < 0)	{
		return -1 ;
	}

	return 0;
}

/*clear GPIO port Bit value*/
static int ClearGPIOBitValue(int fd, int bit)
{
	R_GPIO_data op;
	int ret ;

	op.gpio_value = 0 ;
	op.gpio_num = bit;
	ret = ioctl(fd, SET_GPIO, &op);

	if(ret < 0)	{
		return -1 ;
	}

	return 0;
}
/*get GPIO port Bit value*/
static int GetGPIOBitValue(int fd, int bit)
{

	R_GPIO_data op;
	int ret , val = -1 ;

	op.gpio_value = 0 ;
	op.gpio_num = bit;
	ret = ioctl(fd,	GET_GPIO, &op);

	if(ret < 0)	{
		return -1 ;
	}

	val = op.gpio_value;
	return (val);
}





static int set_gpio_bit(int bit, int fd)
{
	int ret = 0;
	ret = SetGPIOBitValue(fd, bit);

	if(ret < 0) {
		printf("set_gpio_bit() failed!!!,bit:%d,fd:%d\n ",bit,fd);
		return -1 ;
	}

	printf("balance mode switch !!!!bit=%d\n", bit);
	return 0;
}



static int clear_gpio_bit(int bit, int fd)
{
	int ret = 0;
	ret = ClearGPIOBitValue(fd, bit);

	if(ret < 0) {
		printf("clear_gpio_bit() failed!!!,bit:%d,fd:%d\n ",bit,fd);
		return -1 ;
	}

	return 0;
}

static int get_gpio_bit(int bit, int fd)
{
	int ret =  -1;
	ret = GetGPIOBitValue(fd, bit);

	if(ret < 0) {
		printf("GetGPIOBitValue() failed!!!,bit:%d,fd:%d\n ",bit,fd);
		return -1 ;
	}

	return (ret);
}

#define PER_READ_LEN  256
/*升级FPGA程序*/
static int updateFpgaProgram(const char *fpgafile, int fd,int fpgaNo)
{
	int r_cnt1=0,r_cnt2=0;
	int ret = 0;
	char spidata[PER_READ_LEN];
	int spifd = -1;
	FILE *fpgafd = NULL;
	int readlen = 0;
	int writelen = 0;
	int totalwritelen  = 0;
	int flshEnableBit,fpgaProBit,fpgaDoneBit;
	//升级FPGA1
	if(0 == fpgaNo)
	{
		flshEnableBit = FLASH_ENABLE_1;
		fpgaProBit = FPGA_PRO_1;
		fpgaDoneBit = FPGA_DONE_1;
	}
	else
	{
		flshEnableBit = FLASH_ENABLE_2;
		fpgaProBit = FPGA_PRO_2;
		fpgaDoneBit = FPGA_DONE_2;
	}
	printf("Enter into update FPGA Program \n");
	ret = clear_gpio_bit(flshEnableBit, fd);
	printf("FLASH_ENABLE ret=%x \n", ret);
	
	clear_gpio_bit(fpgaProBit, fd);
	while(1){
		usleep(100);
		ret = get_gpio_bit(fpgaDoneBit, fd);
		if( ret != 0 ){
			printf("fpga Done is Not 0!ret:%d\n",ret);
		}else{
			printf("fpga Done is 0!\n");
			break;
		}
		r_cnt1++;
		if( r_cnt1>10){
			break;
		}
	}

	if(0 == fpgaNo)
	{
		ret = system("flash_eraseall /dev/mtd0");
		printf(" flash_eraseall /dev/mtd0 ret=%x \n", ret);	
		spifd =  open("/dev/mtd0", O_RDWR, 0);
		if(spifd < 0)	{
			printf("open the SPI flash 0 Failed \n");
			ret = -1;
			goto cleanup;
		}
	}
	else
	{
		ret = system("flash_eraseall /dev/mtd1");
		printf(" flash_eraseall /dev/mtd1 ret=%x \n", ret);	
		spifd =  open("/dev/mtd1", O_RDWR, 0);
		if(spifd < 0)	{
			printf("open the SPI flash 1 Failed \n");
			ret = -1;
			goto cleanup;
		}
	}

	fpgafd = fopen(fpgafile, "r+b");
	if(fpgafd == NULL)	{
		printf("open the FPGA bin Failed \n");
		ret = -1;
		goto cleanup;
	}
	rewind(fpgafd);

	while(1) {		
		readlen = fread(spidata, 1, PER_READ_LEN, fpgafd);

		if(readlen < 1)	{
			printf("file read end \n");
			break;
		}

		writelen = write(spifd, spidata, readlen);
		totalwritelen += writelen;

		if(feof(fpgafd)) {
			printf("writelen = %d \n", writelen);
			writelen = write(spifd, spidata, readlen);
			break;
		}
	}

	close(spifd);
	spifd  = -1;
	printf("totalwritelen = %d \n", totalwritelen);
cleanup:
	printf("002 flash_eraseall updateFpgaProgram \n");
	ret = set_gpio_bit(flshEnableBit, fd);
	printf("FLASH_ENABLE ret=%x \n", ret);

	if(spifd > 0) {
		close(spifd);
	}

	if(fpgafd) {
		fclose(fpgafd);
	}
	ret = set_gpio_bit(fpgaProBit, fd);
	printf("========set_gpio_bit:ret:%d\n",ret);
	usleep(300);
	while( 1 ) {
		ret = get_gpio_bit(fpgaDoneBit, fd);
		printf("========get_gpio_bit:ret:%d\n",ret);
		if( ret == 0){//正在写，允许写3s
			r_cnt1++;
			r_cnt2=0;
		}else if(1 ==  ret ){ //写完成，确保5次
			r_cnt2++;
			r_cnt1=0;
		}else{
			break;
		}
		if(r_cnt1 >10 || r_cnt2 >5){
			break;
		}
		usleep(300);
	}
	return ret;
}

int updatekernel(const char *file)
{
	char command[256] = {0};

	if(NULL == file)
	{
		printf("updatekernel:file is NULL\n");
		return -1;
	}

	/*更新内核*/
	//主系统是mtd4 ,副系统是mtd5
	if(access(file, F_OK) == 0) {
		printf("i Will 'flash_eraseall /dev/mtd4'\n");
		system("flash_eraseall /dev/mtd4");
		sprintf(command, "nandwrite -p /dev/mtd4 %s", file);
		printf("i Will '%s'\n", command);
		system(command);

		printf("update kernel is end\n");

	}

	return 0;
}

int updateuboot(const char *file)
{
	char command[256] = {0};

	if(NULL == file)
	{
		printf("updateuboot:file is NULL\n");
		return -1;
	}

	/*更新uboot,分区为mtd2*/
	if(access(file, F_OK) == 0) {
		printf("i Will 'flash_eraseall /dev/mtd2'\n");
		system("flash_eraseall /dev/mtd2");
		sprintf(command, "nandwrite -p /dev/mtd2 %s", file);
		printf("i Will '%s'\n", command);
		system(command);

		printf("update uboot is end\n");

	}

	return 0;
}

int updatefpga(int fd,int fpgaNo,const char *file)
{
	int ret = 0;
	
	if(NULL == file)
	{
		printf("updatefpga:file is NULL\n");
		return -1;
	}

	if(fd < 0)
	{
		printf("updatefpga:gpio fd is %d!\n",fd);
		return -1;
	}
	
	if(access(file, F_OK) == 0) {
		sleep(1);
		printf("----begin to update fpga---\n");
		ret = updateFpgaProgram(file,fd, fpgaNo);
		printf("----end to update fpga---\n");
	} else {
		printf("have no file \n");
	}
	return ret;
}
