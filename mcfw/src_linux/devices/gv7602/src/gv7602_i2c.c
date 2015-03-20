/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/
#include <sys/ioctl.h>
#include "ti_media_std.h"
#include "ti_vsys_common_def.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_gv7602.h>
#include <gv7602_priv.h>
#include <osa_i2c.h>

#define	DEVICE_GV7602_DEBUG

enum
{
	GV7602_CH0=0,
	GV7602_CH1,
	GV7602_CH2,
};

struct R_GPIO_data{
	unsigned int gpio_num;
	unsigned int gpio_value;
};

#define FXTAL						28636360
#define RETRYNUMS					1

static int gGV7602_CH0_Lps = -1;
static int gGV7602_CH0_Mode=-1;
static int gGV7602_CH0_TMDS = 1;
static int gGV7602_CH0_Hpv = 0;
static int gGV7602_CH0_Vps = 0;
//static int gGV7602_CH0_CurStatus=-1;
static int gGV7602_CH1_Lps = -1;
static int gGV7602_CH1_Mode=-1;
static int gGV7602_CH1_TMDS = 1;
static int gGV7602_CH1_Hpv = 0;
static int gGV7602_CH1_Vps = 0;
//static int gGV7602_CH1_CurStatus=-1;
static int gGV7602_CH2_Lps = -1;
static int gGV7602_CH2_Mode=-1;
static int gGV7602_CH2_TMDS = 1;
static int gGV7602_CH2_Hpv = 0;
static int gGV7602_CH2_Vps = 0;
//static int gGV7602_CH2_CurStatus=-1;
static int gGV7602_CH2_InterFace=-1;

static int gGV7602_3D = 0;
static int gGV7602_CH_ByPass=GV7602_CH2;
static int gADV7844_Source_Ch=0;;

static int g_fpga_vesion = 0;

static Vps_Gv7602_hpv_vps gv7602_gLineNum[DEVICE_STD_REACH_LAST] = {
	{"480I",		240,60,1587,720,240}, // 0-480ix60             
	{"576I",		288,50,1600,720,288},  // 1-576ix50
	{"480I",		240,60,1587,720,240}, // 2-480ix60             
	{"576I",		288,50,1600,720,288},  // 3-576ix50
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */
	{"480P",		0xFFFF,0xFFFF,0xFFFF,0,0}, // 7-480px60             
	{"576P",		0xFFFF,0xFFFF,0xFFFF,0,0},  // 8-576px50
#if 1	
	{"720P60",	750,60,1111,1280,720},  // 9-1280x720x60 
	{"720P50",	750,50,1333,1280,720},  // 10-1280x720x50 
	{"1080I60",	563,60,1481,1920,540},  // 11-1920x1080x60i                 
	{"1080I50",	562,50,1777,1920,540},  // 12-1920x1080x50i
	{"1080P60",	1125,60,740,1920,1080}, // 13-1920x1080x60p             
	{"1080P50",	1125,50,888,1920,1080},  // 14-1920x1080x50p
	{"1080P25",	1125,25,1777,1920,1080}, // 15-1920x1080x25p             
	{"1080P30",	1125,30,1481,1920,1080},  // 16-1920x1080x30p
#else
	{"720P60",	720,60,1111,1280,720},  // 9-1280x720x60 
	{"720P50",	720,50,1333,1280,720},  // 10-1280x720x50 
	{"1080I60",	540,60,1481,1920,540},  // 11-1920x1080x60i                 
	{"1080I50",	540,50,1777,1920,540},  // 12-1920x1080x50i
	{"1080P60",	1080,60,740,1920,1080}, // 13-1920x1080x60p             
	{"1080P50",	1080,50,888,1920,1080},  // 14-1920x1080x50p
	{"1080P25",	1080,25,1777,1920,1080}, // 15-1920x1080x25p             
	{"1080P30",	1080,30,1481,1920,1080},  // 16-1920x1080x30p
#endif
	{"640x480@60",	525,60,1588,640,480},  // 17-640x480x60   
	{"640x480@72",	520,72,1320,640,480},  // 18-640x480x72   
	{"640x480@75",	500,75,1333,640,480},  // 19-640x480x75   
	{"640x480@85",	509,85,1155,640,480},  // 20-640x480x85   
	{"800x600@60",	628,60,1320,800,600},  // 21-800x600x60   
	{"800x600@72",	666,72,1040,800,600},  // 22-800x600x72
	{"800x600@75",	625,75,1066,800,600},  // 23-800x600x75   
	{"800x600@85",	631,85,931,800,600},  // 24-800x600x85   
	{"1024x768@60",	806,60,1033,1024,768},  // 25-1024x768x60                   
	{"1024x768@70",	806,70,885,1024,768},  // 26-1024x768x70
	{"1024x768@75",	800,75,833,1024,768},  // 27-1024x768x75                  
	{"1024x768@85",	808,85,728,1024,768},  // 28-1024x768x85 
	
	{"1280x720@60",	750,60,1111,1280,720},  // 29-1280x720x60 
	{"1280x768@60",	798,60,1054,1280,768},  // 30-1280x768x60 
	{"1280x768@75",	0xFFFF,0xFFFF,0xFFFF,0,0},  // 31-1280x768x75 
	{"1280x768@85",	0xFFFF,0xFFFF,0xFFFF,0,0},  // 32-1280x768x85 
	{"1280x800@60",	828,60,1006,1280,800},  // 33-1280x800x60 
	{"1280x960@60",	1000,60,833,1280,960},  // 34-1280x960x60 
	{"1280x1024@60",	1066,60,781,1280,1024}, // 35-1280x1024x60
	{"1280x1024@75",	1066,75,625,1280,1024}, // 36-1280x1024x75
	{"1280x1024@85",	0xFFFF,0xFFFF,0xFFFF,0,0}, // 37-1280x1024x85
	
	{"1366x768@60",	795,60,1047,1366,768}, // 38-1366x768x60
	{"1440x900@60",	934,60,901,1440,900}, // 39-1440x900x60
	{"1400x1050@60",	1089,60,765,1400,1050}, // 40-1400x1050x60
	{"1400x1050@75",	0xFFFF,0xFFFF,0xFFFF,0,0}, // 41-1400x1050x75
	{"1600x1200@60",	1250,60,666,1600,1200},// 42-1600x1200x60
	{"1920x1080@60_DMT", 1125,60,740,1920,1080}, // 43-1920x1080x60-DMT             
	{"1920x1080@60_GTF", 1125,60,740,1920,1080}, // 44-1920x1080x60-GTF
	{"1920x1200@60",	1244,60,675,1920,1200},// 45-1920x1200x60
	{"2560x1440@60",	1481,60,0xFFFF,2560,1440},// 46-2560x1440x60
	
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0},// FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */

	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
			//The video timing is provided by the application.
			//Used in display mode. */

	{"Max",0xFFFF,0xFFFF,0xFFFF,0,0} // FVID2_STD_MAX
};

static Vps_Gv7602_sav_eav_vbi  gv7602_SavEavVbi[DEVICE_STD_REACH_LAST] = {
	{"480I",		0x00,0x00,0x00,0x00},  			// 0-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 1-576ix50     
	{"480I",		0x00,0x00,0x00,0x00},  			// 2-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 3-576ix50     
	{"Revs",		0x00,0x00,0x00,0x00},			// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P",		0x00,0x00,0x00,0x00},  			// 7-480px60     
	{"576P",		0x00,0x00,0x00,0x00},			// 8-576px50   

	{"720P60",	365,0x00,0x00,29},		// 9-1280x720x60
	{"720P50",	695,0x00,0x00,29},  		// 10-1280x720x50           
	{"1080I60",	275,0x00,0x00,21},		// 11-1920x1080x60i              	
	{"1080I50",	715,0x00,0x00,21},		// 12-1920x1080x50i  
	{"1080P60",	273,0x00,0x00,44},		// 13-1920x1080x60p      
	{"1080P50",	713,0x00,0x00,44},		// 14-1920x1080x50p      
	{"1080P25",	715,0x00,0x00,44},		// 15-1920x1080x25p              
	{"1080P30",	275,0x00,0x00,44},		// 16-1920x1080x30p  
	
	{"640x480@60",	100,746,525,45},				// 17 -640x480x60            
	{"640x480@72",	100,746,520,40},				// 18 -640x480x72
	{"640x480@75",	100,746,500,20},				// 19 -640x480x75
	{"640x480@85",	100,746,509,29},				// 20 -640x480x85
	{"800x600@60",	100,906,628,28}, 				// 21 -800x600x60
	{"800x600@72",	100,906,666,66},	 			// 22 -800x600x72               
	{"800x600@75",	100,906,625,25},	 			// 23 -800x600x75               
	{"800x600@85",	100,906,631,31},	 			// 24 -800x600x85
	{"1024x768@60",	100,1130,806,38},				 // 25 -1024x768x60              
	{"1024x768@70",	100,1130,806,38},				 // 26 -1024x768x70              
	{"1024x768@75",	100,1130,800,32},				 // 27-1024x768x75              
	{"1024x768@85",	100,1130,808,40},		 		// 28-1024x768x85 
	{"1280x720@60",	500,1786,725,5},		 		// 29-1280x720x60 
	{"1280x768@60",	100,1386,798,30},				 // 30-1280x768x60
	{"1280x768@75",	0x00,0x00,0x00,0x00},			 // 31-1280x768x75 
	{"1280x768@85",	0x00,0x00,0x00,0x00},	 		// 32-1280x768x85 
	{"1280x800@60",	20,1306,810,10},	 			// 33-1280x800x60 
	{"1280x960@60",	200,1486,990,30},	 			// 34-1280x960x60 
	{"1280x1024@60",	360,1646,1066,42},	 		// 35-1280x1024x60
	{"1280x1024@75",	100,1386,1066,42},	 		// 36-1280x1024x75	
	{"1280x1024@85",	100,1386,1072,48},	 		// 37-1280x1024x85	
	{"1366x768@60",	266,1638,788,20},	 		       // 38-1366x768x60	
	{"1440x900@60",	500,1946,920,20},	 			// 39-1440x900x60
	{"1400x1050@60",	100,1506,1087,37},	 		// 40-1400x1050x60
	{"1400x1050@75",	100,1506,1096,46},	 		// 41-1400x1050x75
	{"1600x1200@60",	480,2086,1250,50},  			 // 42-1600x1200x60
	{"1920x1080@60_DMT", 300,2326,1100,20},		// 43-1920x1080x60-DMT
	{"1920x1080@60_GTF", 100,2026,1090,10},		// 44-1920x1080x60-GTF
	{"1920x1200@60",	536,2462,1242,38},  			 // 45-1920x1200x60
	{"2560x1440@60",	0x00,0x00,0x00,0x00},			// 46-2560x1440x60

    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0x00,0x00,0x00,0x00},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0x00,0x00,0x00,0x00}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0x00,0x00,0x00,0x00} // FVID2_STD_MAX
};

static Vps_Gv7602_InMode ArgMode_GV7602[DEVICE_STD_REACH_LAST] = {
	{"480I",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 0-480ix60                                                
	{"576I",0x0d,0x21,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 1-576ix50     
	{"480I",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 2-480ix60                                                
	{"576I",0x0d,0x21,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 3-576ix50     
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P60",0x07,0x21,0x5F,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 7-480px60     
	{"576P50",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x02,0x58,0x32,0x00},	// 8-576px50   
	{"720P60",0x0a,0x21,0x5F,0xd0,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	// 9-1280x720x60
	{"720P50",0x2a,0x21,0x5F,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 10-1280x720x50           
	{"1080I60",0x0c,0x21,0x5d,0x10,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 11-1920x1080x60i              	
	{"1080I50",0x2c,0x21,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 12-1920x1080x50i  
	{"1080P60",0x0b,0x21,0x5d,0x00,0x21,0x5e,0x10,0x05,0x21,0x5e,0x10},	// 13-1920x1080x60p      
	{"1080P50",0x2b,0x21,0x5d,0x00,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 14-1920x1080x50p      
	{"1080P25",0x05,0x21,0x5d,0x00,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 15-1920x1080x25p              
	{"1080P30",0x05,0x61,0x5e,0x00,0x61,0x5e,0x10,0x05,0x61,0x5e,0x10},	// 16-1920x1080x30p  
	
	{"640x480@60",0x08,0x11,0x5b,0x10,0x63,0x5b,0x10,0x08,0x11,0x5b,0x10},	 // 17 -640x480x60            
	{"640x480@72",0x09,0x11,0x5b,0x10,0x63,0x5b,0x10,0x09,0x11,0x5b,0x10},  	 // 18 -640x480x72
	{"640x480@75",0x0a,0x11,0x5c,0x10,0x63,0x5c,0x10,0x0a,0x11,0x5c,0x10},  	 // 19 -640x480x75
	{"640x480@85",0x0b,0x11,0x5c,0x10,0x63,0x5c,0x10,0x0b,0x11,0x5c,0x10},	 // 20 -640x480x85
	{"800x600@60",0x01,0x11,0x5c,0x10,0x63,0x5c,0x10,0x01,0x11,0x5c,0x10}, 	 // 21 -800x600x60
	{"800x600@72",0x02,0x11,0x5c,0x10,0x63,0x5c,0x10,0x02,0x11,0x5c,0x10},	 // 22 -800x600x72               
	{"800x600@75",0x03,0x11,0x5c,0x10,0x63,0x5c,0x10,0x03,0x11,0x5c,0x10},	 // 23 -800x600x75               
	{"800x600@85",0x04,0x21,0x5c,0x10,0x63,0x5c,0x10,0x04,0x21,0x5c,0x10},	 // 24 -800x600x85
	{"1024x768@60",0x0c,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0c,0x21,0x5d,0x10},	 // 25 -1024x768x60              
	{"1024x768@70",0x0d,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0d,0x21,0x5d,0x10},	 // 26 -1024x768x70              
	{"1024x768@75",0x0e,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0e,0x21,0x5d,0x10},	 // 27-1024x768x75              
	{"1024x768@85",0x0f,0x21,0x5d,0x10,0x63,0x5d,0x10,0x0f,0x21,0x5d,0x10},	 // 28-1024x768x85              
	{"1280x768@60",0x00,0x21,0x5d,0xd0,0xE6,0x90,0xd0,0x72,0x58,0x31,0xe0},	 // 29-1280x768x60
	{"1280x768@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 30-1280x768x75 
	{"1280x768@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 31-1280x768x85 
	{"1280x800@60",0x00,0x21,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 32-1280x800x60 
	{"1280x960@60",0x00,0x21,0x5d,0x10,0xe6,0xb0,0x10,0x01,0xe0,0x3e,0x80},	// 33-1280x960x60 
	{"1280x1024@60",0x05,0x21,0x5d,0x10,0x63,0x5d,0x10,0x05,0x21,0x5d,0x10},	 // 34-1280x1024x60
	{"1280x1024@75",0x06,0x21,0x5d,0x10,0x63,0x5e,0x10,0x05,0x21,0x5e,0x10},	 // 35-1280x1024x75	
	{"1280x1024@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 36-1280x1024x85
	{"1440x900@60",0x00,0x21,0x5d,0x10,0xe7,0x30,0x10,0x02,0x00,0x3a,0x60},	// 37-1440x900x60
	{"1400x1050@60",0x00,0x21,0x5d,0x10,0xE7,0x48,0x10,0x01,0xBA,0x44,0x1E},	 // 38-1400x1050x60
	{"1400x1050@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 39-1400x1050x75
	{"1600x1200@60",0x00,0x21,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	 // 40-1600x1200x60
	{"1920x1200@60",0x00,0x21,0x5d,0x10,0xea,0x20,0xd0,0x01,0x7c,0x4d,0xa0},	 //41-1920x1200x60   
	{"2560x1440@60",	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 42-2560x1440x60

    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} // FVID2_STD_MAX
};

static Vps_Gv7602_InMode ArgMode_ADV7844[DEVICE_STD_REACH_LAST] = {
	{"480I",0x00,0x00,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 0-480ix60                                                
	{"576I",0x0d,0x00,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 1-576ix50     
	{"480I",0x00,0x00,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	// 2-480ix60                                                
	{"576I",0x0d,0x00,0x5F,0xd0,0x21,0x5d,0xd0,0x0d,0x21,0x5d,0xd0},  	// 3-576ix50     
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},  	// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P60",0x07,0x00,0x05,0xd0,0x21,0x5F,0xd0,0x07,0x21,0x5F,0xd0},  	// 7-480px60     
	{"576P50",0x00,0x00,0x05,0xd0,0xE6,0x90,0xd0,0x02,0x58,0x32,0x00},	// 8-576px50   
	{"720P60",0x19,0x19,0x05,0x013,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	// 9-1280x720x60
	{"720P50",0x19,0x19,0x15,0x013,0xe7,0xbc,0xd0,0x02,0xfb,0x2e,0xe0},  	// 10-1280x720x50           
	{"1080I60",0x14,0x1c,0x05,0x14,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 11-1920x1080x60i              	
	{"1080I50",0x14,0x1c,0x15,0x14,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 12-1920x1080x50i  
	{"1080P60",0x1e,0x16,0x05,0x1e,0xea,0x10,0xd0,0x01,0xAA,0x46,0x50},	// 13-1920x1080x60p     
	{"1080P50",0x1e,0x16,0x15,0x1e,0x21,0x5d,0x10,0x05,0x21,0x5d,0x10},	// 14-1920x1080x50p      
	{"1080P25",0x1e,0x00,0x35,0x16,0xea,0x50,0x10,0x03,0xfa,0x46,0x50},	// 15-1920x1080x25p              
	{"1080P30",0x1e,0x00,0x25,0x16,0xe8,0x98,0x10,0x03,0x51,0x46,0x50},	// 16-1920x1080x30p  
	
	{"640x480@60",0x08,0x08,0x5b,0x10,0x63,0x5b,0x10,0x08,0x11,0x5b,0x10},	 // 17 -640x480x60            
	{"640x480@72",0x09,0x09,0x5b,0x10,0x63,0x5b,0x10,0x09,0x11,0x5b,0x10},  	 // 18 -640x480x72
	{"640x480@75",0x0a,0x0a,0x5c,0x10,0x63,0x5c,0x10,0x0a,0x11,0x5c,0x10},  	 // 19 -640x480x75
	{"640x480@85",0x0b,0x0b,0x5c,0x10,0x63,0x5c,0x10,0x0b,0x11,0x5c,0x10},	 // 20 -640x480x85
	{"800x600@60",0x01,0x01,0x5c,0x10,0x63,0x5c,0x10,0x01,0x11,0x5c,0x10}, 	 // 21 -800x600x60
	{"800x600@72",0x02,0x02,0x5c,0x10,0x63,0x5c,0x10,0x02,0x11,0x5c,0x10},	 // 22 -800x600x72               
	{"800x600@75",0x03,0x03,0x5c,0x10,0x63,0x5c,0x10,0x03,0x11,0x5c,0x10},	 // 23 -800x600x75               
	{"800x600@85",0x04,0x04,0x5c,0x10,0x63,0x5c,0x10,0x04,0x21,0x5c,0x10},	 // 24 -800x600x85
	{"1024x768@60",0x0c,0x0c,0x06,0x10,0x63,0x5d,0x10,0x0c,0x21,0x5d,0x10},	 // 25 -1024x768x60              
	{"1024x768@70",0x0d,0x0d,0x06,0x10,0x63,0x5d,0x10,0x0d,0x21,0x5d,0x10},	 // 26 -1024x768x70              
	{"1024x768@75",0x0e,0x0e,0x06,0x10,0x63,0x5d,0x10,0x0e,0x21,0x5d,0x10},	 // 27-1024x768x75              
	{"1024x768@85",0x0f,0x0f,0x06,0x10,0x63,0x5d,0x10,0x0f,0x21,0x5d,0x10},	 // 28-1024x768x85
	{"1280x720@60",0x00,0x13,0x06,0xd0,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	 // 29-1280x720x60
	{"1280x768@60",0x10,0x00,0x5d,0xd0,0xE6,0x90,0xd0,0x72,0x58,0x31,0xe0},	 // 30-1280x768x60
	{"1280x768@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 31-1280x768x75 
	{"1280x768@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 32-1280x768x85 
	{"1280x800@60",0x00,0x00,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xE0},	// 33-1280x800x60 
	{"1280x960@60",0x00,0x00,0x5d,0x10,0xe7,0x08,0x10,0x01,0xe0,0x3e,0x80},	// 34-1280x960x60 DMT
	{"1280x1024@60",0x05,0x05,0x06,0x10,0x63,0x5d,0x10,0x05,0x21,0x5d,0x10},	 // 35-1280x1024x60
	{"1280x1024@75",0x06,0x06,0x06,0x10,0x63,0x5e,0x10,0x05,0x21,0x5e,0x10},	 // 36-1280x1024x75	
	{"1280x1024@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 37-1280x1024x85
	{"1366x768@60",0x00,0x00,0x5d,0xd0,0xE6,0x90,0xd0,0x02,0x58,0x32,0x00},	 // 38-1366x768x60
	{"1440x900@60",0x00,0x00,0x5d,0x10,0xe7,0x70,0x10,0x02,0x00,0x3a,0x60},	// 39-1440x900x60 DMT
	{"1400x1050@60",0x14,0x00,0x5d,0x10,0xE7,0x48,0x10,0x01,0xBA,0x44,0x1E},	 // 40-1400x1050x60
	{"1400x1050@75",0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 41-1400x1050x75
	{"1600x1200@60",0x16,0x00,0x5d,0x10,0xe8,0x70,0xd0,0x01,0x7e,0x4e,0x20},  	 // 42-1600x1200x60
	{"1920x1080@60_DMT",0x00,0x00,0x5d,0x10,0xea,0x10,0xd0,0x01,0xAA,0x46,0x50},	// 43-1920x1080X60-DMT
	{"1920x1080@60_GTF",0x00,0x00,0x5d,0x10,0xe8,0x98,0xd0,0x01,0xA8,0x46,0x50},	// 44-1920x1080X60-GTF
	{"1920x1200@60",0x19,0x00,0x5d,0x10,0xea,0x20,0xd0,0x01,0x7c,0x4d,0xa0},	 //45-1920x1200x60   
	{"2560x1440@60",	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	// 46-2560x1440x60

    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
    	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{"Revs",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF}, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
														//The video timing is provided by the application.
														//Used in display mode. */
	{"Max",0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF} // FVID2_STD_MAX
};

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */
static UInt16 Device_gv7602_Read16_CPLD(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[2]={0};
	UInt16 data;
	
	regAddr[0] = RegAddr;
	status = OSA_CPLD_i2cRead16(&gDevice_gv7602CommonObj.i2cHandle_1,GV7602_IIC_SLAVE_ADDR, regAddr, regValue, 2);
	data = regValue[1]<<8 | regValue[0];
	
	return data;
}

static UInt16 Device_gv7602_Write16_CPLD(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt16 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[2]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal&0xFF;
	regValue[1] = (RegVal&0xFF00)>>8;
	status = OSA_CPLD_i2cWrite16 (&gDevice_gv7602CommonObj.i2cHandle_1, GV7602_IIC_SLAVE_ADDR, regAddr, regValue, 2 );

	return status;
}

#if 0
static UInt8 Device_gv7602_Read8(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_1, GV7602_IIC_SLAVE_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

static Int32 Device_gv7602_Write8(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_1, GV7602_IIC_SLAVE_ADDR, regAddr, regValue, 1 );

	return status;
}
#endif

static UInt8 Device_adv7844_Read8_IO_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_IO_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

static UInt8 Device_adv7844_Read8_CP_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_CP_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

static UInt8 Device_adv7844_Read8_SDP_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_SDP_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}
#if 0
static UInt8 Device_adv7844_Read8_AFE_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_AFE_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}
#endif

static UInt8 Device_adv7844_Read8_Hdmimap(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_HDMI_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

#if 0
static UInt8 Device_adv7844_Read8_SDP_IO_map(Device_Gv7602Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_SDP_IO_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}
#endif

static Int32 Device_adv7844_Write8_IO_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_IO_MAP_ADDR, regAddr, regValue, 1 );
	if(status){
		printf("Device_adv7844_Write8_IO_MAP() Failed! 0x%x = 0x%x\n",regAddr[0], regValue[0]);
	}

	return status;
}

static Int32 Device_adv7844_Write8_CP_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_CP_MAP_ADDR, regAddr, regValue, 1 );
	if(status){
		printf("Device_adv7844_Write8_CP_MAP() Failed! 0x%x = 0x%x\n",regAddr[0], regValue[0]);
	}

	return status;
}

static Int32 Device_adv7844_Write8_AFE_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_AFE_MAP_ADDR, regAddr, regValue, 1 );
	if(status){
		printf("Device_adv7844_Write8_AFE_MAP() Failed! 0x%x = 0x%x\n",regAddr[0], regValue[0]);
	}

	return status;
}

static Int32 Device_adv7844_Write8_Hdmimap(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_HDMI_MAP_ADDR, regAddr, regValue, 1 );
	if(status){
		printf("Device_adv7844_Write8_Hdmimap() Failed! 0x%x = 0x%x\n",regAddr[0], regValue[0]);
	}

	return status;
}

static Int32 Device_adv7844_Write8_SDP_IO_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_SDP_IO_MAP_ADDR, regAddr, regValue, 1 );
	if(status){
		printf("Device_adv7844_Write8_SDP_IO_MAP() Failed! 0x%x = 0x%x\n",regAddr[0], regValue[0]);
	}

	return status;
}

static Int32 Device_adv7844_Write8_SDP_MAP(Device_Gv7602Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_0, ADV7844_SDP_MAP_ADDR, regAddr, regValue, 1 );
	if(status){
		printf("Device_adv7844_Write8_SDP_MAP() Failed! 0x%x = 0x%x\n",regAddr[0], regValue[0]);
	}

	return status;
}

static Int32 Device_adv7844Reset (Device_Gv7602Obj * pObj )
{
	Int32           retVal = 0;

	Device_adv7844_Write8_IO_MAP(pObj, 0xFF ,0x7f) ; //I2C reset
	usleep(200*1000);
	Device_adv7844_Write8_IO_MAP(pObj, 0xF1 ,0x90) ; //SDP map
	Device_adv7844_Write8_IO_MAP(pObj, 0xF2 ,0x94) ; //SDPIO map
	Device_adv7844_Write8_IO_MAP(pObj, 0xF3 ,0x84) ; //AVLINK
	Device_adv7844_Write8_IO_MAP(pObj, 0xF4 ,0x80) ; //CEC
	Device_adv7844_Write8_IO_MAP(pObj, 0xF5 ,0x7C) ; //INFOFRAME
	Device_adv7844_Write8_IO_MAP(pObj, 0xF8 ,0x4C) ; //AFE
	Device_adv7844_Write8_IO_MAP(pObj, 0xF9 ,0x64) ; //KSV
	Device_adv7844_Write8_IO_MAP(pObj, 0xFA ,0x6C) ; //EDID
	Device_adv7844_Write8_IO_MAP(pObj, 0xFB ,0x68) ; //HDMI
	Device_adv7844_Write8_IO_MAP(pObj, 0xFD ,0x44) ; //CP
	Device_adv7844_Write8_IO_MAP(pObj, 0xFE ,0x48) ; //VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0x0C ,0x40) ; //Power up part and power down VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0xFF) ; //Disable Tristate of Pins
	Device_adv7844_Write8_IO_MAP(pObj, 0x15 ,0x80) ; //Disable Tristate of Pins
	Device_adv7844_Write8_IO_MAP(pObj, 0x19 ,0x83) ; //LLC DLL adjustment
	Device_adv7844_Write8_IO_MAP(pObj, 0x33 ,0x40) ; //LLC DLL Enable
	Device_adv7844_Write8_CP_MAP(pObj ,0xBA ,0x03) ; //Set HDMI FreeRun
	Device_adv7844_Write8_CP_MAP(pObj ,0x6C ,0x00) ; //Use fixed clamp values
	Device_adv7844_Write8_CP_MAP(pObj ,0x3E ,0x00) ; //CP pregain disable
		
	Device_adv7844_Write8_AFE_MAP(pObj,0x00 ,0xFF) ; //Power Down ADC's and there associated clocks
	Device_adv7844_Write8_AFE_MAP(pObj,0x01 ,0xFE) ; //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
	Device_adv7844_Write8_AFE_MAP(pObj,0xB5 ,0x00) ; //Setting MCLK to 128Fs

	Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //ADI recommended writes
	Device_adv7844_Write8_Hdmimap(pObj, 0x00 ,0xF0) ; //Set HDMI Input Port A (Enable BG monitoring)
	Device_adv7844_Write8_Hdmimap(pObj, 0x01 ,0x00) ; //Enable clock terminators
	Device_adv7844_Write8_Hdmimap(pObj, 0x0D ,0xF4) ; //ADI recommended writes
	Device_adv7844_Write8_Hdmimap(pObj, 0x14 ,0x1F) ; //Disable compressed Audio Mute Mask
	Device_adv7844_Write8_Hdmimap(pObj, 0x1A ,0x8A) ; //unmute audio
	Device_adv7844_Write8_Hdmimap(pObj, 0x3D ,0x10) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x46 ,0x1F) ; //ADI Recommended Write ES3/Final silicon
	Device_adv7844_Write8_Hdmimap(pObj, 0x60 ,0x88) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x61 ,0x88) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x6C ,0x10) ; //Disable ISRC clearing bit
	Device_adv7844_Write8_Hdmimap(pObj, 0x57 ,0xB6) ; //TMDS PLL Optimization 
	Device_adv7844_Write8_Hdmimap(pObj, 0x58 ,0x03) ; //TMDS PLL Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x75 ,0x10) ; //DDC drive strength 

	Device_adv7844_Write8_Hdmimap(pObj, 0x85 ,0x1F) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x87 ,0x70) ; //ADI Equaliser Setting
	//Device_adv7844_Write8_Hdmimap(pObj, 0x89 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8A ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8D ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8E ,0x1E) ; //ADI Equaliser Setting
	//Device_adv7844_Write8_Hdmimap(pObj, 0x90 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x91 ,0x1E) ; //ADI Equaliser Setting
	//Device_adv7844_Write8_Hdmimap(pObj, 0x93 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x94 ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x9D ,0x02) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x99 ,0xA1) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0x9B ,0x09) ; //HDMI ADI recommended write

	usleep(500*1000);
	
	return (retVal);    
}

static Int32 Device_adv7844GetInfo_D(Device_Gv7602Obj *pObj, GV7602_SyncInfo *SyncInfo)
{
	Int32 	retVal = 0;
	unsigned int vsync = 0;

	Int32 temp = Device_adv7844_Read8_IO_MAP(pObj, 0x6a);	
	SyncInfo->TMDS_A=(temp&(0x01<<3))>>3;
	SyncInfo->TMDS_B=(temp&(0x01<<7))>>7;

//	int lock = Device_adv7844_Read8_Hdmimap(pObj, 0x04) & 0x02;
	int interlaced = Device_adv7844_Read8_Hdmimap(pObj, 0x0b) & 0x20;
	unsigned w = (Device_adv7844_Read8_Hdmimap(pObj, 0x07) & 0x1f) * 256 + Device_adv7844_Read8_Hdmimap(pObj, 0x08);
	unsigned h = (Device_adv7844_Read8_Hdmimap(pObj, 0x09) & 0x1f) * 256 + Device_adv7844_Read8_Hdmimap(pObj, 0x0a);
	unsigned w_total = (Device_adv7844_Read8_Hdmimap(pObj, 0x1e) & 0x3f) * 256 + Device_adv7844_Read8_Hdmimap(pObj, 0x1f);
	unsigned h_total = ((Device_adv7844_Read8_Hdmimap(pObj, 0x26) & 0x3f) * 256 + Device_adv7844_Read8_Hdmimap(pObj, 0x27)) / 2;
	unsigned freq = (((Device_adv7844_Read8_Hdmimap(pObj, 0x51) << 1) + (Device_adv7844_Read8_Hdmimap(pObj, 0x52) >> 7)) * 1000000) + ((Device_adv7844_Read8_Hdmimap(pObj, 0x52) & 0x7f) * 1000000) / 128;
	if(h_total&&w_total){
		vsync = (int) (((((freq*10)/h_total)/w_total)+5)/10);
	}

	if (1) {
		/* adjust for deep color mode */
		freq = freq * 8 / (((Device_adv7844_Read8_Hdmimap(pObj, 0x0b) & 0xc0)>>6) * 2 + 8);
	}

	SyncInfo->HFreq = freq;
	SyncInfo->VFreq = vsync;
	SyncInfo->Width = w;
	SyncInfo->Hight = h;
	SyncInfo->HTotPix = w_total;
	SyncInfo->VTotPix = h_total;
	SyncInfo->Interlaced = interlaced;

	return retVal;
}

static Int32 Device_adv7844GetInfo_A(Device_Gv7602Obj *pObj, int *hpv, int *vps)
{
	Int32 	retVal = 0;
	unsigned char val1,val2;
	int fcl;

	val1 = Device_adv7844_Read8_CP_MAP(pObj , 0xB3);
	val2 = Device_adv7844_Read8_CP_MAP(pObj , 0xB4);
	*hpv=((val1&0x07)<<8)|val2;

	if(*hpv>1231&&*hpv<1242)
		*hpv=1244;

	val1 = Device_adv7844_Read8_CP_MAP(pObj , 0xb8);
	val2 = Device_adv7844_Read8_CP_MAP(pObj , 0xb9);
	fcl =((val1&0x1f)<<8)|val2;
	if(!fcl)
		*vps=0; 
	else
		*vps=(FXTAL/fcl)/256;	

	return retVal;
}

static Int32 Device_adv7844GetInterface(Device_Gv7602Obj *pObj, int *tmds, GV7602_INPUT *InterFace)
{
	Int32 	retVal = 0;
	unsigned char regVal=0;
	
	if(*tmds){
		*InterFace = GV7602_DIGITAL_INPUT;
	}else{
		regVal = Device_adv7844_Read8_IO_MAP(pObj, 0x13);
		if((regVal & 0x3) == 0x3)
			*InterFace = GV7602_YPbPr_INPUT;
		else
			*InterFace = GV7602_ANALOG_INPUT;
	}

//	printf("Device_adv7844GetInterface() -> *InterFace = %d\n",*InterFace);
	return retVal;
}

static int Device_ad7844_SetCPSAV_EAV(Device_Gv7602Obj *pObj, unsigned int sav, unsigned int eav)
{
	Int32 	retVal = 0;
	unsigned char data;

	if((sav == 0)&&(eav == 0))
		return retVal;
	if(gGV7602_CH2_TMDS)
		return 0;

	printf("sav = %d, eav = %d\n", sav, eav);

	data = (sav>>8);
	Device_adv7844_Write8_CP_MAP(pObj, 0x26, data);
	data = (sav)&0xFF;
	Device_adv7844_Write8_CP_MAP(pObj, 0x27, data);

	
	data = (eav>>8);
	Device_adv7844_Write8_CP_MAP(pObj, 0x28, data);
	data = (eav)&0xFF;
	Device_adv7844_Write8_CP_MAP(pObj, 0x29, data);
	
	return retVal;
}

static int Device_ad7844_SetCPVBI(Device_Gv7602Obj *pObj, unsigned int s_vbi, unsigned int e_vbi)
{
	Int32 	retVal = 0;
	unsigned char data;
	
	if((s_vbi == 0)||(e_vbi == 0))
		return retVal;
	if(gGV7602_CH2_TMDS)
		return 0;
	
	printf("s_vbi = %d, e_vbi = %d\n", s_vbi, e_vbi);
	data = (s_vbi>>4)&0xFF;
	Device_adv7844_Write8_IO_MAP(pObj, 0xA5, data);

	data = (s_vbi&0xF)<<4 | ((e_vbi&0xF00)>>8);
	Device_adv7844_Write8_IO_MAP(pObj, 0xA6, data);
	
	data = e_vbi&0xFF;
	Device_adv7844_Write8_IO_MAP(pObj, 0xA7, data);
	return retVal;
}

static Int32 Device_adv7844GetModeIndex(int hpv, int vps, int *index)
{
	Int32 	retVal = 0;
	int i=-1;
	
	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((vps>gv7602_gLineNum[i].vps-3)&&(vps<gv7602_gLineNum[i].vps+3))&&((hpv>gv7602_gLineNum[i].hpv-4)&&(hpv<gv7602_gLineNum[i].hpv+4))){
			break;
		}

	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((vps>gv7602_gLineNum[i].vps-3)&&(vps<gv7602_gLineNum[i].vps+3))&&((hpv>gv7602_gLineNum[i].hpv-10)&&(hpv<gv7602_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_REACH_LAST)
		*index = -1;
	else
		*index = i;

	return retVal;
}

static Int32 Device_adv7844GetModeIndexSD(Device_Gv7602Obj *pObj, int *inMode)
{
	Int32 	retVal = 0;
	unsigned char data;
	int		Retry=3;

	while(Retry--){
		data = Device_adv7844_Read8_SDP_MAP(pObj, 0x52);
		OSA_printf("ADV7844 SDP map 0x52 = 0x%x\n",data);
		if(data & 0x4){
			*inMode = DEVICE_STD_PAL;
		}else{
			*inMode = DEVICE_STD_NTSC;
		}
	}

	if(*inMode != -1){
		usleep(1000*300);
		data = Device_adv7844_Read8_SDP_MAP(pObj, 0x5a);
		if(data & 0x1){
		}else{
			*inMode = -1;
		}
	}

	return retVal;
}

static char Device_adv7844GetLineWidth(Device_Gv7602Obj *pObj, short int  *w_value)
{
	char b_data;
	b_data=Device_adv7844_Read8_Hdmimap(pObj, 0x07);
	*w_value = ((b_data & 0xF) << 8);
	b_data=Device_adv7844_Read8_Hdmimap(pObj,  0x08);

	*w_value += (b_data & 0xFF);
	return 0;
}

static Int32 Device_adv7844InitComm(Device_Gv7602Obj *pObj, int tmds)
{
	Int32 	retVal = 0;
	if(!tmds){
		Device_adv7844_Write8_IO_MAP(pObj, 0XFF ,0X80 );// I2C reset
		Device_adv7844_Write8_IO_MAP(pObj, 0XF1 ,0X90 );// SDP map
		Device_adv7844_Write8_IO_MAP(pObj, 0XF2 ,0X94 );// SDPIO map
		Device_adv7844_Write8_IO_MAP(pObj, 0XF3 ,0X84 );// AVLINK
		Device_adv7844_Write8_IO_MAP(pObj, 0XF4 ,0X80 );// CEC
		Device_adv7844_Write8_IO_MAP(pObj, 0XF5 ,0X7C );// INFOFRAME
		Device_adv7844_Write8_IO_MAP(pObj, 0XF8 ,0X4C );// AFE
		Device_adv7844_Write8_IO_MAP(pObj, 0XF9 ,0X64 );// KSV
		Device_adv7844_Write8_IO_MAP(pObj, 0XFA ,0X6C );// EDID
		Device_adv7844_Write8_IO_MAP(pObj, 0XFB ,0X68 );// HDMI
		Device_adv7844_Write8_IO_MAP(pObj, 0XFD ,0X44 );// CP
		Device_adv7844_Write8_IO_MAP(pObj, 0XFE ,0X48 );// VDP
		Device_adv7844_Write8_IO_MAP(pObj, 0X00 ,0X05 );// VID_STD=01000b for VGA60
		Device_adv7844_Write8_IO_MAP(pObj, 0X01 ,0X82 );// Prim_Mode to graphics input
		Device_adv7844_Write8_IO_MAP(pObj, 0X02 ,0XF6 );// Auto input color space, Limited Range RGB Output
		Device_adv7844_Write8_IO_MAP(pObj, 0X03 ,0X41 );// 24 bit SDR 444
		Device_adv7844_Write8_IO_MAP(pObj, 0X05 ,0X28 );// Disable AV Codes
		Device_adv7844_Write8_IO_MAP(pObj, 0x06 ,0xA7) ;
		Device_adv7844_Write8_IO_MAP(pObj, 0X14 ,0Xff );// Power up Part

		Device_adv7844_Write8_IO_MAP(pObj, 0X0C ,0X40 );// Power up Part
		Device_adv7844_Write8_IO_MAP(pObj,  0X15 ,0XB0 );// Disable Tristate of Pins except for Audio pins
		Device_adv7844_Write8_CP_MAP(pObj , 0X73 ,0XEA );// Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj , 0X74 ,0X8A );// Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj , 0X75 ,0XA2 );// Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj , 0X76 ,0XA8 );// Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj , 0X85 ,0X0B );// Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
		Device_adv7844_Write8_CP_MAP(pObj , 0XC3 ,0X39 );// ADI recommended write
		Device_adv7844_Write8_CP_MAP(pObj , 0X0C ,0X1F );// ADI recommended write
		Device_adv7844_Write8_AFE_MAP(pObj,0X12 ,0X63 );// ADI recommended write
		Device_adv7844_Write8_AFE_MAP(pObj,0X00 ,0X80 );// ADC power Up
		Device_adv7844_Write8_AFE_MAP(pObj,0X02 ,0X00 );// Ain_Sel to 000. (Ain 1,2,3)
		Device_adv7844_Write8_AFE_MAP(pObj,0XC8 ,0X33 );// DLL_PHASE - 110011b
	}
	else{
		Device_adv7844_Write8_IO_MAP(pObj, 0xFF ,0x80) ; //I2C reset

		Device_adv7844_Write8_IO_MAP(pObj, 0xF1 ,0x90) ; //SDP map
		Device_adv7844_Write8_IO_MAP(pObj, 0xF2 ,0x94) ; //SDPIO map
		Device_adv7844_Write8_IO_MAP(pObj, 0xF3 ,0x84) ; //AVLINK
		Device_adv7844_Write8_IO_MAP(pObj, 0xF4 ,0x80) ; //CEC
		Device_adv7844_Write8_IO_MAP(pObj, 0xF5 ,0x7C) ; //INFOFRAME
		Device_adv7844_Write8_IO_MAP(pObj, 0xF8 ,0x4C) ; //AFE
		Device_adv7844_Write8_IO_MAP(pObj, 0xF9 ,0x64) ; //KSV
		Device_adv7844_Write8_IO_MAP(pObj, 0xFA ,0x6C) ; //EDID
		Device_adv7844_Write8_IO_MAP(pObj, 0xFB ,0x68) ; //HDMI
		Device_adv7844_Write8_IO_MAP(pObj, 0xFD ,0x44) ; //CP
		Device_adv7844_Write8_IO_MAP(pObj, 0xFE ,0x48) ; //VDP
		Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,0x06) ; //Prim_Mode =110b HDMI-GR
		Device_adv7844_Write8_IO_MAP(pObj, 0x02 ,0xF6) ; //Auto input color space, Limited Range RGB Output
		Device_adv7844_Write8_IO_MAP(pObj, 0x03 ,0x41) ; //36 bit SDR 444 Mode 0
		//Device_adv7844_Write8_IO_MAP(pObj, 0x05 ,0x28) ; //AV Codes Off
		Device_adv7844_Write8_IO_MAP(pObj, 0x06 ,0xA7) ; //Invert HS and VS for 861 compliance.
		Device_adv7844_Write8_Hdmimap(pObj, 0xC1 ,0xFF) ; //HDMI power control (power saving)
		Device_adv7844_Write8_Hdmimap(pObj, 0xC2 ,0xFF) ; //HDMI power control (power saving)
		Device_adv7844_Write8_Hdmimap(pObj, 0xC3 ,0xFF) ; //HDMI power control (power saving)
		Device_adv7844_Write8_Hdmimap(pObj, 0xC4 ,0xFF) ; //HDMI power control (power saving)
		Device_adv7844_Write8_Hdmimap(pObj, 0xC5 ,0x00) ; //HDMI power control (power saving)
		Device_adv7844_Write8_Hdmimap(pObj, 0xC6 ,0x00) ; //HDMI power control (power saving)
		Device_adv7844_Write8_Hdmimap(pObj, 0xC0 ,0xFF) ; //HDMI power control (power saving)
		Device_adv7844_Write8_IO_MAP(pObj, 0x0C ,0x40) ; //Power up part and power down VDP
		Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0xFF) ; //Disable Tristate of Pins
		Device_adv7844_Write8_IO_MAP(pObj, 0x15 ,0x80) ; //Disable Tristate of Pins
		Device_adv7844_Write8_IO_MAP(pObj, 0x19 ,0x83) ; //LLC DLL adjustment
		Device_adv7844_Write8_IO_MAP(pObj, 0x33 ,0x40) ; //LLC DLL Enable
		Device_adv7844_Write8_CP_MAP(pObj ,0xBA ,0x01) ; //Set HDMI FreeRun
		Device_adv7844_Write8_CP_MAP(pObj ,0x6C ,0x00) ; //Use fixed clamp values
		Device_adv7844_Write8_CP_MAP(pObj ,0x3E ,0x00) ; //CP pregain disable
		Device_adv7844_Write8_AFE_MAP(pObj,0x00 ,0xFF) ; //Power Down ADC's and there associated clocks
		Device_adv7844_Write8_AFE_MAP(pObj,0x01 ,0xFE) ; //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
		Device_adv7844_Write8_AFE_MAP(pObj,0xB5 ,0x00) ; //Setting MCLK to 128Fs
		Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //ADI recommended writes
		Device_adv7844_Write8_Hdmimap(pObj, 0x00 ,0xF0) ; //Set HDMI Input Port A (Enable BG monitoring)
		Device_adv7844_Write8_Hdmimap(pObj, 0x01 ,0x00) ; //Enable clock terminators
		Device_adv7844_Write8_Hdmimap(pObj, 0x0D ,0xF4) ; //ADI recommended writes
		Device_adv7844_Write8_Hdmimap(pObj, 0x14 ,0x1F) ; //Disable compressed Audio Mute Mask
		Device_adv7844_Write8_Hdmimap(pObj, 0x1A ,0x8A) ; //unmute audio
		Device_adv7844_Write8_Hdmimap(pObj, 0x3D ,0x10) ; //HDMI ADI recommended write
		Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //TMDS PLL Optimization
		Device_adv7844_Write8_Hdmimap(pObj, 0x46 ,0x1F) ; //ADI Recommended Write ES3/Final silicon
		Device_adv7844_Write8_Hdmimap(pObj, 0x60 ,0x88) ; //TMDS PLL Optimization
		Device_adv7844_Write8_Hdmimap(pObj, 0x61 ,0x88) ; //TMDS PLL Optimization
		Device_adv7844_Write8_Hdmimap(pObj, 0x6C ,0x10) ; //Disable ISRC clearing bit
		Device_adv7844_Write8_Hdmimap(pObj, 0x57 ,0xB6) ; //TMDS PLL Optimization 
		Device_adv7844_Write8_Hdmimap(pObj, 0x58 ,0x03) ; //TMDS PLL Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x75 ,0x10) ; //DDC drive strength 
		Device_adv7844_Write8_Hdmimap(pObj, 0x85 ,0x1F) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x87 ,0x70) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x89 ,0x04) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x8A ,0x1E) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x8D ,0x04) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x8E ,0x1E) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x90 ,0x04) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x91 ,0x1E) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x93 ,0x04) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x94 ,0x1E) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x9D ,0x02) ; //ADI Equaliser Setting
		Device_adv7844_Write8_Hdmimap(pObj, 0x99 ,0xA1) ; //HDMI ADI recommended write
		Device_adv7844_Write8_Hdmimap(pObj, 0x9B ,0x09) ; //HDMI ADI recommended write
		Device_adv7844_Write8_Hdmimap(pObj, 0xC9 ,0x01) ; //HDMI free Run based on PRIM_MODE, VID _STD
	}

	usleep(100*1000);
	
	return retVal;
}

