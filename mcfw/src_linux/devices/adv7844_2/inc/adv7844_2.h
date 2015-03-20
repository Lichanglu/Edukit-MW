/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_ADV7844_H_
#define _DEVICEDRV_ADV7844_H_
#include "../src/adv7844_2_priv.h"


#include "../src/adv7844_2_priv.h"


Int32 Device_adv7844_2Init (  );
Int32 Device_adv7844_2DeInit (  );

Device_Adv7844_2Handle Device_adv7844_2Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_adv7844_2Delete ( Device_Adv7844_2Handle handle, Ptr deleteArgs );

Int32 Device_adv7844_2Control ( Device_Adv7844_2Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif
