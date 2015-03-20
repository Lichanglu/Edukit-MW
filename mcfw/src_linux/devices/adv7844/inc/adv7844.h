/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#ifndef _DEVICEDRV_ADV7844_H_
#define _DEVICEDRV_ADV7844_H_
#include "../src/adv7844_priv.h"


#include "../src/adv7844_priv.h"


Int32 Device_adv7844Init (  );
Int32 Device_adv7844DeInit (  );

Device_Adv7844Handle Device_adv7844Create ( UInt32 drvId,
                                UInt32 instanceId,
                                Ptr createArgs,
                                Ptr createStatusArgs);

Int32 Device_adv7844Delete ( Device_Adv7844Handle handle, Ptr deleteArgs );

Int32 Device_adv7844Control ( Device_Adv7844Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs );

#endif
