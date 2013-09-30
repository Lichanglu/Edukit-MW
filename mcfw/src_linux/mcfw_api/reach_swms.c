#include "reach_system_priv.h"
#include "reach_swms_priv.h"



Void swms_init_create_param(SwMsLink_CreateParams *prm)
{
	return SwMsLink_CreateParams_Init(prm);
}


Int32 swms_set_layout(Uint32 swMsId, SwMsLink_LayoutPrm *playout)
{
	Int32 status = -1;
	
	if(swMsId != SYSTEM_LINK_ID_INVALID && playout != NULL)
	{
		status = System_linkControl(swMsId,
		                            SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT,
		                            playout,
		                            sizeof(*playout),
		                            TRUE
		                           );
	}
	
	return status;
}

Int32 swms_get_layout(Uint32 swMsId, SwMsLink_LayoutPrm *playout)
{
	Int32 status = -1;
	
	if(swMsId != SYSTEM_LINK_ID_INVALID && playout != NULL)
	{
		status = System_linkControl(swMsId,
		                            SYSTEM_SW_MS_LINK_CMD_GET_LAYOUT_PARAMS,
		                            playout,
		                            sizeof(*playout),
		                            TRUE
		                           );
	}
	
	return status;
}

Int32 swms_get_input_channel_info(Uint32 swMsId, SwMsLink_WinInfo pinfo)
{
	Int32 status = -1;
	SwMsLink_WinInfo inputInfo;
	
	if(swMsId != SYSTEM_LINK_ID_INVALID)
	{
		status = System_linkControl(swMsId,
		                            SYSTEM_SW_MS_LINK_CMD_GET_INPUT_CHNL_INFO,
		                            &(inputInfo),
		                            sizeof(inputInfo),
		                            TRUE);
	}
	
	return status;
}

Int32 swms_print_statics(Uint32 swMsId)
{
	Int32 status = -1;
	
	if(swMsId != SYSTEM_LINK_ID_INVALID)
	{
		status = System_linkControl(swMsId,
		                            SYSTEM_SW_MS_LINK_CMD_PRINT_STATISTICS,
		                            NULL,
		                            0,
		                            TRUE
		                           );
	}
	
	return status;
}

Int32 swms_print_bufferstatics(Uint32 swMsId)
{
	Int32 status = -1;
	
	if(swMsId != SYSTEM_LINK_ID_INVALID)
	{
		status = System_linkControl(swMsId,
		                            SYSTEM_SW_MS_LINK_CMD_PRINT_BUFFER_STATISTICS,
		                            NULL,
		                            0,
		                            TRUE
		                           );
	}
	
	return status;
}


void swms_set_default_mosaic_param(VDIS_MOSAIC_S *vdMosaicParam)
{
	UInt32 outWidth, outHeight, winId, i;
	
	
	outWidth = 1920;
	outHeight = 1080;
	
	
	vdMosaicParam->outputFPS = 30;
	
	vdMosaicParam->onlyCh2WinMapChanged = FALSE;
	
	vdMosaicParam->displayWindow.height 	= outHeight;
	vdMosaicParam->displayWindow.width		= outWidth;
	vdMosaicParam->displayWindow.start_X	= 0;
	vdMosaicParam->displayWindow.start_Y	= 0;
	
	
	for (i = 0; i < VDIS_MOSAIC_WIN_MAX; i++)
	{
		vdMosaicParam->chnMap[i] = VDIS_CHN_INVALID;
	}
	
	vdMosaicParam->numberOfWindows = 1;
	
	winId = 0;
	
	vdMosaicParam->winList[winId].start_X = 0;
	vdMosaicParam->winList[winId].start_Y = 0;
	vdMosaicParam->winList[winId].width = outWidth;
	vdMosaicParam->winList[winId].height = outHeight;
	
	vdMosaicParam->chnMap[winId] = winId;
	
	vdMosaicParam->useLowCostScaling[winId] = 0;
	
	vdMosaicParam->chnMap[winId] = (vdMosaicParam->chnMap[winId]);
	
}