static Int32 Device_adv7844SDInterFaceInit(Device_Gv7602Obj *pObj)
{
	Int32 		retVal=0;

	Device_adv7844_Write8_IO_MAP(pObj,  0xF1 ,0x90) ; //SDP map
	Device_adv7844_Write8_IO_MAP(pObj,  0xF2 ,0x94) ; //SDPIO map
	Device_adv7844_Write8_IO_MAP(pObj,  0xF3 ,0x84) ; //AVLINK
	Device_adv7844_Write8_IO_MAP(pObj,  0xF4 ,0x80) ; //CEC
	Device_adv7844_Write8_IO_MAP(pObj,  0xF5 ,0x7C) ; //INFOFRAME
	Device_adv7844_Write8_IO_MAP(pObj,  0xF8 ,0x4C) ; //AFE
	Device_adv7844_Write8_IO_MAP(pObj,  0xF9 ,0x64) ; //KSV
	Device_adv7844_Write8_IO_MAP(pObj,  0xFA ,0x6C) ; //EDID
	Device_adv7844_Write8_IO_MAP(pObj,  0xFB ,0x68) ; //HDMI
	Device_adv7844_Write8_IO_MAP(pObj,  0xFD ,0x44) ; //CP
	Device_adv7844_Write8_IO_MAP(pObj,  0xFE ,0x48) ; //VDP
	Device_adv7844_Write8_IO_MAP(pObj,  0x00 ,0x01) ; //CVBS 4x1 mode
	Device_adv7844_Write8_IO_MAP(pObj,  0x01 ,0x00) ; //SD core

	Device_adv7844_Write8_IO_MAP(pObj,  0x03 ,0x80) ; //10 bit Mode
	Device_adv7844_Write8_IO_MAP(pObj,  0x04 ,0x62) ; //Output bus rotation
	Device_adv7844_Write8_IO_MAP(pObj, 0x06 ,0xa8) ; //Invert HS and VS for 861 compliance.
	Device_adv7844_Write8_IO_MAP(pObj,  0x0C ,0x40) ; //Power up Core
	Device_adv7844_Write8_IO_MAP(pObj,  0x14 ,0xFF) ; //Power up pads
	Device_adv7844_Write8_IO_MAP(pObj,  0x15 ,0x80) ; //Power up pads
	Device_adv7844_Write8_IO_MAP(pObj,  0x19 ,0x87) ; //LLC DLL phase
	Device_adv7844_Write8_IO_MAP(pObj,  0x33 ,0x40) ; //LLC DLL enabl
	Device_adv7844_Write8_AFE_MAP(pObj, 0x0C ,0x1F) ; //ADI recommended write 
	Device_adv7844_Write8_AFE_MAP(pObj, 0x12 ,0x63) ; //ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj, 0x00 ,0x00) ; //ADC0 power Up
	Device_adv7844_Write8_AFE_MAP(pObj, 0x02 ,0x00) ; //Manual Mux
	Device_adv7844_Write8_AFE_MAP(pObj, 0x03 ,0x10) ; //Ain11

	Device_adv7844_Write8_AFE_MAP(pObj, 0x15 ,0x05) ; 
	Device_adv7844_Write8_AFE_MAP(pObj, 0x16 ,0x9f) ; 
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0x12 ,0x0d) ; //Timing Adjustment
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0x7A ,0xdd) ; //Timing Adjustment
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0x7B ,0xdF) ; //Timing Adjustment
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0x60 ,0x01) ; //SDRAM reset
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0x97 ,0x00) ; //Hsync width Adjustment
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0xB2 ,0x60) ; //Disable AV codes
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0xB1 ,0x69) ; //Disable AV codes
	Device_adv7844_Write8_SDP_IO_MAP(pObj,0xB3 ,0x69) ; //Disable AV codes


	Device_adv7844_Write8_SDP_MAP(pObj, 0x00 ,0x7F) ; //Autodetect PAL NTSC SECAM
	Device_adv7844_Write8_SDP_MAP(pObj, 0x01 ,0x00) ; //Pedestal Off
	Device_adv7844_Write8_SDP_MAP(pObj, 0x03 ,0xE4) ; //Manual VCR Gain Luma 0x40B
	Device_adv7844_Write8_SDP_MAP(pObj, 0x04 ,0x0B) ; //Manual Luma setting
	Device_adv7844_Write8_SDP_MAP(pObj, 0x05 ,0xC3) ; //Manual Chroma setting 0x3FE
	Device_adv7844_Write8_SDP_MAP(pObj, 0x06 ,0xFE) ; //Manual Chroma setting
	Device_adv7844_Write8_SDP_MAP(pObj, 0x12 ,0x01) ; //Frame TBC,3D comb enabled
	//Device_adv7844_Write8_SDP_MAP(pObj, 0xdd ,0xbf) ; //FREE RUN CORLOR BAR
	//Device_adv7844_Write8_SDP_MAP(pObj, 0xde ,0xaa) ; //FREE RUN CORLOR BAR
	//Device_adv7844_Write8_SDP_MAP(pObj, 0xdf ,0x55) ; //FREE RUN CORLOR BAR
	Device_adv7844_Write8_SDP_MAP(pObj, 0xA7 ,0xff) ; //ADI Recommended Write
	return retVal;
}

