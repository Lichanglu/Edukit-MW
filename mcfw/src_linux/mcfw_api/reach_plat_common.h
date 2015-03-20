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

/*�豸����*/
typedef enum{
	ADV744 = 0,
	ADV7844,
	GS297,
	GV760,
	MAX_DEV
}DEV_TYPE;

/*�豸ͨ������*/
typedef enum{
	CH_DVI = 0,
	CH_SDI,
	MAX_DEV_CH_TYPE
}DEV_CH_TYPE;

/*�����豸��*/
#define MAXDEVNUM 2

/*ÿ���豸����ͨ��ID��*/
#define MAXCHNUMPERDEV 2

/*�����Ƿ�����mode��*/
typedef struct _Signal_Table_ {
	Vps_Adv7441_hpv_vps signal_info;
	UInt16 res;
	UInt16 digital_val;
	UInt16 analog_val;
} Signal_Table;

/*����һ���ɼ��ڿ��Ի�ȡ����Ϣ*/
typedef  struct VP_DEVICE_STATUS{
	Int32  mode ; //mode
	Int32  isVideoDetect;
	Int32  isInterlaced;
	Int32  isDigital;     // 1,digital  .0 ,ģ��
	Int32  frameWidth;
	Int32  frameHeight;
	Int32  HDCP ; // 1:HDCP������0:��HDCP��sdi����ע����ź�
	Int32  frameInterval;
}VP_DEVICE_STATUS;

/*#################################ͨ�ýӿ�####begin#########################################*/
/*��ȡFPGA�汾��*/
Int32 platform_get_fpga_version(int devId);
/*��ȡ�ں˰汾��*/
Int32 platform_get_kernel_version(Int32 fd,Int8 *kernel_version);
/*����FPGA����*/
Int32 platform_update_fpga(Int32 fd,Int32 fpgaNo,const Int8 *file);
/*�����ں�*/
Int32 platform_update_kernel(const Int8 *file);

/*����VP�ڵĲɼ�Ϊ7441,7442*/
Device_Adv7441Handle platform_vp_select_adv7441(Int32 fd);
Device_Adv7442Handle platform_vp_select_adv7442(Int32 fd);

/*����VP�ڵĲɼ�Ϊgs2971,gs2972*/
Device_Gs2971Handle platform_vp_select_gs2971(Int32 fd);
Device_Gs2972Handle platform_vp_select_gs2972(Int32 fd);

/*����VP�ڵĲɼ�Ϊ7844,7844_2*/
Device_Adv7844Handle platform_vp_select_adv7844(Int32 fd);
Device_Adv7844_2Handle platform_vp_select_adv7844_2(Int32 fd);

/*����VP�ڵĲɼ�Ϊ7601,7602*/
Device_Gv7601Handle platform_vp_select_gv7601(Int32 fd);
Device_Gv7602Handle platform_vp_select_gv7602(Int32 fd);
/*���òɼ�7602��ͨ������*/
Int32 platform_vp_set_gv7602_ch_type(void *hand,Int32 chId,DEV_CH_TYPE chType);
/*��ȡ�ɼ�7602��ͨ������*/
Int32 platform_vp_get_gv7602_ch_type(void *hand,Int32 chId,DEV_CH_TYPE *chType);

/*��ȡVP�ڵĲɼ��豸type*/
Int32 platform_vp_get_devtype(Int32 devId,Int32 *devType);


/*��ȡ��������Դ/��Դ,����/���У�������Ϣ*/
Int32 platform_vp_get_adv7441_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_adv7442_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_adv7844_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_adv7844_2_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gs2971_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gs2972_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gv7601_status(void *hand, VP_DEVICE_STATUS *info);
Int32 platform_vp_get_gv7602_status(void *hand, VP_DEVICE_STATUS *info);
/*��dev��Ϣ��ȡͨ����Ϣ*/
Int32 platform_vp_get_ch_status(Int32 chId, VP_DEVICE_STATUS *dev_info,VP_DEVICE_STATUS *ch_info);

/*����cbcrƫɫ��������Ҫ���DVI�ź�*/
Int32 platform_vp_set_adv7441_cbcr(void *hand,Int32 value);
Int32 platform_vp_set_adv7442_cbcr(void *hand,Int32 value);
Int32 platform_vp_set_adv7844_cbcr(void *hand,Int32 value);
Int32 platform_vp_set_adv7844_2_cbcr(void *hand,Int32 value);

/*����DVI�ӿڵ�VGA�źŵĺڱ�*/
Int32 platform_vp_set_adv7441_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_adv7442_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_adv7844_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_adv7844_2_hv(void *hand,Int32 Hvalue,Int32 Vvalue);


/*����SDI�ӿڵ����Һڱߵ������Ǳر�*/
Int32 platform_vp_set_gs2971_hv(void *hand,Int32 Hvalue,Int32 Vvalue);
Int32 platform_vp_set_gs2972_hv(void *hand,Int32 Hvalue,Int32 Vvalue);

/*����HDMI�����mode*/
Int32 platform_display_set_hdmi_mode(Int32 mode);
/*����DVO�����mode*/
Int32 platform_display_set_Dvo_mode(Int32 mode);

/*����audio����Ƶ������*/
Int32 platform_audio_set_capture_samplerate(void *hand,Int32 rate);
/*��ȡaudio����Ƶ������*/
Int32 platform_audio_get_capture_samplerate(void *hand,Int32 *rate);
/*������Ƶ��С*/
Int32 platform_audio_set_capture_volume(int chId,Int32 LVolume,Int32 RVolume);

/*��MODEID��ȡsignal_info*/
Int32 platform_signal_info_from_modeId(Int32 modeId,Signal_Table *psignalInfo);

/*#################################ͨ�ýӿ�####end###########################################*/

#endif
