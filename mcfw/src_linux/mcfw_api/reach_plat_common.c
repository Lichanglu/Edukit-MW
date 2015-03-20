#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include "reach_plat_common.h"
#include "reach_update.h"
#include "mcfw/src_linux/devices/inc/device_videoDecoder.h"
#include "mcfw/interfaces/common_def/ti_vcap_common_def.h"
#include "mcfw/interfaces/link_api/system_linkId.h"

#include "mcfw/src_linux/devices/adv7441/inc/adv7441.h"
#include "mcfw/src_linux/devices/adv7442/inc/adv7442.h"
#include "mcfw/src_linux/devices/gs2971/inc/gs2971.h"
#include "mcfw/src_linux/devices/gs2972/inc/gs2972.h"
#include "mcfw/src_linux/devices/adv7844/inc/adv7844.h"
#include "mcfw/src_linux/devices/adv7844_2/inc/adv7844_2.h"
#include "mcfw/src_linux/devices/gv7601/inc/gv7601.h"
#include "mcfw/src_linux/devices/gv7602/inc/gv7602.h"
 
static int s_capture_dev_type[MAXDEVNUM] = {GV760,GV760};

const const Signal_Table s_signal_table[DEVICE_STD_REACH_LAST + 1] = {
	{{"480I",		263, 60, 1587, 720, 240}, DEVICE_STD_NTSC, 1, 1}, // 0-480ix60
	{{"576I",		313, 50, 1600, 720, 288}, DEVICE_STD_PAL, 1, 1}, // 1-576ix50
	{{"480I",		263, 60, 1587, 720, 240}, DEVICE_STD_480I, 1, 1}, // 2-480ix60
	{{"576I",		313, 50, 1600, 720, 288}, DEVICE_STD_576I, 1, 1}, // 3-576ix50
	{{"Revs",		0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_CIF, 1, 1}, // FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
	{{"Revs",		0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_HALF_D1, 1, 1}, // FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
	{{"Revs",		0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_D1, 1, 1}, // FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */
	{{"480P",		0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_480P, 1, 1}, // 7-480px60
	{{"576P",		0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_576P, 1, 1}, // 8-576px50

	{{"720P60",	750, 60, 1111, 1280, 720}, DEVICE_STD_720P_60, 1, 1}, // 9-1280x720x60
	{{"720P50",	750, 50, 1333, 1280, 720}, DEVICE_STD_720P_50, 1, 1}, // 10-1280x720x50
	{{"1080I60",	563, 60, 1481, 1920, 540}, DEVICE_STD_1080I_60, 1, 1}, // 11-1920x1080x60i
	{{"1080I50",	562, 50, 1777, 1920, 540}, DEVICE_STD_1080I_50, 1, 1}, // 12-1920x1080x50i
	{{"1080P60",	1125, 60, 740, 1920, 1080}, DEVICE_STD_1080P_60, 1, 1}, // 13-1920x1080x60p
	{{"1080P50",	1125, 50, 888, 1920, 1080}, DEVICE_STD_1080P_50, 1, 1}, // 14-1920x1080x50p
	{{"1080P25",	1125, 25, 1777, 1920, 1080}, DEVICE_STD_1080P_24, 1, 1}, // 15-1920x1080x25p
	{{"1080P30",	1125, 30, 1481, 1920, 1080}, DEVICE_STD_1080P_30, 1, 1}, // 16-1920x1080x30p

	{{"640x480@60",	525, 60, 1588, 640, 480}, DEVICE_STD_VGA_640X480X60, 1, 1}, // 17-640x480x60
	{{"640x480@72",	520, 72, 1320, 640, 480}, DEVICE_STD_VGA_640X480X72, 1, 1}, // 18-640x480x72
	{{"640x480@75",	500, 75, 1333, 640, 480}, DEVICE_STD_VGA_640X480X75, 1, 1}, // 19-640x480x75
	{{"640x480@85",	509, 85, 1155, 640, 480}, DEVICE_STD_VGA_640X480X85, 1, 1}, // 20-640x480x85
	{{"800x600@60",	628, 60, 1320, 800, 600}, DEVICE_STD_VGA_800X600X60, 1, 1}, // 21-800x600x60
	{{"800x600@72",	666, 72, 1040, 800, 600}, DEVICE_STD_VGA_800X600X72, 1, 1}, // 22-800x600x72
	{{"800x600@75",	625, 75, 1066, 800, 600}, DEVICE_STD_VGA_800X600X75, 1, 1}, // 23-800x600x75
	{{"800x600@85",	631, 85, 931, 800, 600}, DEVICE_STD_VGA_800X600X85, 1, 1}, // 24-800x600x85
	{{"1024x768@60",	806, 60, 1033, 1024, 768}, DEVICE_STD_VGA_1024X768X60, 1, 1}, // 25-1024x768x60
	{{"1024x768@70",	806, 70, 885, 1024, 768}, DEVICE_STD_VGA_1024X768X70, 1, 1}, // 26-1024x768x70
	{{"1024x768@75",	800, 75, 833, 1024, 768}, DEVICE_STD_VGA_1024X768X75, 1, 1}, // 27-1024x768x75
	{{"1024x768@85",	808, 85, 728, 1024, 768}, DEVICE_STD_VGA_1024X768X85, 1, 1}, // 28-1024x768x85
	{{"1280x720@60",	750, 60, 1111, 1280, 720}, DEVICE_STD_VGA_1280X720X60, 1, 1}, // 29-1280x720x60
	{{"1280x768@60",	798, 60, 1054, 1280, 768}, DEVICE_STD_VGA_1280X768X60, 1, 1}, // 30-1280x768x60
	{{"1280x768@75",	0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_VGA_1280X768X75, 1, 1}, // 31-1280x768x75
	{{"1280x768@85",	0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_VGA_1280X768X85, 1, 1}, // 32-1280x768x85
	{{"1280x800@60",	828, 60, 1006, 1280, 800}, DEVICE_STD_VGA_1280X800X60, 1, 1}, // 33-1280x800x60
	{{"1280x960@60",	1000, 60, 833, 1280, 960}, DEVICE_STD_VGA_1280X960X60, 1, 1}, // 34-1280x960x60
	{{"1280x1024@60",	1066, 60, 781, 1280, 1024}, DEVICE_STD_VGA_1280X1024X60, 1, 1}, // 35-1280x1024x60
	{{"1280x1024@75",	1066, 75, 625, 1280, 1024}, DEVICE_STD_VGA_1280X1024X75, 1, 1}, // 36-1280x1024x75
	{{"1280x1024@85",	0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_VGA_1280X1024X85, 1, 1}, // 37-1280x1024x85
	{{"1366x768@60",	795, 60, 1047, 1366, 768}, DEVICE_STD_VGA_1366X768X60, 1, 1}, // 38-1366x768x60
	{{"1440x900@60",	934, 60, 901, 1440, 900}, DEVICE_STD_VGA_1440X900X60, 1, 1}, // 39-1440x900x60
	{{"1400x1050@60",	1089, 60, 765, 1400, 1050}, DEVICE_STD_VGA_1400X1050X60, 1, 1}, // 40-1400x1050x60
	{{"1400x1050@75",	0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_VGA_1400X1050X75, 1, 1}, // 41-1400x1050x75
	{{"1600x1200@60",	1250, 60, 666, 1600, 1200}, DEVICE_STD_VGA_1600X1200X60, 1, 1}, // 42-1600x1200x60
	{{"1920x1080@60_DMT", 1125, 60, 740, 1920, 1080}, DEVICE_STD_VGA_1920X1080X60_DMT, 1, 1}, // 43-1920x1080x60-DMT
	{{"1920x1080@60_GTF", 1125, 60, 740, 1920, 1080}, DEVICE_STD_VGA_1920X1080X60_GTF, 1, 1}, // 44-1920x1080x60-GTF
	{{"1920x1200@60",	1244, 60, 675, 1920, 1200}, DEVICE_STD_VGA_1920X1200X60, 1, 1}, // 45-1920x1200x60
	{{"2560x1440@60",	1481, 60, 0xFFFF, 2560, 1440}, DEVICE_STD_VGA_2560X1440X60, 1, 1}, // 46-2560x1440x60

	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_2CH_D1, 1, 1}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_2CH_HALF_D1, 1, 1}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_2CH_CIF, 1, 1}, // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_4CH_D1, 1, 1}, // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_4CH_CIF, 1, 1}, // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_4CH_HALF_D1, 1, 1}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_8CH_CIF, 1, 1}, // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_MUX_8CH_HALF_D1, 1, 1}, // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_AUTO_DETECT, 1, 1}, // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{{"Revs", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_CUSTOM, 1, 1}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
	//The video timing is provided by the application.
	//Used in display mode. */

	{{"Max", 0xFFFF, 0xFFFF, 0xFFFF, 0, 0}, DEVICE_STD_REACH_LAST, 0, 0} // FVID2_STD_MAX
};

/*获取FPGA版本号*/
Int32 platform_get_fpga_version(int devId)
{
	OSA_I2cHndl i2cHandle;
	UInt8 regAddr[1]={0};
	UInt8 regValue[2]={0};
	UInt16 devAddr = 0;
	UInt16 data = 0;
	Int32 status = OSA_SOK;

        status = OSA_i2cOpen(&i2cHandle, I2C_DEFAULT_INST_ID);
        if ( status < 0)
        {
        	OSA_printf("======OSA_i2cOpen:i2cHandle:%p\n",(&i2cHandle));
        	status = OSA_EFAIL;
		return status;
        }
	
	regAddr[0] = 30;
	if(0 == devId)
	{
		devAddr = 0x3c;
	}
	else
	{
		devAddr = 0x3d;
	}
	OSA_printf("=============devId:%d\n",devId);
	status = OSA_CPLD_i2cRead16(&i2cHandle,devAddr, regAddr, regValue, 2);
        if ( status < 0)
        {
        	OSA_printf("======OSA_CPLD_i2cRead16 is error!\n");
		OSA_i2cClose(&i2cHandle);
		status = OSA_EFAIL;
		return status;
        }

	data = regValue[1]<<8 | regValue[0];		
	
	OSA_i2cClose(&i2cHandle);
	return (Int32)data;
}

/*获取内核版本号*/
Int32 platform_get_kernel_version(Int32 fd,Int8 *kernel_version)
{
	Int32 status = OSA_SOK;
	int ret ;

	if(fd < 0)
	{
		return OSA_EFAIL;
	}

	ret = ioctl(fd, 0x11111111, kernel_version);
	if(ret < 0)
		printf("platform_get_kernel_version-11111111:fd:%d,ret:%d\n",fd,ret);
	
	return status;
}


/*更新FPGA程序*/
Int32 platform_update_fpga(Int32 fd,Int32 fpgaNo,const Int8 *file)
{
	Int32 ret = 0;
	
	ret = updatefpga(fd,fpgaNo,file);
	return ret;
}

/*更新内核*/
Int32 platform_update_kernel(const Int8 *file)
{
	Int32 ret = 0;
	
	ret = updatekernel(file);
	
	return ret;
}

/*设置VP口的采集为7441*/
Device_Adv7441Handle platform_vp_select_adv7441(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Adv7441Handle handle 			= NULL;

	s_capture_dev_type[0] = ADV744;
	
	SelectAD7441(fd);

	Device_adv7441Delete(gDevice_adv7441CommonObj.adv7441handle[0],NULL);
	Device_adv7441Init();
	
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	handle = (Void *)Device_adv7441Create(0, 0, &(createArgs), &(createStatusArgs));

	if(NULL != handle)
	{
		//设置成高清模式
		Cap_SetSource7441Chan((void **)&handle, 1);
	}
	 
	return handle;
}

/*设置VP口的采集为7442*/
Device_Adv7442Handle platform_vp_select_adv7442(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Adv7442Handle handle 			= NULL;

	s_capture_dev_type[1] = ADV744;
	
	SelectAD7442(fd);

	Device_adv7442Delete(gDevice_adv7442CommonObj.adv7442handle[0],NULL);
	Device_adv7442Init();
	
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;

	handle = (Void *)Device_adv7442Create(0, 0, &createArgs, &createStatusArgs);

	if(NULL != handle)
	{
		//设置成高清模式
		Cap_SetSource7442Chan((void **)&handle, 1);
	}
	 
	return handle;
}

/*设置VP口的采集为gs2971*/
Device_Gs2971Handle platform_vp_select_gs2971(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Gs2971Handle handle = NULL;

	s_capture_dev_type[0] = GS297;
	
	SelectGS2971(fd);

	Device_gs2971Delete(gDevice_gs2971CommonObj.gs2971handle[0],NULL);
	Device_gs2971Init();
	
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	
	handle = Device_gs2971Create(0, 0, &(createArgs), &(createStatusArgs));
	 
	return handle;
}

/*设置VP口的采集为gs2972*/
Device_Gs2972Handle platform_vp_select_gs2972(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Gs2972Handle handle = NULL;

	s_capture_dev_type[1] = GS297;
	
	SelectGS2972(fd);

	Device_gs2972Delete(gDevice_gs2972CommonObj.gs2972handle[0],NULL);
	Device_gs2972Init();
	
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = Device_gs2972Create(0, 0, &(createArgs), &(createStatusArgs));
	 
	return handle;
}

/*设置VP口的采集为7844*/
Device_Adv7844Handle platform_vp_select_adv7844(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Adv7844Handle handle 			= NULL;

	s_capture_dev_type[0] = ADV7844;
	
	SelectAD7844(fd);

	Device_adv7844Delete(gDevice_adv7844CommonObj.adv7844handle[0],NULL);
	Device_adv7844Init();
	
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	
	handle = Device_adv7844Create(0, 0, &(createArgs), &(createStatusArgs));
	if(NULL != handle)
	{
		//设置成高清模式
		Cap_SetSource7844Chan((void **)&handle, 1);
	}
	
	return handle;
}

/*设置VP口的采集为7844_2*/
Device_Adv7844_2Handle platform_vp_select_adv7844_2(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Adv7844_2Handle handle 			= NULL;

	s_capture_dev_type[1] = ADV7844;
	
	SelectAD7844_2(fd);

	Device_adv7844_2Delete(gDevice_adv7844_2CommonObj.adv7844_2handle[0],NULL);
	Device_adv7844_2Init();
	
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	
	handle = Device_adv7844_2Create(0, 0, &(createArgs), &(createStatusArgs));
	if(NULL != handle)
	{
		//设置成高清模式
		Cap_SetSource7844_2Chan((void **)&handle, 1);
	}
	 
	return handle;
}
/*设置VP口的采集为gv7601*/
Device_Gv7601Handle platform_vp_select_gv7601(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Gv7601Handle 						handle = NULL;

	s_capture_dev_type[0] = GV760;
	Device_gv7601Delete(gDevice_gv7601CommonObj.gv7601handle[0],NULL);
	Device_gv7601Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = (Void *)Device_gv7601Create(0, 0, &(createArgs), &(createStatusArgs));

	return handle;
}

/*设置VP口的采集为gv7602*/
Device_Gv7602Handle platform_vp_select_gv7602(Int32 fd)
{
	Device_VideoDecoderCreateParams		createArgs;
	Device_VideoDecoderCreateStatus		createStatusArgs;
	Device_Gv7602Handle 					handle = NULL;
	Int32 								byPassChannel;
	Int32 								status;

	s_capture_dev_type[1] = GV760;
	Device_gv7602Delete(gDevice_gv7602CommonObj.gv7602handle[0],NULL);
	Device_gv7602Init();
	memset(&createArgs, 0, sizeof(Device_VideoDecoderCreateParams));

	createArgs.deviceI2cInstId	= 1;
	createArgs.numDevicesAtPort	= 1;
	createArgs.deviceI2cAddr[0] = 0;
	createArgs.deviceResetGpio[0] = DEVICE_VIDEO_DECODER_GPIO_NONE;
	handle = (Void *)Device_gv7602Create(0, 0, &(createArgs), &(createStatusArgs));
	if(NULL != handle)
	{
		status = Device_gv7602Control(handle,
										IOCTL_DEVICE_VIDEO_DECODER_RESET,
										NULL, NULL);

		/*默认通道2选择SDI,可以动态设置*/
		byPassChannel = 2;
		status = Device_gv7602Control(handle,
										IOCTL_DEVICE_VIDEO_DECODER_SET_BYPASS_CH,
										&byPassChannel, NULL);
	}
	return handle;
}

/*设置采集7602的通道类型*/
Int32 platform_vp_set_gv7602_ch_type(void *hand,Int32 chId,DEV_CH_TYPE chType)
{
	Int32 status = OSA_EFAIL;
	Int32 byPassChannel;
	Device_Gv7602Handle 	pobj = NULL;
	
	if(NULL == hand)
	{
		return status;
	}

	pobj = (Device_Gv7602Handle)hand;

	if((0 > chId) || (chId >= MAXCHNUMPERDEV))
	{
		return status;
	}

	if(chType >= MAX_DEV_CH_TYPE)
	{
		return status;
	}
	printf("=========platform_vp_set_gv7602_ch_type\n");
	/*选择DVI*/
	if(CH_DVI == chType)
	{
		byPassChannel = 1;
	}
	/*选择SDI*/
	else
	{
		byPassChannel = 2;
	}
	status = Device_gv7602Control(pobj,
									IOCTL_DEVICE_VIDEO_DECODER_SET_BYPASS_CH,
									&byPassChannel, NULL);
	return status;
}
/*获取采集7602的通道类型*/
Int32 platform_vp_get_gv7602_ch_type(void *hand,Int32 chId,DEV_CH_TYPE *chType)
{
	Int32 status = OSA_EFAIL;
	Device_Gv7602Handle 	pobj = NULL;
	
	if(NULL == hand)
	{
		return status;
	}

	pobj = (Device_Gv7602Handle)hand;

	if((0 > chId) || (chId >= MAXCHNUMPERDEV))
	{
		return status;
	}

	if(NULL == chType)
	{
		return status;
	}

	status = Device_gv7602Control(pobj,
									IOCTL_DEVICE_VIDEO_DECODER_GET_BYPASS_CH,
									chType, NULL);
	return status;
}

/*获取VP口的采集设备type*/
Int32 platform_vp_get_devtype(Int32 devId,Int32 *devType)
{
	Int32 status = OSA_SOK;

	if((NULL == devType) || (devId >= MAXDEVNUM))
	{
		 status = OSA_EFAIL;
		return status;
	}
	
	*devType = s_capture_dev_type[devId];

	return status;
}

/*获取7441的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_adv7441_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Adv7441Handle pObj = NULL;
	Device_VideoDecoderExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((ADV744 != s_capture_dev_type[0]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Adv7441Handle)hand;
	
	ret = Device_adv7441Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}

	ret = Device_adv7441Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_HDCP, &(info->HDCP), NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	ret = Device_adv7441Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取7442的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_adv7442_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Adv7442Handle pObj = NULL;
	Device_VideoDecoderExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((ADV744 != s_capture_dev_type[1]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Adv7442Handle)hand;
	
	ret = Device_adv7442Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}

	ret = Device_adv7442Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_HDCP, &(info->HDCP), NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	ret = Device_adv7442Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取7844的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_adv7844_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Adv7844Handle pObj = NULL;
	Device_VideoDecoderExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((ADV7844 != s_capture_dev_type[0]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Adv7844Handle)hand;
	
	ret = Device_adv7844Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}

	ret = Device_adv7844Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_HDCP, &(info->HDCP), NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	ret = Device_adv7844Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取7844_2的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_adv7844_2_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Adv7844_2Handle pObj = NULL;
	Device_VideoDecoderExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((ADV7844 != s_capture_dev_type[1]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Adv7844_2Handle)hand;
	
	ret = Device_adv7844_2Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}

	ret = Device_adv7844_2Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_HDCP, &(info->HDCP), NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	ret = Device_adv7844_2Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取gs2971的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_gs2971_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Gs2971Handle pObj = NULL;
	Device_VideoDecoderExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((GS297 != s_capture_dev_type[0]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Gs2971Handle)hand;
	
	ret = Device_gs2971Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}

	ret = Device_gs2971Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取gs2972的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_gs2972_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Gs2972Handle pObj = NULL;
	Device_VideoDecoderExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((GS297 != s_capture_dev_type[1]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Gs2972Handle)hand;
	
	ret = Device_gs2972Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}

	ret = Device_gs2972Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取gv7601的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_gv7601_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Gv7601Handle pObj = NULL;
	//Device_VideoDecoder3DExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((GV760 != s_capture_dev_type[0]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Gv7601Handle)hand;
#if 0	
	ret = Device_gv7601Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}
#endif
	ret = Device_gv7601Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*获取gv7602的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_gv7602_status(void *hand, VP_DEVICE_STATUS *info)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Device_Gv7602Handle pObj = NULL;
	//Device_VideoDecoder3DExternInforms signal_info;
	VCAP_VIDEO_SOURCE_CH_STATUS_S videoStatus;

	if((GV760 != s_capture_dev_type[1]) || (NULL == hand) || (NULL == info))
	{
		status = OSA_EFAIL;
		return status;
	}

	pObj = (Device_Gv7602Handle)hand;
#if 0	
	ret = Device_gv7601Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM, &signal_info, NULL);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->mode 		= (Int32)signal_info.ModeID;
		info->isDigital 	= (Int32)signal_info.SignalTmds;
	}
#endif
	ret = Device_gv7602Control(pObj, IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS, NULL, &videoStatus);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	else
	{
		info->frameInterval 	= videoStatus.frameInterval;
		info->isInterlaced 	= videoStatus.isInterlaced;
		info->isVideoDetect 	= videoStatus.isVideoDetect;
		info->frameWidth 		= videoStatus.frameWidth;
		info->frameHeight		= videoStatus.frameHeight;
		
	}

	return status;
}

/*由dev信息获取通道信息*/
Int32 platform_vp_get_ch_status(Int32 chId, VP_DEVICE_STATUS *dev_info,VP_DEVICE_STATUS *ch_info)
{
	Int32 status = OSA_EFAIL;

	if(dev_info == NULL)
	{
		return status;
	}

	if(ch_info == NULL)
	{
		return status;
	}

	if((chId < 0) || (chId >= MAXCHNUMPERDEV))
	{
		return status;
	}

	if(chId == 0)
	{
		ch_info->frameHeight = (Int32)((dev_info->frameHeight) & 0xFFFF);
		ch_info->frameWidth= (Int32)((dev_info->frameWidth) & 0xFFFF);
		ch_info->isVideoDetect= (Int32)((dev_info->isVideoDetect) & 0xFFFF);
		ch_info->isDigital= (Int32)((dev_info->isDigital) & 0xFFFF);
		ch_info->isInterlaced= (Int32)((dev_info->isInterlaced) & 0xFFFF);
		ch_info->frameInterval= (Int32)((dev_info->frameInterval) & 0xFFFF);
		ch_info->HDCP= (Int32)((dev_info->HDCP) & 0xFFFF);
		ch_info->mode= (Int32)((dev_info->mode) & 0xFFFF);
	}
	else if(chId == 1)
	{
		ch_info->frameHeight = (Int32)((dev_info->frameHeight) >> 16);
		ch_info->frameWidth= (Int32)((dev_info->frameWidth) >> 16);
		ch_info->isVideoDetect= (Int32)((dev_info->isVideoDetect) >> 16);
		ch_info->isDigital= (Int32)((dev_info->isDigital) >> 16);
		ch_info->isInterlaced= (Int32)((dev_info->isInterlaced) >> 16);
		ch_info->frameInterval= (Int32)((dev_info->frameInterval) >> 16);
		ch_info->HDCP= (Int32)((dev_info->HDCP) >> 16);
		ch_info->mode= (Int32)((dev_info->mode) >> 16);
	}

	status = OSA_SOK;
	
	return status;
}

/*设置7441 cbcr偏色矫正，主要针对DVI信号*/
Int32 platform_vp_set_adv7441_cbcr(void *hand,Int32 value)
{
	Int32 status = OSA_SOK;

	return status;
}

/*设置7442 cbcr偏色矫正，主要针对DVI信号*/
Int32 platform_vp_set_adv7442_cbcr(void *hand,Int32 value)
{
	Int32 status = OSA_SOK;

	return status;
}

/*设置7844 cbcr偏色矫正，主要针对DVI信号*/
Int32 platform_vp_set_adv7844_cbcr(void *hand,Int32 value)
{
	Int32 status = OSA_SOK;

	return status;
}

/*设置7844_2 cbcr偏色矫正，主要针对DVI信号*/
Int32 platform_vp_set_adv7844_2_cbcr(void *hand,Int32 value)
{
	Int32 status = OSA_SOK;

	return status;
}

/*设置7441的VGA信号的黑边*/
Int32 platform_vp_set_adv7441_hv(void *hand,Int32 Hvalue,Int32 Vvalue)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Int16  h_pos = 0, v_pos = 0;
	Int32 pos = 0, new_pos = 0;

	if((ADV744 != s_capture_dev_type[0]) || (NULL == hand))
	{
		status = OSA_EFAIL;
		return status;
	}
	
	ret = cap_get_adv7441_HV(&hand, &pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
		return status;
	}
	
	h_pos = (pos & 0xFFFF0000) >> 16;
	v_pos = pos & 0xFFFF;
	new_pos = ((h_pos + Hvalue) << 16) | (v_pos + Vvalue);

	ret = cap_set_adv7441_HV(&hand, new_pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	if(Hvalue == 1 || Hvalue == -1) {
		ret = cap_invert_cbcr_adv7441_HV(&hand, 1);
		if(ret < 0)
		{
			status = OSA_EFAIL;
		}
	}
	
	return status;
}

/*设置7442的VGA信号的黑边*/
Int32 platform_vp_set_adv7442_hv(void *hand,Int32 Hvalue,Int32 Vvalue)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Int16  h_pos = 0, v_pos = 0;
	Int32 pos = 0, new_pos = 0;

	if((ADV744 != s_capture_dev_type[1]) || (NULL == hand))
	{
		status = OSA_EFAIL;
		return status;
	}
	
	ret = cap_get_adv7442_HV(&hand, &pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
		return status;
	}
	
	h_pos = (pos & 0xFFFF0000) >> 16;
	v_pos = pos & 0xFFFF;
	new_pos = ((h_pos + Hvalue) << 16) | (v_pos + Vvalue);

	ret = cap_set_adv7442_HV(&hand, new_pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	if(Hvalue == 1 || Hvalue == -1) {
		ret = cap_invert_cbcr_adv7442_HV(&hand, 1);
		if(ret < 0)
		{
			status = OSA_EFAIL;
		}
	}
	
	return status;
}

/*设置7844的VGA信号的黑边*/
Int32 platform_vp_set_adv7844_hv(void *hand,Int32 Hvalue,Int32 Vvalue)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Int16  h_pos = 0, v_pos = 0;
	Int32 pos = 0, new_pos = 0;

	if((ADV7844 != s_capture_dev_type[0]) || (NULL == hand))
	{
		status = OSA_EFAIL;
		return status;
	}
	
	ret = cap_get_adv7844_HV(&hand, &pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
		return status;
	}
	
	h_pos = (pos & 0xFFFF0000) >> 16;
	v_pos = pos & 0xFFFF;
	printf("==========platform_vp_set_adv7844_hv:pos:0x%x,h_pos:0x%x,v_pos:0x%x\n",pos,h_pos,v_pos);
	new_pos = ((h_pos + Hvalue) << 16) | (v_pos + Vvalue);
	printf("==========platform_vp_set_adv7844_hv:new_pos:0x%x,h_pos:0x%x,v_pos:0x%x\n",new_pos,Hvalue,Vvalue);
	ret = cap_set_adv7844_HV(&hand, new_pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	if(Hvalue == 1 || Hvalue == -1) {
		ret = cap_invert_cbcr_adv7844_HV(&hand, 1);
		if(ret < 0)
		{
			status = OSA_EFAIL;
		}
	}
	
	return status;
}

/*设置7844_2的VGA信号的黑边*/
Int32 platform_vp_set_adv7844_2_hv(void *hand,Int32 Hvalue,Int32 Vvalue)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Int16  h_pos = 0, v_pos = 0;
	Int32 pos = 0, new_pos = 0;

	if((ADV7844 != s_capture_dev_type[1]) || (NULL == hand))
	{
		status = OSA_EFAIL;
		return status;
	}
	
	ret = cap_get_adv7844_2_HV(&hand, &pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
		return status;
	}
	
	h_pos = (pos & 0xFFFF0000) >> 16;
	v_pos = pos & 0xFFFF;
	new_pos = ((h_pos + Hvalue) << 16) | (v_pos + Vvalue);

	ret = cap_set_adv7844_2_HV(&hand, new_pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}

	if(Hvalue == 1 || Hvalue == -1) {
		ret = cap_invert_cbcr_adv7844_2_HV(&hand, 1);
		if(ret < 0)
		{
			status = OSA_EFAIL;
		}
	}
	
	return status;
}

/*设置gs2971信号的黑边*/
Int32 platform_vp_set_gs2971_hv(void *hand,Int32 Hvalue,Int32 Vvalue)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Int16  h_pos = 0, v_pos = 0;
	Int32 pos = 0, new_pos = 0;

	if((GS297 != s_capture_dev_type[0]) || (NULL == hand))
	{
		status = OSA_EFAIL;
		return status;
	}
	
	ret = cap_get_gs2971_HV(&hand, &pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
		return status;
	}
	
	h_pos = (pos & 0xFFFF0000) >> 16;
	v_pos = pos & 0xFFFF;
	new_pos = ((h_pos + Hvalue) << 16) | (v_pos + Vvalue);

	ret = cap_set_gs2971_HV(&hand, new_pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	
	return status;
}

/*设置gs2972信号的黑边*/
Int32 platform_vp_set_gs2972_hv(void *hand,Int32 Hvalue,Int32 Vvalue)
{
	Int32 ret = 0;
	Int32 status = OSA_SOK;
	Int16  h_pos = 0, v_pos = 0;
	Int32 pos = 0, new_pos = 0;

	if((GS297 != s_capture_dev_type[1]) || (NULL == hand))
	{
		status = OSA_EFAIL;
		return status;
	}
	
	ret = cap_get_gs2972_HV(&hand, &pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
		return status;
	}
	
	h_pos = (pos & 0xFFFF0000) >> 16;
	v_pos = pos & 0xFFFF;
	new_pos = ((h_pos + Hvalue) << 16) | (v_pos + Vvalue);

	ret = cap_set_gs2972_HV(&hand, new_pos);
	if(ret < 0)
	{
		status = OSA_EFAIL;
	}
	
	return status;
}

/*设置HDMI输出的mode*/
Int32 platform_display_set_hdmi_mode(Int32 mode)
{
	dis_stop_drv(SYSTEM_LINK_ID_DISPLAY_0);
	dis_set_resolution(SYSTEM_LINK_ID_DISPLAY_0, mode);
	dis_start_drv(SYSTEM_LINK_ID_DISPLAY_0);

	return OSA_SOK;
}

/*设置DVO输出的mode*/
Int32 platform_display_set_Dvo_mode(Int32 mode)
{
	dis_stop_drv(SYSTEM_LINK_ID_DISPLAY_1);
	dis_set_resolution(SYSTEM_LINK_ID_DISPLAY_1, mode);
	dis_start_drv(SYSTEM_LINK_ID_DISPLAY_1);

	return OSA_SOK;
}

/*设置audio的音频采样率*/
Int32 platform_audio_set_capture_samplerate(void *hand,Int32 rate)
{
	//Int32 ret = 0;
	Int32 status = OSA_SOK;
	//Int8 device_name[64] = {0};
	audio_capture_handle *pacaphandle;

	if(NULL == hand)
	{
		status = OSA_EFAIL;
		return status;
	}

	pacaphandle = (audio_capture_handle *)hand;

	if(pacaphandle->create_params.samplerate != rate)
	{
		pacaphandle->create_params.samplerate = rate;
		pacaphandle->updateparam = 1;
#if 0		
		if(pacaphandle->create_params.device_name)
		{
			strcpy(device_name, pacaphandle->create_params.device_name);
		}
		else
		{
			OSA_printf("platform_audio_set_capture_samplerate:device_name is NULL\n");
			status = OSA_EFAIL;
			return status;	
		}

		if(pacaphandle->phandle_in)
		{
			snd_pcm_close(pacaphandle->phandle_in);
			pacaphandle->phandle_in = NULL;
		}

		usleep(100000);

		ret = snd_pcm_open(&(pacaphandle->phandle_in), device_name, SND_PCM_STREAM_CAPTURE, 0);
		if(ret < 0){
			OSA_printf("platform_audio_set_capture_samplerate: snd_pcm_open error: %s\n", snd_strerror(ret));	
			status = OSA_EFAIL;
		}

		OSA_printf("pacaphandle->phandle_i =%p=%s\n",pacaphandle->phandle_in,device_name);
		ret = hwparam_init(&(pacaphandle->create_params), &(pacaphandle->phandle_in), &(pacaphandle->params_in));
		if(ret < 0){
			OSA_printf("platform_audio_set_capture_samplerate: hwparam_init error: %s\n", snd_strerror(ret));	
			status = OSA_EFAIL;
		}
#endif
	}
	
	return status;
}

/*获取audio的音频采样率*/
Int32 platform_audio_get_capture_samplerate(void *hand,Int32 *rate)
{
	//Int32 ret = 0;
	Int32 status = OSA_SOK;
	audio_capture_handle *pacaphandle;

	if((NULL == hand) || (NULL == rate))
	{
		status = OSA_EFAIL;
		return status;
	}

	pacaphandle = (audio_capture_handle *)hand;

	*rate = pacaphandle->create_params.samplerate;

	return status;
}

/*设置音频大小*/
Int32 platform_audio_set_capture_volume(int chId,Int32 LVolume,Int32 RVolume)
{
	int l_value = 0;
	int r_value = 0;

	if(0 == LVolume) {
		l_value = 0;
	} else {
		l_value = 100 + 10 * (LVolume / 3);
	}

	if(0 == RVolume) {
		r_value = 0;
	} else {
		r_value = 100 + 10 * (RVolume / 3);
	}

	OSA_printf("platform_audio_set_capture_volume:num:[%d] LVolume:[%d] RVolume:[%d]\n", chId, LVolume, RVolume);
	int card = chId;

	if(0 == chId) {
		card = 1;
	} else if(1 == chId) {
		card = 0;
	}

	char cmd[256] = {0};
	sprintf(cmd, "amixer -c %d cset numid=5,iface=MIXER,name='PCM Volume' %d ; amixer -c %d cset numid=6,iface=MIXER,name='L ADC VOLUME' %d", card, r_value, card, l_value);
	system(cmd);

	OSA_printf("platform_audio_set_capture_volume:num=%d,card=%d,RVolume=%d,LVolume=%d\n", chId, card, r_value, l_value);
	return OSA_SOK;
}

/*从MODEID获取signal_info*/
Int32 platform_signal_info_from_modeId(Int32 modeId,Signal_Table *psignalInfo)
{
	if((modeId < 0) || (modeId > DEVICE_STD_REACH_LAST))
	{
		return OSA_EFAIL;
	}

	*psignalInfo = s_signal_table[modeId];
	
	return OSA_SOK;
}