static Int32 Device_adv7844DigitalInterFaceInit(Device_Gv7602Obj *pObj)
{
#if 1
	Device_adv7844_Write8_IO_MAP(pObj, 0x00,0X05) ; //Prim_Mode =110b HDMI-GR
	Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,0x06) ; //Prim_Mode =110b HDMI-GR
	Device_adv7844_Write8_IO_MAP(pObj, 0x02 ,0xF6) ; //Auto input color space, Limited Range RGB Output
	Device_adv7844_Write8_IO_MAP(pObj, 0x03 ,0x41) ; //36 bit SDR 444 Mode 0
	Device_adv7844_Write8_IO_MAP(pObj, 0x05 ,0x28) ; //AV Codes Off
	Device_adv7844_Write8_IO_MAP(pObj, 0x06 ,0xA6) ; //Invert HS and VS for 861 compliance.
	Device_adv7844_Write8_Hdmimap(pObj, 0xC1 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC2 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC3 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC4 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC5 ,0x00) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC6 ,0x00) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC0 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_IO_MAP(pObj, 0x0C ,0x40) ; //Power up part and power down VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0xFF) ; //Disable Tristate of Pins
	Device_adv7844_Write8_IO_MAP(pObj, 0x15 ,0x80) ; //Disable Tristate of Pins
	Device_adv7844_Write8_IO_MAP(pObj, 0x19 ,0x83) ; //LLC DLL adjustment
	Device_adv7844_Write8_IO_MAP(pObj, 0x33 ,0x40) ; //LLC DLL Enable
	Device_adv7844_Write8_CP_MAP(pObj ,0xBA ,0x01) ; //Set HDMI FreeRun
	Device_adv7844_Write8_CP_MAP(pObj ,0x6C ,0x00) ; //Use fixed clamp values
	Device_adv7844_Write8_CP_MAP(pObj ,0x3E ,0x00) ; //CP pregain disable
	Device_adv7844_Write8_AFE_MAP(pObj,0x00 ,0xFF) ; //Power Down ADC's and there associated clocks
	Device_adv7844_Write8_AFE_MAP(pObj,0x01 ,0xFE) ; //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
	Device_adv7844_Write8_AFE_MAP(pObj,0xB5 ,0x00) ; //Setting MCLK to 128Fs
	Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //ADI recommended writes
	Device_adv7844_Write8_Hdmimap(pObj, 0x00 ,0xF0) ; //Set HDMI Input Port A (Enable BG monitoring)
	Device_adv7844_Write8_Hdmimap(pObj, 0x01 ,0x00) ; //Enable clock terminators
	Device_adv7844_Write8_Hdmimap(pObj, 0x0D ,0xF4) ; //ADI recommended writes
	Device_adv7844_Write8_Hdmimap(pObj, 0x14 ,0x1F) ; //Disable compressed Audio Mute Mask
	Device_adv7844_Write8_Hdmimap(pObj, 0x1A ,0x8A) ; //unmute audio
	Device_adv7844_Write8_Hdmimap(pObj, 0x3D ,0x10) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x46 ,0x1F) ; //ADI Recommended Write ES3/Final silicon
	Device_adv7844_Write8_Hdmimap(pObj, 0x60 ,0x88) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x61 ,0x88) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x6C ,0x10) ; //Disable ISRC clearing bit
	Device_adv7844_Write8_Hdmimap(pObj, 0x57 ,0xB6) ; //TMDS PLL Optimization 
	Device_adv7844_Write8_Hdmimap(pObj, 0x58 ,0x03) ; //TMDS PLL Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x75 ,0x10) ; //DDC drive strength 
	Device_adv7844_Write8_Hdmimap(pObj, 0x85 ,0x1F) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x87 ,0x70) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x89 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8A ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8D ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8E ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x90 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x91 ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x93 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x94 ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x9D ,0x02) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x99 ,0xA1) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0x9B ,0x09) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0xC9 ,0x01) ; //HDMI free Run based on PRIM_MODE, VID _STD
#endif

	return 0;
}