void swms_set_default_mosaic_param_720P(VDIS_MOSAIC_S *vdMosaicParam)
{
	UInt32 outWidth, outHeight, winId, i;
	
	
	outWidth = 1280;
	outHeight = 720;
	
	
	vdMosaicParam->outputFPS = 30;
	
	vdMosaicParam->onlyCh2WinMapChanged = FALSE;
	
	vdMosaicParam->displayWindow.height 	= outHeight;
	vdMosaicParam->displayWindow.width		= outWidth;
	vdMosaicParam->displayWindow.start_X	= 0;
	vdMosaicParam->displayWindow.start_Y	= 0;
	
	
	for (i = 0; i < VDIS_MOSAIC_WIN_MAX; i++)
	{
		vdMosaicParam->chnMap[i] = VDIS_CHN_INVALID;
	}
	
	vdMosaicParam->numberOfWindows = 1;
	
	winId = 0;
	
	vdMosaicParam->winList[winId].start_X = 0;
	vdMosaicParam->winList[winId].start_Y = 0;
	vdMosaicParam->winList[winId].width = outWidth;
	vdMosaicParam->winList[winId].height = outHeight;
	
	vdMosaicParam->chnMap[winId] = winId;
	
	vdMosaicParam->useLowCostScaling[winId] = 0;
	
	vdMosaicParam->chnMap[winId] = (vdMosaicParam->chnMap[winId]);
	
}

Int32 getoutsize(UInt32 outRes, UInt32 * width, UInt32 * height)
{
	switch (outRes)
	{
		case VSYS_STD_3840x2400_60:
			*width = 3840;
			*height = 2400;
			break;
			
		case VSYS_STD_3840x1200_60:
			*width = 3840;
			*height = 1200;
			break;
		case VSYS_STD_SXGAP_60:
			*width = 1400;
			*height = 1050;
			break;
			
        case VSYS_STD_1440_900_60:
               *width = 1440;
               *height = 900;
               break;	
			   
		case VSYS_STD_1920x1200_60:
			*width = 1920;
			*height = 1200;
			break;
			
		case VSYS_STD_SXGA_60:
			*width = 1280;
			*height = 1024;
			break;
		case VSYS_STD_1280x800_60:
			*width = 1280;
			*height = 800;
			break;
		case VSYS_STD_720P_60:
			*width = 1280;
			*height = 720;
			break;
		case VSYS_STD_XGA_60:
			*width = 1024;
			*height = 768;
			break;
		default
				:
		case VSYS_STD_1080I_60:
		case VSYS_STD_1080P_60:
		case VSYS_STD_1080P_30:
			*width = 1920;
			*height = 1080;
			break;
			
		case VSYS_STD_NTSC:
			*width = 720;
			*height = 480;
			break;
			
		case VSYS_STD_PAL:
			*width = 720;
			*height = 576;
			break;
				
	}
	return 0;
}


Void swms_set_swms_layout(UInt32 devId, SwMsLink_CreateParams *swMsCreateArgs)
{
	SwMsLink_LayoutPrm *layoutInfo;
	SwMsLink_LayoutWinInfo *winInfo;
	UInt32 outWidth, outHeight, row, col, winId, widthAlign, heightAlign;
	UInt32 outputfps;
	
	getoutsize(swMsCreateArgs->maxOutRes, &outWidth, &outHeight);
	
	widthAlign = 8;
	heightAlign = 1;
	
	if(devId>1)
		devId = 0;
		
	layoutInfo = &swMsCreateArgs->layoutPrm;
	outputfps = layoutInfo->outputFPS;
	
	memset(layoutInfo, 0, sizeof(*layoutInfo));
	layoutInfo->onlyCh2WinMapChanged = FALSE;
	layoutInfo->outputFPS = outputfps;
	
	layoutInfo->numWin = 1;
	
	for(row=0; row<2; row++)
	{
		for(col=0; col<2; col++)
		{
			winId = row*2+col;
			winInfo = &layoutInfo->winInfo[winId];
			
			winInfo->width	= SystemUtils_align((outWidth*2)/5, widthAlign);
			winInfo->height = SystemUtils_align(outHeight/2, heightAlign);
			winInfo->startX = winInfo->width*col;
			winInfo->startY = winInfo->height*row;
			
			
			winInfo->width	= 1920;
			winInfo->height = 1080;
			winInfo->startX = 0;
			winInfo->startY = 0;
			
			winInfo->bypass = TRUE;
			winInfo->channelNum = devId*SYSTEM_SW_MS_MAX_WIN + winId;
		}
	}
	
	for(row=0; row<4; row++)
	{
		winId = 4 + row;
		winInfo = &layoutInfo->winInfo[winId];
		
		winInfo->width	= layoutInfo->winInfo[0].width/2;
		winInfo->height = layoutInfo->winInfo[0].height/2;
		winInfo->startX = layoutInfo->winInfo[0].width*2;
		winInfo->startY = winInfo->height*row;
		
		winInfo->width	= 1920;
		winInfo->height = 1080;
		winInfo->startX = 0;
		winInfo->startY = 0;
		
		winInfo->bypass = TRUE;
		winInfo->channelNum = devId*SYSTEM_SW_MS_MAX_WIN + winId;
	}
}

Void swMsGenerateLayoutParams(UInt32 layoutId,
                                     SwMsLink_CreateParams *swMsCreateArgs)
{
    SwMsLink_LayoutPrm *layoutInfo;
    SwMsLink_LayoutWinInfo *winInfo;
    UInt32 outWidth, outHeight, row, col, winId, widthAlign, heightAlign;
    UInt32 SwMsOutputFps;

    getoutsize(swMsCreateArgs->maxOutRes, &outWidth, &outHeight);

    widthAlign = 8;
    heightAlign = 1;

    layoutInfo = &swMsCreateArgs->layoutPrm;
    /* store the SwMs output fps locally */
    SwMsOutputFps = layoutInfo->outputFPS;
    /* init to known default */
    memset(layoutInfo, 0, sizeof(*layoutInfo));

    layoutInfo->onlyCh2WinMapChanged = FALSE;
    /* restore the value OR
       Modify with new value if required */
    layoutInfo->outputFPS = SwMsOutputFps;


    if(layoutId == 0)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_2X2_PLUS_4CH\n");

        layoutInfo->numWin = 8;

        for(row=0; row<2; row++)
        {
            for(col=0; col<2; col++)
            {
                winId = row*2+col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width  = SystemUtils_align((outWidth*2)/5, widthAlign);
                winInfo->height = SystemUtils_align(outHeight/2, heightAlign);
                winInfo->startX = winInfo->width*col;
                winInfo->startY = winInfo->height*row;
                winInfo->bypass = TRUE;
                winInfo->channelNum =  winId;
            }
        }

        for(row=0; row<4; row++)
        {
            winId = 4 + row;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width  = layoutInfo->winInfo[0].width/2;
            winInfo->height = layoutInfo->winInfo[0].height/2;
            winInfo->startX = layoutInfo->winInfo[0].width*2;
            winInfo->startY = winInfo->height*row;
            winInfo->bypass = TRUE;
            winInfo->channelNum =  winId;
        }
    }

    if(layoutId == 1)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_2X2\n");

        layoutInfo->numWin    = 4;

        for(row=0; row<2; row++)
        {
            for(col=0; col<2; col++)
            {
                winId = row*2+col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width  = SystemUtils_align(outWidth/2, widthAlign);
                winInfo->height = SystemUtils_align(outHeight/2, heightAlign);
                winInfo->startX = winInfo->width*col;
                winInfo->startY = winInfo->height*row;
                winInfo->bypass = TRUE;
                winInfo->channelNum = winId;
            }
        }
    }

    if(layoutId == 2)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_1x1\n");

        layoutInfo->numWin    = 1;

        winId = 0;

        winInfo = &layoutInfo->winInfo[winId];

        winInfo->startX = 0;
        winInfo->startY = 0;
        winInfo->width  = outWidth;
        winInfo->height = outHeight;
        winInfo->bypass = TRUE;
        winInfo->channelNum =  winId;
    }

    if(layoutId == 3)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_1x1_PLUS_2PIP\n");

        layoutInfo->numWin = 3;

        winId = 0;

        winInfo = &layoutInfo->winInfo[winId];

        winInfo->startX = 0;
        winInfo->startY = 0;
        winInfo->width  = outWidth;
        winInfo->height = outHeight;
        winInfo->bypass = TRUE;
        winInfo->channelNum = winId;

        for(col=0; col<2; col++)
        {
            winId = 1 + col;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width  = SystemUtils_align(outWidth/4, widthAlign);
            winInfo->height = SystemUtils_align(outHeight/4, heightAlign);

            if(col==0)
            {
                winInfo->startX = SystemUtils_align(outWidth/20, widthAlign);
            }
            else
            {
                winInfo->startX = SystemUtils_align(
                    outWidth - winInfo->width - outWidth/20,
                    widthAlign);
            }

            winInfo->startY = SystemUtils_align(
                            outHeight - winInfo->height - outHeight/20,
                            heightAlign
                            );
            winInfo->bypass = TRUE;
            winInfo->channelNum = winId;
        }
    }

    if(layoutId == 4)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_3X3\n");

        layoutInfo->numWin    = 9;

        for(row=0; row<3; row++)
        {
            for(col=0; col<3; col++)
            {
                winId = row*3+col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width  = SystemUtils_align(outWidth/3, widthAlign);
                winInfo->height = SystemUtils_align(outHeight/3, heightAlign);
                winInfo->startX = winInfo->width*col;
                winInfo->startY = winInfo->height*row;
                winInfo->bypass = TRUE;
                winInfo->channelNum =  winId;
            }
        }

    }

    if(layoutId == 5)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_4X4\n");

        layoutInfo->numWin    = 16;

        for(row=0; row<4; row++)
        {
            for(col=0; col<4; col++)
            {
                winId = row*4+col;

                winInfo = &layoutInfo->winInfo[winId];

                winInfo->width  = SystemUtils_align(outWidth/4, widthAlign);
                winInfo->height = SystemUtils_align(outHeight/4, heightAlign);
                winInfo->startX = winInfo->width*col;
                winInfo->startY = winInfo->height*row;
                winInfo->bypass = TRUE;
                winInfo->channelNum =  winId;
            }
        }
    }

    if(layoutId == 6)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_5CH_PLUS_1CH\n");

        layoutInfo->numWin = 6;

        winId = 0;

        winInfo = &layoutInfo->winInfo[winId];

        winInfo->width  = SystemUtils_align((outWidth*2)/3, widthAlign);
        winInfo->height = SystemUtils_align((outHeight*2)/3, heightAlign);
        winInfo->startX = 0;
        winInfo->startY = 0;
        winInfo->bypass = TRUE;
        winInfo->channelNum =  winId;

        for(row=0; row<2; row++)
        {
            winId = 1 + row;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width  = SystemUtils_align(layoutInfo->winInfo[0].width/2, widthAlign);
            winInfo->height = SystemUtils_align(layoutInfo->winInfo[0].height/2, heightAlign);
            winInfo->startX = layoutInfo->winInfo[0].width;
            winInfo->startY = winInfo->height*row;
            winInfo->bypass = TRUE;
            winInfo->channelNum =  winId;

        }

        for(col=0; col<3; col++)
        {
            winId = 3 + col;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width  = SystemUtils_align(layoutInfo->winInfo[0].width/2, widthAlign);
            winInfo->height = SystemUtils_align(layoutInfo->winInfo[0].height/2, heightAlign);
            winInfo->startX = winInfo->width*col;
            winInfo->startY = layoutInfo->winInfo[0].height;
            winInfo->bypass = TRUE;
            winInfo->channelNum = winId;
        }

    }

    if(layoutId == 7)
    {
        printf("Display 0: Layout: CHAINS_LAYOUT_7CH_PLUS_1CH\n");
        layoutInfo->numWin = 8;

        winId = 0;

        winInfo = &layoutInfo->winInfo[winId];

        winInfo->width  = SystemUtils_align((outWidth)/4, widthAlign)*3;
        winInfo->height = SystemUtils_align((outHeight)/4, heightAlign)*3;
        winInfo->startX = 0;
        winInfo->startY = 0;
        winInfo->bypass = TRUE;
        winInfo->channelNum = winId;

        for(row=0; row<3; row++)
        {
            winId = 1 + row;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width  = SystemUtils_align(
                                outWidth - layoutInfo->winInfo[0].width,
                                widthAlign);
            winInfo->height = SystemUtils_align(
                                layoutInfo->winInfo[0].height / 3,
                                heightAlign);
            winInfo->startX = layoutInfo->winInfo[0].width;
            winInfo->startY = winInfo->height*row;
            winInfo->bypass = TRUE;
            winInfo->channelNum =  winId;

        }

        for(col=0; col<4; col++)
        {
            winId = 4 + col;

            winInfo = &layoutInfo->winInfo[winId];

            winInfo->width  = SystemUtils_align(outWidth / 4, widthAlign);
            winInfo->height = SystemUtils_align(
                                outHeight - layoutInfo->winInfo[0].height,
                                heightAlign);
            winInfo->startX = winInfo->width*col;
            winInfo->startY = layoutInfo->winInfo[0].height;
            winInfo->bypass = TRUE;
            winInfo->channelNum = winId;
        }
    }
}

Int32 swMsSwitchLayout(UInt32 swMsLinkId, SwMsLink_CreateParams *swMsPrm, UInt32 curLayoutId)
{
	/* If switching mosaic create entire Mosaic Layout */
	swMsGenerateLayoutParams(curLayoutId, swMsPrm);
	
	System_linkControl(swMsLinkId, SYSTEM_SW_MS_LINK_CMD_SWITCH_LAYOUT, &swMsPrm->layoutPrm, sizeof(swMsPrm->layoutPrm), TRUE);


	
	return OSA_SOK;
}

