#ifndef _REACH_PLATFORM_COMMON_
#define _REACH_PLATFORM_COMMON_

#ifdef __cplusplus
extern "C" {
#endif
#include <osa_i2c.h>
#include <mcfw/src_linux/devices/inc/device.h>
#include <mcfw/src_linux/devices/adv7441/src/adv7441_priv.h>
#include <mcfw/src_linux/devices/adv7442/src/adv7442_priv.h>
#include <mcfw/src_linux/devices/gs2971/src/gs2971_priv.h>
#include <mcfw/src_linux/devices/gs2972/src/gs2972_priv.h>
#include <mcfw/src_linux/devices/adv7844/src/adv7844_priv.h>
#include <mcfw/src_linux/devices/adv7844_2/src/adv7844_2_priv.h>
#include <mcfw/src_linux/devices/gv7601/src/gv7601_priv.h>
#include <mcfw/src_linux/devices/gv7602/src/gv7602_priv.h>
#include "reach_system_priv.h"

/*设备类型*/
typedef enum{
	ADV744 = 0,
	ADV7844,
	GS297,
	GV760,
	MAX_DEV
}DEV_TYPE;

/*设备通道类型*/
typedef enum{
	CH_DVI = 0,
	CH_SDI,
	MAX_DEV_CH_TYPE
}DEV_CH_TYPE;

/*最大的设备数*/
#define MAXDEVNUM 2

/*每个设备最大的通道ID数*/
#define MAXCHNUMPERDEV 2

/*考虑是否整理mode表*/
typedef struct _Signal_Table_ {
	Vps_Adv7441_hpv_vps signal_info;
	UInt16 res;
	UInt16 digital_val;
	UInt16 analog_val;
} Signal_Table;

/*定义一个采集口可以获取的信息*/
typedef  struct VP_DEVICE_STATUS{
	Int32  mode ; //mode
	Int32  isVideoDetect;
	Int32  isInterlaced;
	Int32  isDigital;     // 1,digital  .0 ,模拟
	Int32  frameWidth;
	Int32  frameHeight;
	Int32  HDCP ; // 1:HDCP保护，0:非HDCP，sdi不关注这个信号
	Int32  frameInterval;
}VP_DEVICE_STATUS;

/*#################################通用接口####begin#########################################*/
/*获取FPGA版本号*/
Int32 platform_get_fpga_version(int devId);
/*获取内核版本号*/
Int32 platform_get_kernel_version(Int32 fd,Int8 *kernel_version);
/*更新FPGA程序*/
Int32 platform_update_fpga(Int32 fd,Int32 fpgaNo,const Int8 *file);
/*更新内核*/
Int32 platform_update_kernel(const Int8 *file);

/*设置VP口的采集为7441,7442*/
Device_Adv7441Handle platform_vp_select_adv7441(Int32 fd);
Device_Adv7442Handle platform_vp_select_adv7442(Int32 fd);

/*设置VP口的采集为gs2971,gs2972*/
Device_Gs2971Handle platform_vp_select_gs2971(Int32 fd);
Device_Gs2972Handle platform_vp_select_gs2972(Int32 fd);

/*设置VP口的采集为7844,7844_2*/
Device_Adv7844Handle platform_vp_select_adv7844(Int32 fd);
Device_Adv7844_2Handle platform_vp_select_adv7844_2(Int32 fd);

/*设置VP口的采集为7601,7602*/
Device_Gv7601Handle platform_vp_select_gv7601(Int32 fd);
Device_Gv7602Handle platform_vp_select_gv7602(Int32 fd);
/*设置采集7602的通道类型*/
Int32 platform_vp_set_gv7602_ch_type(void *hand,Int32 chId,DEV_CH_TYPE chType);
/*获取采集7602的通道类型*/
Int32 platform_vp_get_gv7602_ch_type(void *hand,Int32 chId,DEV_CH_TYPE *chType);

/*获取VP口的采集设备type*/
Int32 platform_vp_get_devtype(Int32 devId,Int32 *devType);


/*获取驱动的有源/无源,隔行/逐行，长宽信息*/
Int32 platform_vp_get_adv7441_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_adv7442_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_adv7844_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_adv7844_2_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gs2971_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gs2972_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gv7601_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gv7602_status(void *hand, VP_DEVICE_STATUS *info);
/*由dev信息获取通道信息*/
Int32 platform_vp_get_ch_status(Int32 chId, VP_DEVICE_STATUS *dev_info,VP_DEVICE_STATUS *ch_info);

/*设置cbcr偏色矫正，主要针对DVI信号*/
Int32 platform_vp_set_adv7441_cbcr(void *hand,Int32 value);
Int32 platform_vp_set_adv7442_cbcr(void *hand,Int32 value);
Int32 platform_vp_set_adv7844_cbcr(void *hand,Int32 value);
Int32 platform_vp_set_adv7844_2_cbcr(void *hand,Int32 value);

/*设置DVI接口的VGA信号的黑边*/
Int32 platform_vp_set_adv7441_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_adv7442_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_adv7844_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_adv7844_2_hv(void *hand,Int32 Hvalue,Int32 Vvalue);


/*设置SDI接口的左右黑边调整，非必备*/
Int32 platform_vp_set_gs2971_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_gs2972_hv(void *hand,Int32 Hvalue,Int32 Vvalue);

/*设置HDMI输出的mode*/
Int32 platform_display_set_hdmi_mode(Int32 mode);
/*设置DVO输出的mode*/
Int32 platform_display_set_Dvo_mode(Int32 mode);

/*设置audio的音频采样率*/
Int32 platform_audio_set_capture_samplerate(void *hand,Int32 rate);
/*获取audio的音频采样率*/
Int32 platform_audio_get_capture_samplerate(void *hand,Int32 *rate);
/*设置音频大小*/
Int32 platform_audio_set_capture_volume(int chId,Int32 LVolume,Int32 RVolume);

/*从MODEID获取signal_info*/
Int32 platform_signal_info_from_modeId(Int32 modeId,Signal_Table *psignalInfo);

/*#################################通用接口####end###########################################*/

#endif