static Int32 Device_adv7844DigitalInterFaceSetRes(Device_Gv7602Obj *pObj, int inMode)
{

//	OSA_printf("Get Information ArgMode_ADV7844[inMode].config[1]= %x \n",ArgMode_ADV7844[inMode].config[1]);
	
	Device_adv7844_Write8_IO_MAP(pObj,  0x00,ArgMode_ADV7844[inMode].config[3]);//; PLL Divide         
	//Device_adv7844_Write8_IO_MAP(pObj, 0x00,0x013); //Prim_Mode =110b HDMI-GR
	Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,0x06) ; //Prim_Mode =110b HDMI-GR
	Device_adv7844_Write8_IO_MAP(pObj, 0x02 ,0xF6) ; //Auto input color space, Limited Range RGB Output
	Device_adv7844_Write8_IO_MAP(pObj, 0x03 ,0x41) ; //36 bit SDR 444 Mode 0
	Device_adv7844_Write8_IO_MAP(pObj, 0x05 ,0x28) ; //AV Codes Off
	Device_adv7844_Write8_IO_MAP(pObj, 0x06 ,0xA6) ; //Invert HS and VS for 861 compliance.
	Device_adv7844_Write8_Hdmimap(pObj, 0xC1 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC2 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC3 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC4 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC5 ,0x00) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC6 ,0x00) ; //HDMI power control (power saving)
	Device_adv7844_Write8_Hdmimap(pObj, 0xC0 ,0xFF) ; //HDMI power control (power saving)
	Device_adv7844_Write8_IO_MAP(pObj, 0x0C ,0x40) ; //Power up part and power down VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0xFF) ; //Disable Tristate of Pins
	Device_adv7844_Write8_IO_MAP(pObj, 0x15 ,0x80) ; //Disable Tristate of Pins
	Device_adv7844_Write8_IO_MAP(pObj, 0x19 ,0x83) ; //LLC DLL adjustment
	Device_adv7844_Write8_IO_MAP(pObj, 0x33 ,0x40) ; //LLC DLL Enable
	Device_adv7844_Write8_CP_MAP(pObj ,0xBA ,0x03) ; //Set HDMI FreeRun
	Device_adv7844_Write8_CP_MAP(pObj ,0x6C ,0x00) ; //Use fixed clamp values
	Device_adv7844_Write8_CP_MAP(pObj ,0x3E ,0x00) ; //CP pregain disable
	Device_adv7844_Write8_AFE_MAP(pObj,0x00 ,0xFF) ; //Power Down ADC's and there associated clocks
	Device_adv7844_Write8_AFE_MAP(pObj,0x01 ,0xFE) ; //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
	Device_adv7844_Write8_AFE_MAP(pObj,0xB5 ,0x00) ; //Setting MCLK to 128Fs
	Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //ADI recommended writes
	Device_adv7844_Write8_Hdmimap(pObj, 0x00 ,0xF0) ; //Set HDMI Input Port A (Enable BG monitoring)
	Device_adv7844_Write8_Hdmimap(pObj, 0x01 ,0x00) ; //Enable clock terminators
	Device_adv7844_Write8_Hdmimap(pObj, 0x0D ,0xF4) ; //ADI recommended writes
	Device_adv7844_Write8_Hdmimap(pObj, 0x14 ,0x1F) ; //Disable compressed Audio Mute Mask
	Device_adv7844_Write8_Hdmimap(pObj, 0x1A ,0x8A) ; //unmute audio
	Device_adv7844_Write8_Hdmimap(pObj, 0x3D ,0x10) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0x44 ,0x85) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x46 ,0x1F) ; //ADI Recommended Write ES3/Final silicon
	Device_adv7844_Write8_Hdmimap(pObj, 0x60 ,0x88) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x61 ,0x88) ; //TMDS PLL Optimization
	Device_adv7844_Write8_Hdmimap(pObj, 0x6C ,0x10) ; //Disable ISRC clearing bit
	Device_adv7844_Write8_Hdmimap(pObj, 0x57 ,0xB6) ; //TMDS PLL Optimization 
	Device_adv7844_Write8_Hdmimap(pObj, 0x58 ,0x03) ; //TMDS PLL Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x75 ,0x10) ; //DDC drive strength 
	Device_adv7844_Write8_Hdmimap(pObj, 0x85 ,0x1F) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x87 ,0x70) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x89 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8A ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8D ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x8E ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x90 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x91 ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x93 ,0x04) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x94 ,0x1E) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x9D ,0x02) ; //ADI Equaliser Setting
	Device_adv7844_Write8_Hdmimap(pObj, 0x99 ,0xA1) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0x9B ,0x09) ; //HDMI ADI recommended write
	Device_adv7844_Write8_Hdmimap(pObj, 0xC9 ,0x01) ; //HDMI free Run based on PRIM_MODE, VID _ST
	
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[5]= %x \n",ArgMode_ADV7844[inMode].config[4]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[6]= %x \n",ArgMode_ADV7844[inMode].config[5]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[7]= %x \n",ArgMode_ADV7844[inMode].config[7]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[8]= %x \n",ArgMode_ADV7844[inMode].config[8]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[9]= %x \n",ArgMode_ADV7844[inMode].config[9]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[10]= %x \n",ArgMode_ADV7844[inMode].config[10]);

	if(ArgMode_ADV7844[inMode].config[1])
		Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,ArgMode_ADV7844[inMode].config[2]) ; //Prim_Mode =110b HDMI-GR
	if(!ArgMode_ADV7844[inMode].config[1]){
		Device_adv7844_Write8_IO_MAP(pObj,  0x00,0x00);
		Device_adv7844_Write8_IO_MAP(pObj,  0x16,ArgMode_ADV7844[inMode].config[4]);//; PLL Divide                                                              
		Device_adv7844_Write8_IO_MAP(pObj,  0x17,ArgMode_ADV7844[inMode].config[5]);//; PLL Divide 
		Device_adv7844_Write8_CP_MAP(pObj,  0x8F,ArgMode_ADV7844[inMode].config[7]);//; Set Free                                                         
		Device_adv7844_Write8_CP_MAP(pObj,  0x90,ArgMode_ADV7844[inMode].config[8]);//; Set Free 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAB,ArgMode_ADV7844[inMode].config[9]);//; Set line 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAC,ArgMode_ADV7844[inMode].config[10]);//; Set line 
	}

	return 0;
}

static Int32 Device_adv7844AnalogInterFaceInit(Device_Gv7602Obj *pObj)
{
	Int32 	retVal = 0;
	Device_adv7844_Write8_IO_MAP(pObj, 0XFF ,0X80 );// I2C reset
	usleep(200*1000);
	Device_adv7844_Write8_IO_MAP(pObj, 0XF1 ,0X90 );// SDP map
	Device_adv7844_Write8_IO_MAP(pObj, 0XF2 ,0X94 );// SDPIO map
	Device_adv7844_Write8_IO_MAP(pObj, 0XF3 ,0X84 );// AVLINK
	Device_adv7844_Write8_IO_MAP(pObj, 0XF4 ,0X80 );// CEC
	Device_adv7844_Write8_IO_MAP(pObj, 0XF5 ,0X7C );// INFOFRAME
	Device_adv7844_Write8_IO_MAP(pObj, 0XF8 ,0X4C );// AFE
	Device_adv7844_Write8_IO_MAP(pObj, 0XF9 ,0X64 );// KSV
	Device_adv7844_Write8_IO_MAP(pObj, 0XFA ,0X6C );// EDID
	Device_adv7844_Write8_IO_MAP(pObj, 0XFB ,0X68 );// HDMI
	Device_adv7844_Write8_IO_MAP(pObj, 0XFD ,0X44 );// CP
	Device_adv7844_Write8_IO_MAP(pObj, 0XFE ,0X48 );// VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0X00 ,0X05 );// VID_STD=01000b for VGA60
	Device_adv7844_Write8_IO_MAP(pObj, 0X01 ,0X82 );// Prim_Mode to graphics input
	Device_adv7844_Write8_IO_MAP(pObj, 0X02 ,0XF6 );// Auto input color space, Limited Range RGB Output
	Device_adv7844_Write8_IO_MAP(pObj, 0X03 ,0X41 );// 24 bit SDR 444
	Device_adv7844_Write8_IO_MAP(pObj, 0X05 ,0X28 );// Disable AV Codes
	Device_adv7844_Write8_IO_MAP(pObj, 0X14 ,0Xff );// Power up Part

	Device_adv7844_Write8_IO_MAP(pObj, 0X0C ,0X40 );// Power up Part
	Device_adv7844_Write8_IO_MAP(pObj,  0X15 ,0XB0 );// Disable Tristate of Pins except for Audio pins
	Device_adv7844_Write8_CP_MAP(pObj , 0X73 ,0XEA );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X74 ,0X8A );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X75 ,0XA2 );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X76 ,0XA8 );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X85 ,0X0B );// Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
	Device_adv7844_Write8_CP_MAP(pObj , 0XC3 ,0X39 );// ADI recommended write
	Device_adv7844_Write8_CP_MAP(pObj , 0X0C ,0X1F );// ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj,0X12 ,0X63 );// ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj,0X00 ,0X80 );// ADC power Up
	Device_adv7844_Write8_AFE_MAP(pObj,0X02 ,0X00 );// Ain_Sel to 000. (Ain 1,2,3)
	Device_adv7844_Write8_AFE_MAP(pObj,0XC8 ,0X33 );// DLL_PHASE - 110011b
	return retVal;
}

static Int32 Device_adv7844AnalogInterFaceSetRes(Device_Gv7602Obj *pObj, int inMode)
{
	Int32 	retVal = 0;

//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[0]= %x \n",ArgMode_ADV7844[inMode].config[0]);

	Device_adv7844_Write8_IO_MAP(pObj, 0XFF ,0X80 );// I2C reset
	usleep(200*1000);
	Device_adv7844_Write8_IO_MAP(pObj, 0XF1 ,0X90 );// SDP map
	Device_adv7844_Write8_IO_MAP(pObj, 0XF2 ,0X94 );// SDPIO map
	Device_adv7844_Write8_IO_MAP(pObj, 0XF3 ,0X84 );// AVLINK
	Device_adv7844_Write8_IO_MAP(pObj, 0XF4 ,0X80 );// CEC
	Device_adv7844_Write8_IO_MAP(pObj, 0XF5 ,0X7C );// INFOFRAME
	Device_adv7844_Write8_IO_MAP(pObj, 0XF8 ,0X4C );// AFE
	Device_adv7844_Write8_IO_MAP(pObj, 0XF9 ,0X64 );// KSV
	Device_adv7844_Write8_IO_MAP(pObj, 0XFA ,0X6C );// EDID
	Device_adv7844_Write8_IO_MAP(pObj, 0XFB ,0X68 );// HDMI
	Device_adv7844_Write8_IO_MAP(pObj, 0XFD ,0X44 );// CP
	Device_adv7844_Write8_IO_MAP(pObj, 0XFE ,0X48 );// VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0X00 ,ArgMode_ADV7844[inMode].config[0] );// VID_STD=01000b for VGA60
	Device_adv7844_Write8_IO_MAP(pObj, 0X01 ,0X02 );// Prim_Mode to graphics input
	Device_adv7844_Write8_IO_MAP(pObj, 0X02 ,0XF6 );// Auto input color space, Limited Range RGB Output
	Device_adv7844_Write8_IO_MAP(pObj, 0X03 ,0X41 );// 24 bit SDR 444
	Device_adv7844_Write8_IO_MAP(pObj, 0X05 ,0X28 );// Disable AV Codes
	Device_adv7844_Write8_IO_MAP(pObj, 0x06 ,0xA6) ;
	Device_adv7844_Write8_IO_MAP(pObj, 0X14 ,0Xff );// Power up Part

	Device_adv7844_Write8_IO_MAP(pObj , 0X0C ,0X40 );// Power up Part
	Device_adv7844_Write8_IO_MAP(pObj , 0X15 ,0XB0 );// Disable Tristate of Pins except for Audio pins
	Device_adv7844_Write8_CP_MAP(pObj , 0X73 ,0XEA );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X74 ,0X8A );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X75 ,0XA2 );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X76 ,0XA8 );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X85 ,0X0B );// Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
	Device_adv7844_Write8_CP_MAP(pObj , 0XC3 ,0X39 );// ADI recommended write
	Device_adv7844_Write8_CP_MAP(pObj , 0X0C ,0X1F );// ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj,0X12 ,0X63 );// ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj,0X00 ,0X80 );// ADC power Up
	Device_adv7844_Write8_AFE_MAP(pObj,0X02 ,0X00 );// Ain_Sel to 000. (Ain 1,2,3)
	Device_adv7844_Write8_AFE_MAP(pObj,0XC8 ,0X33 );// DLL_PHASE - 110011b
	if(inMode < DEVICE_STD_VGA_640X480X60){
		Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,ArgMode_ADV7844[inMode].config[2]) ; //Prim_Mode =110b HDMI-GR
		Device_adv7844_Write8_IO_MAP(pObj, 0X02 ,0XF4 );// Auto input color space, Limited Range RGB Output
		Device_adv7844_Write8_CP_MAP(pObj , 0X3e ,0X0c );// Set manual gain of 0x2A8
	}

