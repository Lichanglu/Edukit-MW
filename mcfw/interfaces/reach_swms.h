#ifndef __REACH_SWMS_H__
#define __REACH_SWMS_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"
#include "common_def/ti_vdis_common_def.h"

typedef struct _swms_struct_ {
	Uint32					link_id;
	SwMsLink_CreateParams	create_params;
}swms_struct;



Void swms_init_create_param(SwMsLink_CreateParams *prm);

Int32 swms_set_layout(Uint32 swmsId, SwMsLink_LayoutPrm *playout);

Int32 swms_get_layout(Uint32 swmsId, SwMsLink_LayoutPrm *playout);

Int32 swms_get_input_channel_info(Uint32 swmsId, SwMsLink_WinInfo pinfo);

Int32 swms_print_statics(Uint32 sclrId);

Int32 swms_print_bufferstatics(Uint32 swMsId);

void swms_set_default_mosaic_param(VDIS_MOSAIC_S *vdMosaicParam);

Void swms_set_swms_layout(UInt32 devId, SwMsLink_CreateParams *swMsCreateArgs);

Int32 getoutsize(UInt32 outRes, UInt32 * width, UInt32 * height);

Int32 swMsSwitchLayout(UInt32 swMsLinkId, SwMsLink_CreateParams *swMsPrm, UInt32 curLayoutId);

Int32 swMsSetInChInfo(UInt32 swMsLinkId, SwMsLink_InChInfo *Prm);

Int32 swMsSetAutoGetInChInfo(UInt32 swMsLinkId, Int32 chId);
#endif

