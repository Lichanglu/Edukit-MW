/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include "ti_media_std.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_gv7602.h>
#include <gv7602_priv.h>

/* Global object storing all information related to all
  GV7602 driver handles */

/* Control API that gets called when Device_control is called

  This API does handle level semaphore locking

  handle - GV7602 driver handle
  cmd - command
  cmdArgs - command arguments
  cmdStatusArgs - command status

  returns error in case of
  - illegal parameters
  - I2C command RX/TX error
*/
Int32 Device_gv7602Control ( Device_Gv7602Handle handle,
                           UInt32 cmd, Ptr cmdArgs, Ptr cmdStatusArgs )
{
	Device_Gv7602Obj *pObj = ( Device_Gv7602Obj * ) handle;
	Int32 status;

	if ( pObj == NULL )
		return -1;

	switch ( cmd )
	{
		case DEVICE_CMD_START:
			status = Device_gv7602Start ( pObj );
		break;

		case DEVICE_CMD_STOP:
			status = Device_gv7602Stop ( pObj );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_GET_CHIP_ID:
			status = Device_gv7602GetChipId ( pObj, cmdArgs, cmdStatusArgs );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_RESET:
			status = Device_gv7602Reset ( pObj );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_MODE:
			status = Device_gv7602SetVideoMode ( pObj, cmdArgs );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_GET_VIDEO_STATUS:
			status = Device_gv7602GetVideoStatus ( pObj, cmdArgs, cmdStatusArgs );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_SET_VIDEO_COLOR:
			status = Device_gv7602SetVideoColor ( pObj, cmdArgs );
		break;

		case IOCTL_DEVICE_GV7602_SET_VIDEO_NF:
			status = Device_gv7602SetVideoNf ( pObj, cmdArgs );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_REG_WRITE:
			status = Device_gv7602RegWrite ( pObj, cmdArgs );
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_REG_READ:
			status = Device_gv7602RegRead ( pObj, cmdArgs );
		break;

		//	case IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM:
		//           status = Device_gv7602GetExternInformation(pObj, cmdArgs);
		//            break;

		case IOCTL_DEVICE_VIDEO_DECODER_SET_DIRECTION:
			status = Device_gv7602SetDirection(pObj, cmdArgs);
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_GET_DIRECTION:
			status = Device_gv7602GetDirection(pObj, cmdArgs);
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_EXTERN_INFORM:
		case IOCTL_DEVICE_VIDEO_DECODER_3D_EXTERN_INFORM:
			status = Device_gv7602Get3DExternInformation(pObj, cmdArgs);
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_SET_3D_MODE:
			status = Device_gv7602Set3DMode(pObj, cmdArgs);
		break;

		case IOCTL_DEVICE_VIDEO_DECODER_GET_3D_MODE:
			status = Device_gv7602Get3DMode(pObj, cmdArgs);
		break;
		case IOCTL_DEVICE_VIDEO_DECODER_SET_BYPASS_CH:
			status =Device_gv7602SetByPassChannel(pObj, cmdArgs);
		break;
		case IOCTL_DEVICE_VIDEO_DECODER_GET_BYPASS_CH:
			status =Device_gv7602GetByPassChannel(pObj, cmdArgs);
		break;

		default:
			status = -1;
		break;
	}

	return status;
}

/*
  Create API that gets called when Device_create is called

  This API does not configure the GV7602 is any way.

  This API
  - validates parameters
  - allocates driver handle
  - stores create arguments in its internal data structure.

  Later the create arguments will be used when doing I2C communcation with
  GV7602

  drvId - driver ID, must be DEVICE_VID_DEC_GV7602_DRV
  instanceId - must be 0
  createArgs - create arguments
  createStatusArgs - create status
  fdmCbParams  - NOT USED, set to NULL

  returns NULL in case of any error
*/
Device_Gv7602Handle Device_gv7602Create ( UInt8 drvId,
                                            UInt8 instId,
                                            Ptr createArgs,
                                            Ptr createStatusArgs)
{
    Int32 status = 0;
    Device_Gv7602Obj *pObj;
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
         || vidDecCreateArgs->numDevicesAtPort > DEVICE_GV7602_DEV_MAX )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }

    pObj = (Device_Gv7602Obj *)malloc(sizeof(Device_Gv7602Obj));

    if ( pObj == NULL )
    {
        vidDecCreateStatus->retVal = -1;
        return NULL;
    }
    else
    {
        memset(pObj, 0, sizeof(Device_Gv7602Obj));

        gDevice_gv7602CommonObj.gv7602handle[instId] = pObj;

        /*
         * copy parameters to allocate driver handle
         */
        memcpy ( &pObj->createArgs, vidDecCreateArgs,
                 sizeof ( *vidDecCreateArgs ) );

        status = OSA_i2cOpen(&(gDevice_gv7602CommonObj.i2cHandle_0), 1);

        if ( status < 0)
        {
            vidDecCreateStatus->retVal = -1;
            return NULL;
        }

        status = OSA_i2cOpen(&(gDevice_gv7602CommonObj.i2cHandle_1), 2);

        if ( status < 0)
        {
            vidDecCreateStatus->retVal = -1;
            return NULL;
        }

	gDevice_gv7602CommonObj.GpioHandle = open("/dev/Rgpio",O_RDWR | O_SYNC);
        if ( gDevice_gv7602CommonObj.GpioHandle < 0)
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
Int32 Device_gv7602Delete ( Device_Gv7602Handle handle, Ptr deleteArgs )
{
    Device_Gv7602Obj *pObj = ( Device_Gv7602Obj * ) handle;

    if ( pObj == NULL )
        return -1;


    OSA_i2cClose(&(gDevice_gv7602CommonObj.i2cHandle_0));

    OSA_i2cClose(&(gDevice_gv7602CommonObj.i2cHandle_1));

    close(gDevice_gv7602CommonObj.GpioHandle);
	
    /*
     * free driver handle object
     */
    free(pObj);

    return 0;
}

/*
  System init for GV7602 driver

  This API
  - create semaphore locks needed
  - registers driver to FVID2 sub-system
  - gets called as part of Device_deviceInit()

*/
Int32 Device_gv7602Init (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    memset ( &gDevice_gv7602CommonObj, 0, sizeof ( gDevice_gv7602CommonObj ) );

    return 0;
}

Int32 Device_gv7602DeInit (  )
{
    /*
     * Set to 0's for global object, descriptor memory
     */
    return 0;
}