//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[5]= %x \n",ArgMode_ADV7844[inMode].config[4]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[6]= %x \n",ArgMode_ADV7844[inMode].config[5]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[7]= %x \n",ArgMode_ADV7844[inMode].config[7]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[8]= %x \n",ArgMode_ADV7844[inMode].config[8]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[9]= %x \n",ArgMode_ADV7844[inMode].config[9]);
//	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_ADV7844[inMode].config[10]= %x \n",ArgMode_ADV7844[inMode].config[10]);

	if(!ArgMode_ADV7844[inMode].config[0]){
		Device_adv7844_Write8_IO_MAP(pObj, 0x02, 0xF6) ; //Auto input color space, Limited Range RGB Output
		Device_adv7844_Write8_IO_MAP(pObj, 0x03, 0x40) ; //24 bit SDR 444
		Device_adv7844_Write8_IO_MAP(pObj, 0x05, 0x28) ; //Disable AV Codes
		Device_adv7844_Write8_IO_MAP(pObj, 0x0C, 0x40) ; //Power up Part
		Device_adv7844_Write8_IO_MAP(pObj, 0x15, 0xB0) ; //Disable Tristate of Pins except for Audio pins
		Device_adv7844_Write8_IO_MAP(pObj, 0x19, 0x80) ; //enable LLC DLL
		Device_adv7844_Write8_IO_MAP(pObj, 0x33, 0x40) ; //select DLL for LLC clock
		Device_adv7844_Write8_CP_MAP(pObj , 0x73, 0xEA) ; //Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj , 0x74, 0x8A) ; //Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj ,0x75, 0xA2) ; //Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj ,0x76, 0xA8) ; //Set manual gain of 0x2A8
		Device_adv7844_Write8_CP_MAP(pObj ,0x85, 0x0B) ; //Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
		Device_adv7844_Write8_CP_MAP(pObj ,0xBE, 0x02) ; //HCOUNT ALIGN ADJ
		Device_adv7844_Write8_CP_MAP(pObj ,0xBF, 0x32) ; //HCOUNT ALIGN ADJ
		Device_adv7844_Write8_CP_MAP(pObj ,0xC3, 0x39) ; //ADI recommended write
		Device_adv7844_Write8_AFE_MAP(pObj, 0x12, 0x63) ; //ADI recommended write
		Device_adv7844_Write8_AFE_MAP(pObj, 0x0C, 0x1F) ; //ADI recommended write
		Device_adv7844_Write8_AFE_MAP(pObj, 0x00, 0x80) ; //ADC power Up
		Device_adv7844_Write8_AFE_MAP(pObj,0x02, 0x00) ; //Ain_Sel to 000. (Ain 1,2,3)
		Device_adv7844_Write8_AFE_MAP(pObj, 0xC8, 0x33) ; //DLL Phase, 110011b
		Device_adv7844_Write8_IO_MAP(pObj,  0x00, 0x07) ; //Set VID_STD to 0b000111 for autographics
		Device_adv7844_Write8_IO_MAP(pObj,  0x01, 0x02) ; //Set PRIM_MODE to 0b0010 for graphics mode
//		Device_adv7844_Write8_IO_MAP(pObj,  0x16, 0xC6) ; //set PLL for 1600 samples per line
//		Device_adv7844_Write8_IO_MAP(pObj,  0x17, 0x40) ; //set PLL for 1600 samples per line
		Device_adv7844_Write8_CP_MAP(pObj ,0x81, 0xD0) ; //enable blue screen freerun in autographics mode
		Device_adv7844_Write8_CP_MAP(pObj , 0x8B, 0x4C) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj , 0x8C, 0x02) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj , 0x8B, 0x4C) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj ,0x8D, 0x40) ; //set horizontal DE start/end position
//		Device_adv7844_Write8_CP_MAP(pObj ,0x8F, 0x02) ; //configure freerun parameters FR_LL
//		Device_adv7844_Write8_CP_MAP(pObj , 0x90, 0x04) ; //configure freerun parameters FR_LL
		Device_adv7844_Write8_CP_MAP(pObj , 0x91, 0x00) ; //set CP core to progressive mode
		Device_adv7844_Write8_CP_MAP(pObj , 0xA5, 0x39) ; //set vertical DE start/end position (odd/prog)
		Device_adv7844_Write8_CP_MAP(pObj , 0xA6, 0xC0) ; //set vertical DE start/end position (odd/prog)
		Device_adv7844_Write8_CP_MAP(pObj ,0xA7, 0x18) ; //set vertical DE start/end position (odd/prog)
		Device_adv7844_Write8_CP_MAP(pObj ,0xA8, 0x1C) ; //set vertical DE start/end position (even)
		Device_adv7844_Write8_CP_MAP(pObj , 0xA9, 0xD1) ; //set vertical DE start/end position (even)
		Device_adv7844_Write8_CP_MAP(pObj , 0xAA, 0xDA) ; //set vertical DE start/end position (even)
//		Device_adv7844_Write8_CP_MAP(pObj , 0xAB, 0x39) ; //configure freerun parameter LCOUNT_MAX
//		Device_adv7844_Write8_CP_MAP(pObj , 0xAC, 0xE0) ; //configure freerun parameter LCOUNT_MAX
		Device_adv7844_Write8_IO_MAP(pObj,  0x16,ArgMode_ADV7844[inMode].config[4]);//; PLL Divide                                                              
		Device_adv7844_Write8_IO_MAP(pObj,  0x17,ArgMode_ADV7844[inMode].config[5]);//; PLL Divide 
		Device_adv7844_Write8_CP_MAP(pObj,  0x8F,ArgMode_ADV7844[inMode].config[7]);//; Set Free                                                         
		Device_adv7844_Write8_CP_MAP(pObj,  0x90,ArgMode_ADV7844[inMode].config[8]);//; Set Free 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAB,ArgMode_ADV7844[inMode].config[9]);//; Set line 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAC,ArgMode_ADV7844[inMode].config[10]);//; Set line 
	}
	
	Device_ad7844_SetCPSAV_EAV(pObj, gv7602_SavEavVbi[inMode].sav, gv7602_SavEavVbi[inMode].eav);
	Device_ad7844_SetCPVBI(pObj, gv7602_SavEavVbi[inMode].s_vbi, gv7602_SavEavVbi[inMode].e_vbi);
	
	return retVal;
}

static Int32 Device_adv7844YPbPrInterFaceSetRes(Device_Gv7602Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
	
	Device_adv7844_Write8_IO_MAP(pObj, 0XFF ,0X80 );// I2C reset
	usleep(200*1000);
	Device_adv7844_Write8_IO_MAP(pObj, 0XF1 ,0X90 );// SDP map
	Device_adv7844_Write8_IO_MAP(pObj, 0XF2 ,0X94 );// SDPIO map
	Device_adv7844_Write8_IO_MAP(pObj, 0XF3 ,0X84 );// AVLINK
	Device_adv7844_Write8_IO_MAP(pObj, 0XF4 ,0X80 );// CEC
	Device_adv7844_Write8_IO_MAP(pObj, 0XF5 ,0X7C );// INFOFRAME
	Device_adv7844_Write8_IO_MAP(pObj, 0XF8 ,0X4C );// AFE
	Device_adv7844_Write8_IO_MAP(pObj, 0XF9 ,0X64 );// KSV
	Device_adv7844_Write8_IO_MAP(pObj, 0XFA ,0X6C );// EDID
	Device_adv7844_Write8_IO_MAP(pObj, 0XFB ,0X68 );// HDMI
	Device_adv7844_Write8_IO_MAP(pObj, 0XFD ,0X44 );// CP
	Device_adv7844_Write8_IO_MAP(pObj, 0XFE ,0X48 );// VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0X00 ,ArgMode_ADV7844[inMode].config[0] );// VID_STD=01000b for VGA60
	Device_adv7844_Write8_IO_MAP(pObj, 0X01 ,0X82 );// Prim_Mode to graphics input
	Device_adv7844_Write8_IO_MAP(pObj, 0X02 ,0XF6 );// Auto input color space, Limited Range RGB Output
	Device_adv7844_Write8_IO_MAP(pObj, 0X03 ,0X41 );// 24 bit SDR 444
	Device_adv7844_Write8_IO_MAP(pObj, 0X05 ,0X28 );// Disable AV Codes
	Device_adv7844_Write8_IO_MAP(pObj, 0X14 ,0Xff );// Power up Part

	Device_adv7844_Write8_IO_MAP(pObj, 0X0C ,0X40 );// Power up Part
	Device_adv7844_Write8_IO_MAP(pObj,  0X15 ,0XB0 );// Disable Tristate of Pins except for Audio pins
	Device_adv7844_Write8_CP_MAP(pObj , 0X73 ,0XEA );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X74 ,0X8A );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X75 ,0XA2 );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X76 ,0XA8 );// Set manual gain of 0x2A8
	Device_adv7844_Write8_CP_MAP(pObj , 0X85 ,0X0B );// Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
	Device_adv7844_Write8_CP_MAP(pObj , 0XC3 ,0X39 );// ADI recommended write
	Device_adv7844_Write8_CP_MAP(pObj , 0X0C ,0X1F );// ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj,0X12 ,0X63 );// ADI recommended write
	Device_adv7844_Write8_AFE_MAP(pObj,0X00 ,0X80 );// ADC power Up
	Device_adv7844_Write8_AFE_MAP(pObj,0X02 ,0X00 );// Ain_Sel to 000. (Ain 1,2,3)
	Device_adv7844_Write8_AFE_MAP(pObj,0XC8 ,0X33 );// DLL_PHASE - 110011b
	return retVal;
}

static Int32 Device_adv7844Config(Device_Gv7602Obj *pObj, int inMode, int tmds, GV7602_INPUT InterFace)
{
	Int32 	retVal = 0;

	printf("Call Device_adv7844Config()! inMode = %d tmds = %d InterFace = %d\n",inMode, tmds, InterFace);

	Device_adv7844OutputEnable(pObj, FALSE);
	switch(InterFace){
		case GV7602_DIGITAL_INPUT:
			Device_adv7844Reset(pObj);
			Device_adv7844DigitalInterFaceInit(pObj);
			Device_adv7844DigitalInterFaceSetRes(pObj, inMode);
			break;
		case GV7602_ANALOG_INPUT:
			if(ArgMode_ADV7844[inMode].config[0]){
				Device_adv7844Reset(pObj);
				usleep(100000);
				Device_adv7844AnalogInterFaceInit(pObj);
			}
			Device_adv7844AnalogInterFaceSetRes(pObj, inMode);
			break;
		case GV7602_YPbPr_INPUT:
			Device_adv7844YPbPrInterFaceSetRes(pObj, inMode);
				break;
		default:
			printf("ADV7844 Unknow Interface in Device_adv7844Config()!\n");
	}
	
	Device_adv7844OutputEnable(pObj, TRUE);

	return retVal;
}

static int Device_adv7844_check_1366Or1280(Device_Gv7602Obj *pObj, int *index)
{
	Int32 	retVal = 0;
	Int32	lwidth=0;
	unsigned char data;

	data = Device_adv7844_Read8_IO_MAP(pObj, 0xB5);
	if(gGV7602_CH2_TMDS){
		data = Device_adv7844_Read8_Hdmimap(pObj, 0x07);
		lwidth = (data&0xF)<<8;
		data = Device_adv7844_Read8_Hdmimap(pObj, 0x08);
		lwidth += (data&0xFF);
		if(lwidth > 1300)
			*index = DEVICE_STD_VGA_1366X768X60;
		else
			*index = DEVICE_STD_VGA_1280X768X60;
	}else{
		if(data&0x08){    
			*index = DEVICE_STD_VGA_1366X768X60;
		}
		else {
			*index = DEVICE_STD_VGA_1280X768X60;
		}
	}
	
	return retVal;
}

static int Device_adv7844_check_VGAorVideo(Device_Gv7602Obj *pObj, int *index)
{
	Int32 	retVal = 0;
	unsigned char data;

	data = Device_adv7844_Read8_IO_MAP(pObj, 0xB5);
	if(gGV7602_CH2_TMDS){
		*index = DEVICE_STD_720P_60;
	}else{
		if((data&0x03)==0x03)
			*index = DEVICE_STD_720P_60;
		else
			*index = DEVICE_STD_VGA_1280X720X60;
	}
	return retVal;
}

static int Device_adv7844_check_1080_DMTorGTF(Device_Gv7602Obj *pObj, int *index)
{
	Int32 	retVal = 0;
	unsigned char data;

	data = Device_adv7844_Read8_CP_MAP(pObj, 0xB5);
	if(!gGV7602_CH2_TMDS){
		if(data&0x08)
			*index = DEVICE_STD_VGA_1920X1080X60_GTF;
		else
			*index = DEVICE_STD_VGA_1920X1080X60_DMT;

		if((data&0x03)==0x03)
			*index = DEVICE_STD_1080P_60;
	}
	return retVal;
}

static Int32 Device_gv7602Ch0Config(Device_Gv7602Obj *pObj, int inMode, int tmds)
{
	Int32 	retVal = 0;

	if((gv7602_gLineNum[inMode].width != 0)&&(gv7602_gLineNum[inMode].hight != 0)
		&&(gv7602_SavEavVbi[inMode].sav != 0)&&(gv7602_SavEavVbi[inMode].e_vbi != 0))
		{
			printf("Device_gv7602Config() Ch0 inMode = %d\n",inMode);
			Device_gv7602_Write16_CPLD(pObj, 2*2, gv7602_gLineNum[inMode].width);
			Device_gv7602_Write16_CPLD(pObj, 3*2, gv7602_gLineNum[inMode].hight);
			Device_gv7602_Write16_CPLD(pObj, 6*2, 11);
//			Device_gv7602_Write16_CPLD(pObj, 8*2, 2);
	}else{
		printf("SDI Std = %s \n",gv7602_gLineNum[inMode].name);
		printf("Device_gv7602Config() Ch0 Errror!\n");
	}

	return retVal;
}

static Int32 Device_gv7602Ch1Config(Device_Gv7602Obj *pObj, int inMode, int tmds)
{
	Int32 	retVal = 0;

	if((gv7602_gLineNum[inMode].width != 0)&&(gv7602_gLineNum[inMode].hight != 0)
		&&(gv7602_SavEavVbi[inMode].sav != 0)&&(gv7602_SavEavVbi[inMode].e_vbi != 0))
		{
			printf("Device_gv7602Config() Ch1 inMode = %d\n",inMode);
			Device_gv7602_Write16_CPLD(pObj, 4*2, gv7602_gLineNum[inMode].width);
			Device_gv7602_Write16_CPLD(pObj, 5*2, gv7602_gLineNum[inMode].hight);
			Device_gv7602_Write16_CPLD(pObj, 6*2, 11);
			Device_gv7602_Write16_CPLD(pObj, 8*2, 2);
	}else{
		printf("SDI Std = %s \n",gv7602_gLineNum[inMode].name);
		printf("Device_gv7602Config() Ch1 Errror!\n");
	}

	return retVal;
}

static Int32 Device_gv7602Ch2Config(Device_Gv7602Obj *pObj, int inMode, int tmds)
{
	Int32 	retVal = 0;

	if((gv7602_gLineNum[inMode].width != 0)&&(gv7602_gLineNum[inMode].hight != 0)
		&&(gv7602_SavEavVbi[inMode].sav != 0)&&(gv7602_SavEavVbi[inMode].e_vbi != 0))
	{
			printf("Device_gv7602Config() Ch2 inMode = %d\n",inMode);
			Device_gv7602_Write16_CPLD(pObj, 4*2, gv7602_gLineNum[inMode].width);
			Device_gv7602_Write16_CPLD(pObj, 5*2, gv7602_gLineNum[inMode].hight);
			Device_gv7602_Write16_CPLD(pObj, 6*2, 11);
			Device_gv7602_Write16_CPLD(pObj, 8*2, 1);
	}else{
		printf("SDI Std = %s \n",gv7602_gLineNum[inMode].name);
		printf("Device_gv7602Config() Ch2 Errror!\n");
	}

	return retVal;
}

