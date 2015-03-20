/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_GV7601_H_
#define _DEVICEDRV_GV7601_H_

#include "../src/gv7601_priv.h"


Int32 Device_gv7601Init (  );
Int32 Device_gv7601DeInit (  );

Device_Gv7601Handle Device_gv7601Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_gv7601Delete ( Device_Gv7601Handle handle, Ptr deleteArgs );

Int32 Device_gv7601Control ( Device_Gv7601Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif

