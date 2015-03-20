#ifndef __REACH_DEC_H__
#define __REACH_DEC_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "reach_system.h"
#include "common_def/ti_vcap_common_def.h"


typedef struct _dec_struct_ {
	Uint32					link_id;
	DecLink_CreateParams	create_params;
}dec_struct;

Void dec_init_create_param(DecLink_CreateParams *prm);

Int32 dec_print_ivahd_statistics(Uint32 decId);

Int32 dec_print_statistics(Uint32 decId);

Int32 dec_print_buffer_statistics(Uint32 decId);

Int32 dec_set_trickplayconfig(Uint32 decId, Uint32 chId, Uint32 inputfps, Uint32 targetfps);

Int32 dec_disable_channel(Uint32 decId, Uint32 chId);

Int32 dec_enable_channel(Uint32 decId, Uint32 chId);

#endif

