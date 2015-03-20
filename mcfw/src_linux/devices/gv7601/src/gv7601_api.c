/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/


#include "ti_media_std.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_gv7601.h>
#include <gv7601_priv.h>

/* Global object storing all information related to all
  GV7601 driver handles */

/* Control API that gets called when Device_control is called

  This API does handle level semaphore locking

  handle - GV7601 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Device_gv7601Control ( Device_Gv7601Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
    Device_Gv7601Obj *pObj = ( Device_Gv7601Obj * ) handle;
    Int32 status;

    if ( pObj == NULL )
        return -1;

    switch ( cmd )
    {
        case DEVICE_CMD_START:
            status = Device_gv7601Start ( pObj );
            break;

        case DEVICE_CMD_STOP:
            status = Device_gv7601Stop ( pObj );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_CHIP_ID:
            status = Device_gv7601GetChipId ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_RESET:
            status = Device_gv7601Reset ( pObj );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_MODE:
            status = Device_gv7601SetVideoMode ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS:
            status = Device_gv7601GetVideoStatus ( pObj, cmdArgs, cmdStatusArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_COLOR:
            status = Device_gv7601SetVideoColor ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_GV7601_SET_VIDEO_NF:
            status = Device_gv7601SetVideoNf ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_REG_WRITE:
            status = Device_gv7601RegWrite ( pObj, cmdArgs );
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_REG_READ:
            status = Device_gv7601RegRead ( pObj, cmdArgs );
            break;
			
//	case IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM:
//           status = Device_gv7601GetExternInformation(pObj, cmdArgs);
//            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_SET_DIRECTION:
            status = Device_gv7601SetDirection(pObj, cmdArgs);
            break;
			
        case IOCTL_DEVICE_VIDEO_DECODER_GET_DIRECTION:
            status = Device_gv7601GetDirection(pObj, cmdArgs);
            break;

	case IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM:
        case IOCTL_DEVICE_VIDEO_DECODER_3D_EXTERN_INFORM:
            status = Device_gv7601Get3DExternInformation(pObj, cmdArgs);
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_SET_3D_MODE:
            status = Device_gv7601Set3DMode(pObj, cmdArgs);
            break;

        case IOCTL_DEVICE_VIDEO_DECODER_GET_3D_MODE:
            status = Device_gv7601Get3DMode(pObj, cmdArgs);
            break;
			
       default:
            status = -1;
            break;
    }

    return status;
}

/*
  Create API that gets called when Device_create is called

  This API does not configure the GV7601 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  GV7601

  drvId - driver ID, must be DEVICE_VID_DEC_GV7601_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Device_Gv7601Handle Device_gv7601Create ( UInt8 drvId,
                                            UInt8 instId,
                                            Ptr createArgs,
                                            Ptr createStatusArgs)
{
    Int32 status = 0;
    Device_Gv7601Obj *pObj;
    Device_VideoDecoderCreateParams *vidDecCreateArgs
        = ( Device_VideoDecoderCreateParams * ) createArgs;

    Device_VideoDecoderCreateStatus *vidDecCreateStatus
        = ( Device_VideoDecoderCreateStatus * ) createStatusArgs;

    /*
     * check parameters
     */
    if ( vidDecCreateStatus == NULL )
        return NULL;

    vidDecCreateStatus->retVal = 0;

    if ( vidDecCreateArgs->deviceI2cInstId >= DEVICE_I2C_INST_ID_MAX
         || vidDecCreateArgs->numDevicesAtPort > DEVICE_GV7601_DEV_MAX )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }

    pObj = (Device_Gv7601Obj *)malloc(sizeof(Device_Gv7601Obj));

    if ( pObj == NULL )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }
    else
    {
        memset(pObj, 0, sizeof(Device_Gv7601Obj));

        gDevice_gv7601CommonObj.gv7601handle[instId] = pObj;

        /*
         * copy parameters to allocate driver handle
         */
        memcpy ( &pObj->createArgs, vidDecCreateArgs,
                 sizeof ( *vidDecCreateArgs ) );

        status = OSA_i2cOpen(&(gDevice_gv7601CommonObj.i2cHandle), I2C_DEFAULT_INST_ID);

        if ( status < 0)
        {
            vidDecCreateStatus->retVal = -1;
            return NULL;
        }
    }
    /*
     * return driver handle object pointer
     */
	
    return pObj;
}

/*
  Delete function that is called when Device_delete is called

  This API
  - free's driver handle object

  handle - driver handle
  deleteArgs - NOT USED, set to NULL

*/
Int32 Device_gv7601Delete ( Device_Gv7601Handle handle, Ptr deleteArgs )
{
    Device_Gv7601Obj *pObj = ( Device_Gv7601Obj * ) handle;

    if ( pObj == NULL )
        return -1;


    OSA_i2cClose(&(gDevice_gv7601CommonObj.i2cHandle));


    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}

/*
  System init for GV7601 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Device_deviceInit()

*/
Int32 Device_gv7601Init (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gDevice_gv7601CommonObj, 0, sizeof ( gDevice_gv7601CommonObj ) );

    return 0;
}

Int32 Device_gv7601DeInit (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    return 0;
}