static Int32 Device_adv7844GetResolutionALL(Device_Gv7602Obj *pObj)
{
	Int32 	retVal = 0;
	Int32	hpv=0, vps=0;
	int 		inMode;
	int 		retry = RETRYNUMS;
	GV7602_SyncInfo sysinformation;
	//unsigned char regVal=0;
	short int   width; 
 	int var ;

	printf("Call Device_adv7844GetResolutionALL!\n");
	
	while(retry--){
		Device_adv7844GetInfo_D(pObj , &sysinformation);
		if(gGV7602_CH2_TMDS != sysinformation.TMDS_A){
			gGV7602_CH2_TMDS = sysinformation.TMDS_A;
			Device_adv7844InitComm(pObj, sysinformation.TMDS_A);
		}

//		OSA_printf("Get Information gGV7602_CH2_TMDS A= %x \n",gGV7602_CH2_TMDS);
		if(gGV7602_CH2_TMDS){
			//Digital input
			Device_adv7844GetInfo_D(pObj , &sysinformation);
			hpv=sysinformation.VTotPix;
			vps=sysinformation.VFreq;
	//		OSA_printf("Get Information From Digital input!\n");
		}else{
			//Analog input
			Device_adv7844GetInfo_A(pObj,&hpv,&vps);
//			OSA_printf("Get Information From Analog input!\n");
		}

		gGV7602_CH2_Hpv = hpv;
		gGV7602_CH2_Vps = vps;
//		OSA_printf("Get Information hpv = %d  vps=%d  \n",hpv,vps);
		Device_adv7844GetModeIndex(gGV7602_CH2_Hpv, gGV7602_CH2_Vps, &inMode);

//////////////////////////////////
		if((inMode == DEVICE_STD_VGA_1366X768X60)||(inMode == DEVICE_STD_VGA_1280X768X60))
			Device_adv7844_check_1366Or1280(pObj, &inMode);
		if((inMode == DEVICE_STD_VGA_1280X720X60)||(inMode == DEVICE_STD_720P_60))
			Device_adv7844_check_VGAorVideo(pObj, &inMode);
		if((inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_1080P_60))
			Device_adv7844_check_1080_DMTorGTF(pObj, &inMode);
//////////////////////////////////	

		if((inMode == gGV7602_CH2_Mode)&&(inMode != -1))
			break;
		usleep(200000);
	}

	if(gGV7602_CH2_TMDS&&(inMode==-1)){
		Device_adv7844GetLineWidth(pObj,&width);
		if(((width>637)&&(width<645))){
			inMode=gGV7602_CH2_Mode;
		}
		if(((width>795)&&(width<805))){
			inMode=gGV7602_CH2_Mode;
		}
	}

	if((gGV7602_CH2_Mode != inMode)&&(inMode != -1)){
		gGV7602_CH2_Mode = inMode;
		Device_adv7844GetInterface(pObj, &gGV7602_CH2_TMDS, &gGV7602_CH2_InterFace);
		Device_adv7844Config(pObj, gGV7602_CH2_Mode, gGV7602_CH2_TMDS, gGV7602_CH2_InterFace);
		Device_gv7602Ch2Config(pObj, gGV7602_CH2_Mode, gGV7602_CH2_TMDS);
		if(gGV7602_CH2_Mode==DEVICE_STD_NTSC||gGV7602_CH2_Mode==DEVICE_STD_PAL){
			Device_adv7844SDInterFaceInit(pObj);
			Device_adv7844GetModeIndexSD(pObj, &inMode);
		}
		
	}else{
		if((inMode == -1)&&(gGV7602_CH2_TMDS == 0)){
//			Device_adv7844SDInterFaceInit(pObj);
//			Device_adv7844GetModeIndexSD(pObj, &inMode);
//			if(inMode == -1){
//				Device_adv7844Reset(pObj);
//			}
		}
		gGV7602_CH2_Mode = inMode;
	}

	if(gGV7602_CH2_Mode != -1){
#ifdef DEVICE_GV7602_DEBUG		
	//	Device_adv7844GetCPLDVersion(pObj);
		printf("###########################################\n");
		printf("Get ADV7844 Resolution!\n");
		printf("gGV7602_CH2_std = %s \n",ArgMode_ADV7844[gGV7602_CH2_Mode].name);
		printf("gGV7602_CH2_hpv = %d gGV7602_CH2_vps = %d \n",gGV7602_CH2_Hpv,gGV7602_CH2_Vps);
		printf("gGV7602_CH2_Mode = %d gGV7602_CH2_TMDS = %d gGV7602_CH2_InterFace = %d\n",gGV7602_CH2_Mode,gGV7602_CH2_TMDS,gGV7602_CH2_InterFace);
		printf("###########################################\n");
#endif
	}else{
#ifdef DEVICE_GV7602_DEBUG
		//Device_adv7844GetCPLDVersion(pObj);
		printf("###########################################\n");
		printf("Get ADV7844 Resolution!\n");
		printf("gGV7602_CH2_std = unknow \n");
		printf("gGV7602_CH2_hpv = %d gGV7602_CH2_vps = %d \n",gGV7602_CH2_Hpv,gGV7602_CH2_Vps);
		printf("gGV7602_CH2_Mode = %d gGV7602_CH2_TMDS = %d gGV7602_CH2_InterFace = %d\n",gGV7602_CH2_Mode,gGV7602_CH2_TMDS,gGV7602_CH2_InterFace);
		printf("###########################################\n");
#endif		
	}
	usleep(500*1000);
#if 0
	if(gGV7602_CH2_Mode==DEVICE_STD_NTSC||gGV7602_CH2_Mode==DEVICE_STD_PAL){
		var = Device_gv7602_Read16_CPLD(pObj,  0x18);
		var = var|0x01;
		Device_gv7602_Write16_CPLD(pObj,  0x18 ,var) ;
		regVal = Device_adv7844_Read8_SDP_MAP(pObj, 0x56);
		if(regVal & 0x01){
			printf("We Must Reset ADV7442 Becase of FreeRun!\n");
			Device_adv7844Reset(pObj);
			gGV7602_CH2_CurStatus = -1;
			gGV7602_CH2_Mode = -1;
			gGV7602_CH2_TMDS = -1;
			gGV7602_CH2_Hpv= -1;
			gGV7602_CH2_Vps= -1;
			gGV7602_CH2_InterFace = ADV7844_INPUT_UNKNOW;
		}
	}
	else{
		var = Device_gv7602_Read16_CPLD(pObj,  0x18);
		var= var&0xfffe;
		Device_gv7602_Write16_CPLD(pObj,  0x18 ,var) ;
	}
#endif
	retry = 3;
	while(retry--){
		var = Device_adv7844_Read8_CP_MAP(pObj, 0xff);
		if(var & 0x10){
			OSA_printf("ADV7442 CP Core Is FreeRun!0xFF=%x @@@@@@@@\n",var);
//			gGV7602_CH2_CurStatus = -1;
//			gGV7602_CH2_Mode = -1;
//			gGV7602_CH2_TMDS = -1;
//			gGV7602_CH2_Hpv= -1;
//			gGV7602_CH2_Vps= -1;
//			gGV7602_CH2_InterFace = -1;
		}
		usleep(500*1000);
	}
	
	return (retVal);
}

static Int32 Device_adv7844GetResolution(Device_Gv7602Obj *pObj)
{
	gADV7844_Source_Ch=0;
	switch(gADV7844_Source_Ch){
		case DEVICE_SOURCE_ALL:
			Device_adv7844GetResolutionALL(pObj);
			break;
#if 0
		case DEVICE_SOURCE_CP:
			Device_adv7844GetResolutionCP(pObj);
			break;
		case DEVICE_SOURCE_CVBS:
			Device_adv7844GetResolutionCVBS(pObj);
			break;
#endif 
		default:
			OSA_printf("Error gADV7844_Source_Ch Index Unknow!\n");
	}
	return 0;
}
#if 0
static Int32 Device_gv7602Config(Device_Gv7602Obj *pObj, int inMode, int tmds)
{
	Int32 	retVal = 0;
#if 0
	if((gv7602_gLineNum[inMode].width != 0)&&(gv7602_gLineNum[inMode].hight != 0)
		&&(gv7602_SavEavVbi[inMode].sav != 0)&&(gv7602_SavEavVbi[inMode].e_vbi != 0))
		{
			printf("Device_gv7602Config() inMode = %d\n",inMode);
//			Device_gv7602_Write16_CPLD(pObj, 16*2, gv7602_gLineNum[inMode].width);
//			Device_gv7602_Write16_CPLD(pObj, 17*2, gv7602_gLineNum[inMode].hight);
//			Device_gv7602_Write16_CPLD(pObj, 18*2, gv7602_SavEavVbi[inMode].sav);
//			Device_gv7602_Write16_CPLD(pObj, 19*2, gv7602_SavEavVbi[inMode].e_vbi);
	}else{
		printf("SDI Std = %s \n",gv7602_gLineNum[inMode].name);
		printf("Device_gv7602Config() Errror!\n");
	}
#endif
	return retVal;
}
#endif

Int32 Device_gv7602Ch2SetDirection ( Device_Gv7602Obj * pObj,
                             unsigned int *direction )
{
	Int32 status = 0;
	Int32 inMode = 0,tmds=0;
	Int32 val;
	Int32 sav,eav,s_vbi,e_vbi;

	inMode = gGV7602_CH2_Mode;
	tmds = gGV7602_CH2_TMDS;
	val = *direction;

	if((tmds)||(inMode == -1))return 0;

	sav = (val&0xFFFF0000)>>16;
	eav = sav+6+gv7602_gLineNum[inMode].width;
	e_vbi = val&0xFFFF;
	s_vbi = e_vbi+gv7602_gLineNum[inMode].hight;
	
	gv7602_SavEavVbi[inMode].sav = 	sav;
	gv7602_SavEavVbi[inMode].eav = 	eav;
	gv7602_SavEavVbi[inMode].s_vbi = s_vbi;
	gv7602_SavEavVbi[inMode].e_vbi = e_vbi;

	Device_ad7844_SetCPSAV_EAV(pObj, sav, eav);
	Device_ad7844_SetCPVBI(pObj, s_vbi, e_vbi);

	return status;
}

Int32 Device_gv7602Ch2GetDirection ( Device_Gv7602Obj * pObj,
                             unsigned int *direction )
{
	Int32 status = 0;
	Int32 inMode = 0,tmds=0;
	Int32 val;
	Int32 sav,eav,s_vbi,e_vbi;

	inMode = gGV7602_CH2_Mode;
	tmds = gGV7602_CH2_TMDS;
	val = *direction;

	if((tmds)||(inMode == -1))return 0;
	
	sav = gv7602_SavEavVbi[inMode].sav;
	e_vbi = gv7602_SavEavVbi[inMode].e_vbi;
	val = (((sav&0xFFFF)<<16) | (e_vbi&0xFFFF));

	*direction = val;
	
	return status;
}

static Int32 Device_gv7602Ch0GetResolution(Device_Gv7602Obj * pObj)
{
	Int32 hsfq = 0, linenum=0;
	int i=0, inMode=0;;
	//unsigned short data;
	unsigned short cpld_version=0;

	cpld_version = Device_gv7602_Read16_CPLD(pObj, 15*2);
	hsfq  = Device_gv7602_Read16_CPLD(pObj, 9*2);
	linenum  = Device_gv7602_Read16_CPLD(pObj, 10*2);
	
	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((hsfq>gv7602_gLineNum[i].lps-10)&&(hsfq<gv7602_gLineNum[i].lps+10))&&((linenum>gv7602_gLineNum[i].hpv-4)&&(linenum<gv7602_gLineNum[i].hpv+4))){
			break;
		}
	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((hsfq>gv7602_gLineNum[i].lps-10)&&(hsfq<gv7602_gLineNum[i].lps+10))&&((linenum>gv7602_gLineNum[i].hpv-10)&&(linenum<gv7602_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_REACH_LAST)
		inMode = -1;
	else
		inMode = i;

	gGV7602_CH0_Hpv= linenum;
	gGV7602_CH0_Lps = hsfq;
	
	if((inMode != -1)&&(gGV7602_CH0_Mode != inMode)){
		gGV7602_CH0_Mode = inMode;
		Device_gv7602Ch0Config(pObj, gGV7602_CH0_Mode, gGV7602_CH0_TMDS);
#if 0
		data = Device_gv7602_Read16_CPLD(pObj, 14*2);
		data &= 3;
		if((inMode == DEVICE_STD_1080I_60)||(inMode == DEVICE_STD_1080I_50)){
			data &= ~1;
			Device_gv7602_Write16_CPLD(pObj, 14*2,data);
		}else{
			data |= 1;
			Device_gv7602_Write16_CPLD(pObj,14*2,data);
		}
#endif
	}else{
		gGV7602_CH0_Mode = inMode;
	}
#if 1	
	if(gGV7602_CH0_Mode != -1){
		printf("###########################################\n");
		printf("Get GV7602 Resolution!\n");
		printf("gGV7602_Ch0_std = %s \n",ArgMode_GV7602[gGV7602_CH0_Mode].name);
		printf("gGV7602_Ch0_hpv = %d gGV7602_lps = %d \n",gGV7602_CH0_Hpv,gGV7602_CH0_Lps);
		printf("gGV7602_Ch0_Mode = %d \n",gGV7602_CH0_Mode);
		printf("FPGA Version = 0x%x \n",cpld_version);
		printf("###########################################\n");
	}else{
		printf("###########################################\n");
		printf("Get GV7602 Resolution!\n");
		printf("gGV7602_Ch0_std = unknow! \n");
		printf("gGV7602_Ch0_hpv = %d gGV7602_lps = %d \n",gGV7602_CH0_Hpv,gGV7602_CH0_Lps);
		printf("gGV7602_Ch0_Mode = %d \n",gGV7602_CH0_Mode);
		printf("FPGA Version = 0x%x \n",cpld_version);
		printf("###########################################\n");
	}
#endif
	return inMode;
}

static Int32 Device_gv7602Ch1GetResolution(Device_Gv7602Obj * pObj)
{
	Int32 hsfq = 0, linenum=0;
	int i=0, inMode=0;;
	//unsigned short data;
	unsigned short cpld_version=0;

	cpld_version = Device_gv7602_Read16_CPLD(pObj, 15*2);
	hsfq  = Device_gv7602_Read16_CPLD(pObj, 11*2);
	linenum  = Device_gv7602_Read16_CPLD(pObj, 12*2);
	
	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((hsfq>gv7602_gLineNum[i].lps-10)&&(hsfq<gv7602_gLineNum[i].lps+10))&&((linenum>gv7602_gLineNum[i].hpv-4)&&(linenum<gv7602_gLineNum[i].hpv+4))){
			break;
		}
	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((hsfq>gv7602_gLineNum[i].lps-10)&&(hsfq<gv7602_gLineNum[i].lps+10))&&((linenum>gv7602_gLineNum[i].hpv-10)&&(linenum<gv7602_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_REACH_LAST)
		inMode = -1;
	else
		inMode = i;

	gGV7602_CH1_Hpv= linenum;
	gGV7602_CH1_Lps = hsfq;
	
	if((inMode != -1)&&(gGV7602_CH1_Mode != inMode)){
		gGV7602_CH1_Mode = inMode;
		Device_gv7602Ch1Config(pObj, gGV7602_CH1_Mode, gGV7602_CH1_TMDS);
	}else{
		gGV7602_CH1_Mode = inMode;
	}
#if 1	
	if(gGV7602_CH1_Mode != -1){
		printf("###########################################\n");
		printf("Get GV7602 Resolution!\n");
		printf("gGV7602_Ch1_std = %s \n",ArgMode_GV7602[gGV7602_CH1_Mode].name);
		printf("gGV7602_Ch1_hpv = %d gGV7602_lps = %d \n",gGV7602_CH1_Hpv,gGV7602_CH1_Lps);
		printf("gGV7602_Ch1_Mode = %d \n",gGV7602_CH1_Mode);
		printf("FPGA Version = 0x%x \n",cpld_version);
		printf("###########################################\n");
	}else{
		printf("###########################################\n");
		printf("Get GV7602 Resolution!\n");
		printf("gGV7602_Ch1_std = unknow! \n");
		printf("gGV7602_Ch1_hpv = %d gGV7602_lps = %d \n",gGV7602_CH1_Hpv,gGV7602_CH1_Lps);
		printf("gGV7602_Ch1_Mode = %d \n",gGV7602_CH1_Mode);
		printf("FPGA Version = 0x%x \n",cpld_version);
		printf("###########################################\n");
	}
#endif
	return inMode;
}

static Int32 Device_gv7602GetResolution(Device_Gv7602Obj *pObj)
{
	Int32 status = 0;

	Device_gv7602Ch0GetResolution(pObj);
	if(gGV7602_CH_ByPass == GV7602_CH2){
		Device_gv7602Ch1GetResolution(pObj);
	}else if(gGV7602_CH_ByPass == GV7602_CH1){
		Device_adv7844GetResolution(pObj);
	}

#if 0
	Device_gv7602_Write16_CPLD(pObj, 2, gGV7602_3D);
	if((gGV7602_CH0_Mode == gGV7602_CH1_Mode)){
		if(gGV7602_3D == 0){
			switch(gGV7602_CH0_Mode){
				case DEVICE_STD_720P_60:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 1);
					break;
				case DEVICE_STD_720P_50:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 0);
					break;
				case DEVICE_STD_1080I_60:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 7);
					break;
				case DEVICE_STD_1080I_50:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 6);
					break;
				case DEVICE_STD_1080P_60:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 11);
					break;
				case DEVICE_STD_1080P_50:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 10);
					break;
				case DEVICE_STD_1080P_24:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 10);
					break;
				case DEVICE_STD_1080P_30:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 11);
					break;
				default:
					printf("Error!\n");
			}
		}
		else if((gGV7602_3D == 1)||(gGV7602_3D == 2)){
			switch(gGV7602_CH0_Mode){
				case DEVICE_STD_720P_60:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 3);
					break;
				case DEVICE_STD_720P_50:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 2);
					break;
				case DEVICE_STD_1080I_60:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 5);
					break;
				case DEVICE_STD_1080I_50:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 4);
					break;
				case DEVICE_STD_1080P_60:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 9);
					break;
				case DEVICE_STD_1080P_50:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 8);
					break;
				case DEVICE_STD_1080P_24:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 8);
					break;
				case DEVICE_STD_1080P_30:
					Device_gv7602_Write16_CPLD(pObj, 6*2, 9);
					break;
				default:
					printf("Error!\n");
			}
		}
		else{
			printf("gGV7602_3D Error gGV7602_3D = %d\n",gGV7602_3D);
		}
	}
	else{
		Device_gv7602_Write16_CPLD(pObj, 6*2, 11);
	}
#endif

	return status;
}

Int32 Device_gv7602GetFpagVersion()
{
	return g_fpga_vesion;
}

Int32 Device_gv7602SetDirection ( Device_Gv7602Obj * pObj,
                             unsigned int *direction )
{
	Int32 status = 0;

	Device_gv7602Ch2SetDirection(pObj, direction);
	
	return status;
}

Int32 Device_gv7602GetDirection ( Device_Gv7602Obj * pObj,
                             unsigned int *direction )
{
	Int32 status = 0;

	Device_gv7602Ch2GetDirection(pObj, direction);
	
	return status;
}

Int32 Device_gv7602Get3DExternInformation(Device_Gv7602Obj * pObj,
							Device_VideoDecoder3DExternInforms* extenInfor)
{
	Int32 status = 0;
	
	memcpy(extenInfor->DeviceName, "gv7602", sizeof(extenInfor->DeviceName));

	extenInfor->ModeID[0] = gGV7602_CH0_Mode;
	extenInfor->SignalTmds[0] = gGV7602_CH0_TMDS;
	extenInfor->SignalHpv[0] = gGV7602_CH0_Hpv;
	extenInfor->SignalFreq[0] = gGV7602_CH0_Vps;
	extenInfor->SignalYPbPr[0] = 0;
	extenInfor->SignalHsfqFpga[0] = gGV7602_CH0_Lps;
	extenInfor->SignalLinenumFpga[0] = gGV7602_CH0_Hpv;
	
	if(gGV7602_CH_ByPass == GV7602_CH2){
		extenInfor->ModeID[1] = gGV7602_CH1_Mode;
		extenInfor->SignalTmds[1] = gGV7602_CH1_TMDS;
		extenInfor->SignalHpv[1] = gGV7602_CH1_Hpv;
		extenInfor->SignalFreq[1] = gGV7602_CH1_Vps;
		extenInfor->SignalYPbPr[1] = 0;
		extenInfor->SignalHsfqFpga[1] = gGV7602_CH1_Lps;
		extenInfor->SignalLinenumFpga[1] = gGV7602_CH1_Hpv;
	}else if(gGV7602_CH_ByPass == GV7602_CH1){
		extenInfor->ModeID[1] = gGV7602_CH2_Mode;
		extenInfor->SignalTmds[1] = gGV7602_CH2_TMDS;
		extenInfor->SignalHpv[1] = gGV7602_CH2_Hpv;
		extenInfor->SignalFreq[1] = gGV7602_CH2_Vps;
		extenInfor->SignalYPbPr[1] = 0;
		extenInfor->SignalHsfqFpga[1] = gGV7602_CH2_Lps;
		extenInfor->SignalLinenumFpga[1] = gGV7602_CH2_Hpv;
	}

	return status;
}

Int32 Device_gv7602SetByPassChannel(Device_Gv7602Obj * pObj,  unsigned int *channel)
{
	Int32 status = 0;
	struct R_GPIO_data data;

	if((*channel == 1)||(*channel == 2)){
		switch(*channel){
			case 0:
				break;
			case 1:
				data.gpio_num = 41;
				data.gpio_value = 0;
				ioctl(gDevice_gv7602CommonObj.GpioHandle, 0x55555555, &data);
				gGV7602_CH_ByPass = *channel;
				gGV7602_CH2_Mode = -1;
				printf("Device_gv7602SetByPassChannel() !gGV7602_CH_ByPass=%d\n",gGV7602_CH_ByPass);
				break;
			case 2:
				data.gpio_num = 41;
				data.gpio_value = 1;
				ioctl(gDevice_gv7602CommonObj.GpioHandle, 0x55555555, &data);
				gGV7602_CH_ByPass = *channel;
				gGV7602_CH1_Mode = -1;
				printf("Device_gv7602SetByPassChannel() !gGV7602_CH_ByPass=%d\n",gGV7602_CH_ByPass);
				break;
			default:
				printf("Error->Device_gv7602SetByPassChannel() !gGV7602_CH_ByPass=%d\n",gGV7602_CH_ByPass);
		}
	}
	
	return status;
}

Int32 Device_gv7602GetByPassChannel(Device_Gv7602Obj * pObj,  unsigned int *channel)
{
	Int32 status = 0;

	*channel = gGV7602_CH_ByPass;

	return status;
}

Int32 Device_gv7602Set3DMode(Device_Gv7602Obj * pObj,  unsigned int *mode_3D)
{
	Int32 status = 0;

	if((*mode_3D == 0)||(*mode_3D == 1)||(*mode_3D == 2)){
		gGV7602_3D = *mode_3D;
	}
	
	return status;
}

Int32 Device_gv7602Get3DMode(Device_Gv7602Obj * pObj,  unsigned int *mode_3D)
{
	Int32 status = 0;

	*mode_3D = gGV7602_3D;
	
	return status;
}

/* Set ADV7441 mode based on

  - mux mode - line or pixel or no-mux
  - number of channels
  - resolution
  - 8/16-bit data width
  - NTSC/PAL standard
  - cascade mode of operation
*/
Int32 Device_gv7602SetVideoMode ( Device_Gv7602Obj * pObj,
                                Device_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = 0;

    return status;
}

/* reset ADV7441 OFM logic  */
Int32 Device_gv7602Reset ( Device_Gv7602Obj * pObj )
{
	Int32           retVal = 0;
	
	printf("GV7602 Reset OK!\n");
	Device_adv7844Reset(pObj);
	
	return (retVal);    
}

/*
  Enable ADV7441 output port
*/
Int32 Device_gv7602OutputEnable ( Device_Gv7602Obj * pObj, UInt32 enable )
{
    Int32 status = 0;

    return status;
}

/* start/enable output  */
Int32 Device_gv7602Start ( Device_Gv7602Obj * pObj )
{
    Int32 status = 0;

    status = Device_gv7602OutputEnable ( pObj, TRUE );

    return status;
}

/* stop/disable output  */
Int32 Device_gv7602Stop ( Device_Gv7602Obj * pObj )
{
    Int32 status = 0;

    status = Device_gv7602OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Get ADV7441 chip ID, revision ID and firmware patch ID
*/
Int32 Device_gv7602GetChipId ( Device_Gv7602Obj * pObj,
                             Device_VideoDecoderChipIdParams * pPrm,
                             Device_VideoDecoderChipIdStatus * pStatus )
{
    Int32 status = 0;

    return status;
}

/*
  Get ADV7441 detect vide standard status

  Can be called for each channel
*/
Int32 Device_gv7602GetVideoStatus ( Device_Gv7602Obj * pObj,
                                     VCAP_VIDEO_SOURCE_STATUS_PARAMS_S * pPrm,
                                     VCAP_VIDEO_SOURCE_CH_STATUS_S     * pStatus )
{
	Int32 status = 0;

	Device_gv7602GetResolution(pObj);
	memset(pStatus, 0, sizeof(VCAP_VIDEO_SOURCE_CH_STATUS_S));
	if(gGV7602_CH0_Mode != -1){
		pStatus->isVideoDetect = 1;
		pStatus->frameWidth = gv7602_gLineNum[gGV7602_CH0_Mode].width;
		pStatus->frameHeight = gv7602_gLineNum[gGV7602_CH0_Mode].hight;
		pStatus->isInterlaced = 0;
		
		if(gGV7602_CH0_Mode < DEVICE_STD_CIF)	
			pStatus->isInterlaced = 1;
		if(gGV7602_CH0_Mode >DEVICE_STD_720P_50 &&  gGV7602_CH0_Mode <DEVICE_STD_1080P_60)	
			pStatus->isInterlaced = 1;
	}else{
		pStatus->isVideoDetect = 0;
	}

	if(gGV7602_CH_ByPass == GV7602_CH2){
		if(gGV7602_CH1_Mode != -1){
			pStatus->isVideoDetect |= 1<<16;
			pStatus->frameWidth |= gv7602_gLineNum[gGV7602_CH1_Mode].width<<16;
			pStatus->frameHeight |= gv7602_gLineNum[gGV7602_CH1_Mode].hight<<16;
			pStatus->isInterlaced |= 0<<16;
			
			if(gGV7602_CH1_Mode < DEVICE_STD_CIF)	
				pStatus->isInterlaced |= 1<<16;
			if(gGV7602_CH1_Mode >DEVICE_STD_720P_50 &&  gGV7602_CH1_Mode <DEVICE_STD_1080P_60)	
				pStatus->isInterlaced |= 1<<16;
		}else{
			pStatus->isVideoDetect |= 0<<16;
		}
	}else if(gGV7602_CH_ByPass == GV7602_CH1){
		if(gGV7602_CH2_Mode != -1){
			pStatus->isVideoDetect |= 1<<16;
			pStatus->frameWidth |= gv7602_gLineNum[gGV7602_CH2_Mode].width<<16;
			pStatus->frameHeight |= gv7602_gLineNum[gGV7602_CH2_Mode].hight<<16;
			pStatus->isInterlaced |= 0<<16;
			
			if(gGV7602_CH2_Mode < DEVICE_STD_CIF)	
				pStatus->isInterlaced |= 1<<16;
			if(gGV7602_CH2_Mode >DEVICE_STD_720P_50 &&  gGV7602_CH2_Mode <DEVICE_STD_1080P_60)	
				pStatus->isInterlaced |= 1<<16;
		}else{
			pStatus->isVideoDetect |= 0<<16;
		}
	}

	return status;
}

/*
  Set video color related parameters
*/
Int32 Device_gv7602SetVideoColor ( Device_Gv7602Obj * pObj,
                                 Device_VideoDecoderColorParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/*
  Set video noise filter related parameters
*/
Int32 Device_gv7602SetVideoNf ( Device_Gv7602Obj * pObj,
                              Device_Gv7602VideoNfParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/* write to I2C registers */
Int32 Device_gv7602RegWrite ( Device_Gv7602Obj * pObj,
                            Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cWrite8 (&gDevice_gv7602CommonObj.i2cHandle_1, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/* read from I2C registers */
Int32 Device_gv7602RegRead ( Device_Gv7602Obj * pObj,
                           Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cRead8 (&gDevice_gv7602CommonObj.i2cHandle_1, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs);

    return status;
}

