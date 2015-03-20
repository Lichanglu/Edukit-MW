#include "reach_system_priv.h"
#include "reach_dup_priv.h"



Void dup_init_create_param(DupLink_CreateParams *prm)
{
	return DupLink_CreateParams_Init(prm);
}


Int32 dup_set_chInfo(Uint32 dupLinkId, System_dupChInfo *prm)
{
	Int32 status = -1;
		
	if(dupLinkId != SYSTEM_LINK_ID_INVALID) {
		status = System_linkControl(dupLinkId,
								DUP_LINK_CMD_SET_INCHAN_INFO,
	                                           	prm,
	                                           	sizeof(*prm),
	                                           	TRUE
	                                           	);
	}

	return status;
}




