/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_GV7602_H_
#define _DEVICEDRV_GV7602_H_

#include "../src/gv7602_priv.h"


Int32 Device_gv7602Init (  );
Int32 Device_gv7602DeInit (  );

Device_Gv7602Handle Device_gv7602Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_gv7602Delete ( Device_Gv7602Handle handle, Ptr deleteArgs );

Int32 Device_gv7602Control ( Device_Gv7602Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif

