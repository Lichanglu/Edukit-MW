
/*******************************************************************************
 *                                                                             *
 * Copyright (c) 2009 Texas Instruments Incorporated - http://www.ti.com/      *
 *                        ALL RIGHTS RESERVED                                  *
 *                                                                             *
 ******************************************************************************/

#include "ti_media_std.h"
#include "ti_vsys_common_def.h"
#include <device.h>
#include <device_videoDecoder.h>
#include <device_adv7844.h>
#include <adv7844_priv.h>
#include <osa_i2c.h>

#define	DEVICE_ADV7844_DEBUG
#ifdef DEVICE_ADV7844_DEBUG
#define ADV_OSA_printf  printf
#else
#define ADV_OSA_printf(...)
#endif

#define FXTAL						28636360
#define RETRYNUMS					1

static int gADV7844_CPLD_Mode=-1;
static int gADV7844_CPLD_Hpv=-1;
static int gADV7844_CPLD_Lps=-1;
static int gADV7844_Mode=-1;
static int gADV7844_TMDS = -1;
static int gADV7844_Hpv = 0;
static int gADV7844_Vps = 0;
static int gADV7844_CurStatus=-1;
static ADV7844_INPUT gADV7844_InterFace = ADV7844_INPUT_UNKNOW;
static Device_Source_CH gADV7844_Source_Ch = DEVICE_SOURCE_CP;//DEVICE_SOURCE_ALL;

static Vps_Adv7844_hpv_vps adv7844_gLineNum[DEVICE_STD_REACH_LAST] = {
	{"480I",		263,60,1587,720,240}, // 0-480ix60             
	{"576I",		313,50,1600,720,288},  // 1-576ix50
	{"480I",		263,60,1587,720,240}, // 2-480ix60             
	{"576I",		313,50,1600,720,288},  // 3-576ix50
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */
	{"Revs",		0xFFFF,0xFFFF,0xFFFF,0,0}, // FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */
	{"480P",		0xFFFF,0xFFFF,0xFFFF,0,0}, // 7-480px60             
	{"576P",		0xFFFF,0xFFFF,0xFFFF,0,0},  // 8-576px50
	{"720P60",	750,60,1111,1280,720},  // 9-1280x720x60 
	{"720P50",	750,50,1333,1280,720},  // 10-1280x720x50 
	{"1080I60",	563,60,1481,1920,540},  // 11-1920x1080x60i                 
	{"1080I50",	562,50,1777,1920,540},  // 12-1920x1080x50i
	{"1080P60",	1125,60,740,1920,1080}, // 13-1920x1080x60p             
	{"1080P50",	1125,50,888,1920,1080},  // 14-1920x1080x50p
	{"1080P25",	1125,25,1777,1920,1080}, // 15-1920x1080x25p             
	{"1080P30",	1125,30,1481,1920,1080},  // 16-1920x1080x30p

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

static Vps_Adv7844_sav_eav_vbi  adv7844_SavEavVbi[DEVICE_STD_REACH_LAST] = {
	{"480I",		0x00,0x00,0x00,0x00},  			// 0-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 1-576ix50     
	{"480I",		0x00,0x00,0x00,0x00},  			// 2-480ix60                                                
	{"576I",		0x00,0x00,0x00,0x00},  			// 3-576ix50     
	{"Revs",		0x00,0x00,0x00,0x00},				// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{"Revs",		0x00,0x00,0x00,0x00},  			// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{"480P",		0x00,0x00,0x00,0x00},  			// 7-480px60     
	{"576P",		0x00,0x00,0x00,0x00},				// 8-576px50   
	{"720P60",	255,1541,748,28},				// 9-1280x720x60
	{"720P50",	251,1537,746,26},  			// 10-1280x720x50           
//	{"1080I60",	185,2111,560,20},				// 11-1920x1080x60i              	
//	{"1080I50",	186,2112,562,22},				// 12-1920x1080x50i    
	{"1080I60", 185,2111,0x00,0x00},				// 11-1920x1080x60i 				
	{"1080I50", 186,2112,0x00,0x00},				// 12-1920x1080x50i  

	{"1080P60",	180,2106,1120,40},				// 13-1920x1080x60p      
	{"1080P50",	180,2106,1120,40},				// 14-1920x1080x50p      
	{"1080P25",	200,2126,1122,42},				// 15-1920x1080x25p              
	{"1080P30",	236,2162,1120,42},				// 16-1920x1080x30p  
	
	{"640x480@60",	135,781,525,45},				// 17 -640x480x60            
	{"640x480@72",	160,806,508,28},				// 18 -640x480x72
	{"640x480@75",	164,810,502,22},				// 19 -640x480x75
	{"640x480@85",	126,772,500,20},				// 20 -640x480x85
	{"800x600@60",	202,1008,628,28}, 				// 21 -800x600x60
	{"800x600@72",	178,984,626,26},	 			// 22 -800x600x72               
	{"800x600@75",	225,1031,625,25},	 			// 23 -800x600x75               
	{"800x600@85",	200,1006,631,31},	 			// 24 -800x600x85
	{"1024x768@60",	280,1310,806,38},				 // 25 -1024x768x60              
	{"1024x768@70",	264,1294,806,38},				 // 26 -1024x768x70              
	{"1024x768@75",	260,1290,800,32},				 // 27-1024x768x75              
	{"1024x768@85",	292,1322,808,40},		 		// 28-1024x768x85  
	{"1280x720@60",	320,1606,750,30},				 // 29-1280x720x60
	{"1280x768@60",	320,1606,798,30},				 // 30-1280x768x60
	{"1280x768@75",	0x00,0x00,0x00,0x00},			 // 31-1280x768x75 
	{"1280x768@85",	0x00,0x00,0x00,0x00},	 		// 32-1280x768x85 
	{"1280x800@60",	330,1616,830,30},	 			// 33-1280x800x60
	{"1280x960@60",	340,1626,990,30},	 			// 34-1280x960x60 
	
	{"1280x1024@60",	350,1636,1066,42},	 		// 35-1280x1024x60
	{"1280x1024@75",	376,1662,1066,42},	 		// 36-1280x1024x75	
	{"1280x1024@85",	0x00,0x00,0x00,0x00},	 		// 37-1280x1024x85	
	{"1366x768@60",	266,1638,788,20},	 		       // 38-1366x768x60	
	{"1440x900@60",	350,1796,930,30},	 			// 39-1440x900x60
	{"1400x1050@60",	359,1765,1087,37},	 		// 40-1400x1050x60
	{"1400x1050@75",	0x00,0x00,0x00,0x00},	 		// 41-1400x1050x75
	{"1600x1200@60",	480,2086,1250,50},  			 // 42-1600x1200x60
	{"1920x1080@60_DMT", 300,2326,1100,20},		// 43-1920x1080x60-DMT
	{"1920x1080@60_GTF", 100,2026,1090,10},		// 44-1920x1080x60-GTF
	{"1920x1200@60",	536,2462,1242,38},  			 // 45-1920x1200x60  XXXXXXXXXXXXXXXXXXXXXXXXXXXX
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

static Vps_Adv7844_InMode ArgMode_7844_2[DEVICE_STD_REACH_LAST] = {
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
	{"1280x720@60",0x0a,0x13,0x06,0xd0,0xe6,0x80,0xd0,0x02,0x7f,0x2e,0xe0},	 // 29-1280x720x60
	{"1280x768@60",0x10,0x00,0x5d,0xd0,0xE6,0x90,0xd0,0x72,0x58,0x31,0xe0},	 // 30-1280x768x60
	{"1280x768@75",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 31-1280x768x75 
	{"1280x768@85",0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00},	 // 32-1280x768x85 
	{"1280x800@60",0x11,0x00,0x5d,0x10,0xE6,0x90,0xd0,0x02,0x40,0x33,0xc0},	// 33-1280x800x60 
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
UInt16 Device_adv7844_Read16_CPLD(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
//	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[2]={0};
	UInt16 data;
	
	regAddr[0] = RegAddr;
	OSA_CPLD_i2cRead16(&gDevice_adv7844CommonObj.i2cHandle,CPLD_IIC_SLAVE_ADDR, regAddr, regValue, 2);
	data = regValue[1]<<8 | regValue[0];
	
	return data;
}

UInt16 Device_adv7844_Write16_CPLD(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt16 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[2]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal&0xFF;
	regValue[1] = (RegVal&0xFF00)>>8;
	status = OSA_CPLD_i2cWrite16 (&gDevice_adv7844CommonObj.i2cHandle, CPLD_IIC_SLAVE_ADDR, regAddr, regValue, 2 );

	return status;
}

UInt8 Device_adv7844_Read8_IO_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_IO_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

static UInt8 Device_adv7844_Read8_CP_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_CP_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}
UInt8 Device_adv7844_Read8_SDP_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_SDP_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}

UInt8 Device_adv7844_Read8_AFE_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_AFE_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}


UInt8 Device_adv7844_Read8_Hdmimap(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_HDMI_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}
UInt8 Device_adv7844_Read8_SDP_IO_map(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_SDP_IO_MAP_ADDR, regAddr, regValue, 1);

	return regValue[0];
}
Int32 Device_adv7844_Write8_IO_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_IO_MAP_ADDR, regAddr, regValue, 1 );

	printf("###############status=%x ############# \n",status);
	return status;
}

Int32 Device_adv7844_Write8_CP_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_CP_MAP_ADDR, regAddr, regValue, 1 );
	printf("###############status=%x ############# \n",status);
	return status;
}

Int32 Device_adv7844_Write8_AFE_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_AFE_MAP_ADDR, regAddr, regValue, 1 );
	printf("###############status=%x ############# \n",status);
	return status;
}
Int32 Device_adv7844_Write8_Hdmimap(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_HDMI_MAP_ADDR, regAddr, regValue, 1 );
	printf("###############status=%x ############# \n",status);
	return status;
}

Int32 Device_adv7844_Write8_SDP_IO_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_SDP_IO_MAP_ADDR, regAddr, regValue, 1 );
	printf("###############status=%x ############# \n",status);
	return status;
}

Int32 Device_adv7844_Write8_SDP_MAP(Device_Adv7844Obj * pObj, UInt8 RegAddr, UInt8 RegVal)
{
	Int32 status = 0;
	UInt8 regAddr[1]={0};
	UInt8 regValue[1]={0};

	regAddr[0] = RegAddr;
	regValue[0] = RegVal;
	status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, ADV7844_SDP_MAP_ADDR, regAddr, regValue, 1 );
	printf("###############status=%x ############# \n",status);
	return status;
}


static int Device_adv7844_check_1366Or1280(Device_Adv7844Obj *pObj, int *index)
{
	Int32 	retVal = 0;
	Int32	lwidth=0;
	unsigned char data;

	ADV_OSA_printf("call Device_adv7844_check_1366Or1280() to check 1366 or 1280!\n");
	data = Device_adv7844_Read8_IO_MAP(pObj, 0xB5);
	if(gADV7844_TMDS){
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

static int Device_adv7844_check_VGAorVideo(Device_Adv7844Obj *pObj, int *index)
{
	Int32 	retVal = 0;
	unsigned char data;

	ADV_OSA_printf("call Device_adv7844_check_VGAorVideo() to check VGA or VIDEO!\n");
	data = Device_adv7844_Read8_IO_MAP(pObj, 0xB5);
	if(gADV7844_TMDS){
		*index = DEVICE_STD_720P_60;
	}else{
		if((data&0x03)==0x03)
			*index = DEVICE_STD_720P_60;
		else
			*index = DEVICE_STD_VGA_1280X720X60;
	}
	return retVal;
}

static int Device_adv7844_check_1080_DMTorGTF(Device_Adv7844Obj *pObj, int *index)
{
	Int32 	retVal = 0;
	unsigned char data;

//	OSA_printf("call Device_adv7844_check_1080_DMTorGTF() to check DMT or GTF!\n");
	data = Device_adv7844_Read8_CP_MAP(pObj, 0xB5);
	if(!gADV7844_TMDS){
		if(data&0x08)
			*index = DEVICE_STD_VGA_1920X1080X60_GTF;
		else
			*index = DEVICE_STD_VGA_1920X1080X60_DMT;

		if((data&0x03)==0x03)
			*index = DEVICE_STD_1080P_60;
	}
	return retVal;
}
#if 0
static Int32 Device_adv7844_check_SD_CH(Device_Adv7844Obj *pObj, int *inMode)
{
	Int32 		retVal=0;
	unsigned char  data;
	unsigned int	standard;

	if(gADV7844_Mode == DEVICE_STD_NTSC){
		*inMode = DEVICE_STD_NTSC;
		return 0;
	}
	Device_adv7844Reset(pObj);
#if 0
	//8-bit-27M
	Reg0 = Device_adv7844_Read8_IO_MAP(pObj, 0x00);
	OSA_printf("ADV7844 0x00 = 0x%x\n",Reg0);
	Device_adv7844_Write8_IO_MAP(pObj, 0x00, 0x01);
	Device_adv7844_Write8_IO_MAP(pObj, 0x05, 0x00);//
//	Device_adv7844_Write8_IO_MAP(pObj, 0x06, 0x02);//
//	Device_adv7844_Write8_IO_MAP(pObj, 0x07, 0xff);//
	Device_adv7844_Write8_IO_MAP(pObj, 0x03, 0x0C);//Disable TOD,10 bit 422 out through P19-P10
	Device_adv7844_Write8_IO_MAP(pObj, 0x3C, 0xAD);//Setup SOG Sync level for divided down SOG
	Device_adv7844_Write8_IO_MAP(pObj, 0x04, 0x47);//Enable SFL
	Device_adv7844_Write8_IO_MAP(pObj, 0x17, 0x41);//select SH1
	Device_adv7844_Write8_IO_MAP(pObj, 0x1D, 0x40);//Disable TRI_LLC
	Device_adv7844_Write8_IO_MAP(pObj, 0x31, 0x00);//Clears NEWAV_MODE, SAV/EAV  to suit ADV video encoders
	Device_adv7844_Write8_IO_MAP(pObj, 0x34, 0x01);
	Device_adv7844_Write8_IO_MAP(pObj, 0x35, 0x22);
	Device_adv7844_Write8_IO_MAP(pObj, 0x3A, 0x07);//Power down ADC 1 & ADC2 & ADC3
	Device_adv7844_Write8_IO_MAP(pObj, 0x3C, 0xA8);//SOG Sync level for atenuated sync, PLL Qpump to default
	Device_adv7844_Write8_IO_MAP(pObj, 0x47, 0x0A);//Enable Automatic PLL_Qpump and VCO Range
	Device_adv7844_Write8_IO_MAP(pObj, 0xBA, 0xA0);//Enable HDMI and Analog in
	Device_adv7844_Write8_IO_MAP(pObj, 0xF3, 0x07);//Enable Anti-Alias Filters
	Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x00);//Enable Anti-Alias Filters
#else
	//16-bit-13.5M
	Device_adv7844_Write8_IO_MAP(pObj, 0x00, 0x01);
	Device_adv7844_Write8_IO_MAP(pObj, 0x05, 0x00);//
//	Device_adv7844_Write8_IO_MAP(pObj, 0x06, 0x02);//
//	Device_adv7844_Write8_IO_MAP(pObj, 0x07, 0xff);//
	Device_adv7844_Write8_IO_MAP(pObj, 0x03, 0x09);//Disable TOD,10 bit 422 out through P19-P10
	Device_adv7844_Write8_IO_MAP(pObj, 0x3C, 0xAD);//Setup SOG Sync level for divided down SOG
	Device_adv7844_Write8_IO_MAP(pObj, 0x04, 0x47);//Enable SFL
	Device_adv7844_Write8_IO_MAP(pObj, 0x17, 0x41);//select SH1
	Device_adv7844_Write8_IO_MAP(pObj, 0x1D, 0x40);//Disable TRI_LLC
	Device_adv7844_Write8_IO_MAP(pObj, 0x31, 0x00);//Clears NEWAV_MODE, SAV/EAV  to suit ADV video encoders
	Device_adv7844_Write8_IO_MAP(pObj, 0x34, 0x01);
	Device_adv7844_Write8_IO_MAP(pObj, 0x35, 0x22);
	Device_adv7844_Write8_IO_MAP(pObj, 0x3A, 0x07);//Power down ADC 1 & ADC2 & ADC3
	Device_adv7844_Write8_IO_MAP(pObj, 0x3C, 0xA8);//SOG Sync level for atenuated sync, PLL Qpump to default
	Device_adv7844_Write8_IO_MAP(pObj, 0x47, 0x0A);//Enable Automatic PLL_Qpump and VCO Range
	Device_adv7844_Write8_IO_MAP(pObj, 0xBA, 0xA0);//Enable HDMI and Analog in
	Device_adv7844_Write8_IO_MAP(pObj, 0xF3, 0x07);//Enable Anti-Alias Filters
	Device_adv7844_Write8_IO_MAP(pObj, 0x8F, 0x50);
	Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x00);//Enable Anti-Alias Filters
#endif	
	usleep(1000*500);
	data = Device_adv7844_Read8_IO_MAP(pObj, 0x13);
	OSA_printf("ADV7844 0x13 = 0x%x\n",data);
	usleep(1000*300);
	while(1){
		data = Device_adv7844_Read8_IO_MAP(pObj, 0x13);
		OSA_printf("ADV7844 0x13 = 0x%x\n",data);
		sleep(2);
	}
	if(data & 0x1){
		standard = (data&0x70)>>4;
		if((standard == 0)||(standard == 1)){
			*inMode = DEVICE_STD_NTSC;
		}else{
			*inMode = DEVICE_STD_PAL;
		}
	}else{
		*inMode = -1;
	}

	*inMode = DEVICE_STD_NTSC;
	
	return retVal;
}
#endif

static int Device_ad7844_SetCPSAV_EAV(Device_Adv7844Obj *pObj, unsigned int sav, unsigned int eav)
{
	Int32 	retVal = 0;
	unsigned char data;

	if((sav == 0)&&(eav == 0))
		return retVal;
	if(gADV7844_TMDS)
		return 0;

	ADV_OSA_printf("sav = %d, eav = %d\n", sav, eav);

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

static int Device_ad7844_SetCPVBI(Device_Adv7844Obj *pObj, unsigned int s_vbi, unsigned int e_vbi)
{
	Int32 	retVal = 0;
	unsigned char data;
	
	if((s_vbi == 0)||(e_vbi == 0))
		return retVal;
	if(gADV7844_TMDS)
		return 0;
	
	ADV_OSA_printf("s_vbi = %d, e_vbi = %d\n", s_vbi, e_vbi);
	data = (s_vbi>>4)&0xFF;
	Device_adv7844_Write8_IO_MAP(pObj, 0xA5, data);

	data = (s_vbi&0xF)<<4 | ((e_vbi&0xF00)>>8);
	Device_adv7844_Write8_IO_MAP(pObj, 0xA6, data);
	
	data = e_vbi&0xFF;
	Device_adv7844_Write8_IO_MAP(pObj, 0xA7, data);
	return retVal;
}

#if 0
static int Device_adv7844_SetPhase(Device_Adv7844Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
	unsigned char data=0;

	if(inMode >= DEVICE_STD_VGA_1280X720X60)
		data = 0x10;
	else
		data = 0x00;
	Device_adv7844_Write8_IO_MAP(pObj, 0x6A, 0x00);
	Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x01);
	switch(inMode){
		case     DEVICE_STD_VGA_640X480X60:
		break;
		case     DEVICE_STD_VGA_640X480X72:
		break;
		case    DEVICE_STD_VGA_640X480X75:
		break;
		case    DEVICE_STD_VGA_640X480X85:
		break;
		case    DEVICE_STD_VGA_800X600X60:
		break;
		case    DEVICE_STD_VGA_800X600X72:
		break;
		case    DEVICE_STD_VGA_800X600X75:
		break;
		case    DEVICE_STD_VGA_800X600X85:
		break;
		case    DEVICE_STD_VGA_1024X768X60:
		break;
		case    DEVICE_STD_VGA_1024X768X70:
		break;
		case    DEVICE_STD_VGA_1024X768X75:
		break;
		case    DEVICE_STD_VGA_1024X768X85:
		break;
		case    DEVICE_STD_VGA_1280X720X60:
		break;
		case    DEVICE_STD_VGA_1280X768X60:
		break;
		case    DEVICE_STD_VGA_1280X768X75:
		break;
		case    DEVICE_STD_VGA_1280X768X85:
			Device_adv7844_Write8_IO_MAP(pObj, 0x6A, 0x00);
			Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x00);
		break;
		case    DEVICE_STD_VGA_1280X800X60:
			Device_adv7844_Write8_IO_MAP(pObj, 0x6A, 0x00);
			Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x00);
		break;
		case    DEVICE_STD_VGA_1280X960X60:
		break;
		case    DEVICE_STD_VGA_1280X1024X60:
		break;
		case    DEVICE_STD_VGA_1280X1024X75:
		break;
		case    DEVICE_STD_VGA_1280X1024X85:
		break;
		case    DEVICE_STD_VGA_1366X768X60:
			Device_adv7844_Write8_IO_MAP(pObj, 0x6A, 0x05);
			Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x00);
		break;
		case    DEVICE_STD_VGA_1440X900X60:
		break;
		case    DEVICE_STD_VGA_1400X1050X60:
			Device_adv7844_Write8_IO_MAP(pObj, 0x6A, 0xff);
			Device_adv7844_Write8_IO_MAP(pObj, 0x37, 0x01);
		break;
		case    DEVICE_STD_VGA_1400X1050X75:
		break;
		case    DEVICE_STD_VGA_1600X1200X60:
		break;
		case    DEVICE_STD_VGA_1920X1080X60_DMT:
		break;
		case    DEVICE_STD_VGA_1920X1080X60_GTF:
		break;
		case    DEVICE_STD_VGA_1920X1200X60:
		break;
		case    DEVICE_STD_VGA_2560X1440X60:
		break;
		default:
		break;
	}
	//Device_adv7844_Write8_IO_MAP(pObj, 0x6A, data);
	return retVal;
}


static Int32 Device_adv7844GetTMDS_A(Device_Adv7844Obj *pObj, unsigned int *tmds_a)
{
	int retVal=0;
	unsigned char data;
	Device_adv7844_Write8_Hdmimap(pObj, 0x01 ,0x00) ; //Enable clock terminators
	data = Device_adv7844_Read8_Hdmimap(pObj, 0x6A);	
	*tmds_a=(data&(0x01<<3))>>3;

	return retVal;
}

static Int32 Device_adv7844GetTMDS_B(Device_Adv7844Obj *pObj, unsigned int *tmds_b)
{
	int retVal=0;
	unsigned char data;

	data = Device_adv7844_Read8_Hdmimap(pObj, 0x04);	
	*tmds_b=(data&(0x01<<2))>>2;

	return retVal;
}
#endif
static inline int hdmi_read(Device_Adv7844Obj * pObj, UInt8 RegAddr)
{


        return Device_adv7844_Read8_Hdmimap(pObj, RegAddr);
}


static Int32 Device_adv7844GetInfo_D(Device_Adv7844Obj *pObj, ADV7844_SyncInfo *SyncInfo)
{
	Int32 	retVal = 0;
	unsigned vsync = 0 ;

	Int32 temp = Device_adv7844_Read8_IO_MAP(pObj, 0x6a);	
	SyncInfo->TMDS_A=(temp&(0x01<<3))>>3;
	SyncInfo->TMDS_B=(temp&(0x01<<7))>>7;


        //         int lock = hdmi_read(pObj, 0x04) & 0x02;
                 int interlaced = hdmi_read(pObj, 0x0b) & 0x20;
                 unsigned w = (hdmi_read(pObj, 0x07) & 0x1f) * 256 + hdmi_read(pObj, 0x08);
                 unsigned h = (hdmi_read(pObj, 0x09) & 0x1f) * 256 + hdmi_read(pObj, 0x0a);
                 unsigned w_total = (hdmi_read(pObj, 0x1e) & 0x3f) * 256 +
                         hdmi_read(pObj, 0x1f);
                 unsigned h_total = ((hdmi_read(pObj, 0x26) & 0x3f) * 256 +
                                     hdmi_read(pObj, 0x27)) / 2;
                 unsigned freq = (((hdmi_read(pObj, 0x51) << 1) +
                                         (hdmi_read(pObj, 0x52) >> 7)) * 1000000) +
                         ((hdmi_read(pObj, 0x52) & 0x7f) * 1000000) / 128;
		if(h_total&&w_total){
			vsync = (int) (((((freq*10)/h_total)/w_total)+5)/10);
			}
//		 int i;
 
                 if (1) {
                         /* adjust for deep color mode */
                         freq = freq * 8 / (((hdmi_read(pObj, 0x0b) & 0xc0)>>6) * 2 + 8);
						  printf("Call Device_adv7844GetInfo_D()!  freq2 =%d \n",freq);
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

static Int32 Device_adv7844GetInfo_A(Device_Adv7844Obj *pObj, int *hpv, int *vps)
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

static Int32 Device_adv7844GetInterface(Device_Adv7844Obj *pObj, int *tmds, ADV7844_INPUT *InterFace)
{
	Int32 	retVal = 0;
	unsigned char regVal=0;
	
	if(*tmds){
		*InterFace = ADV7844_DIGITAL_INPUT;
	}else{
		regVal = Device_adv7844_Read8_IO_MAP(pObj, 0x13);
		if((regVal & 0x3) == 0x3)
			*InterFace = ADV7844_YPbPr_INPUT;
		else
			*InterFace = ADV7844_ANALOG_INPUT;
	}
	return retVal;
}
static Int32 Device_adv7844GetModeIndex(int hpv, int vps, int *index)
{
	Int32 	retVal = 0;
	int i=-1;
	
	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((vps>adv7844_gLineNum[i].vps-3)&&(vps<adv7844_gLineNum[i].vps+3))&&((hpv>adv7844_gLineNum[i].hpv-4)&&(hpv<adv7844_gLineNum[i].hpv+4))){
			break;
		}

	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((vps>adv7844_gLineNum[i].vps-3)&&(vps<adv7844_gLineNum[i].vps+3))&&((hpv>adv7844_gLineNum[i].hpv-10)&&(hpv<adv7844_gLineNum[i].hpv+10))){
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

static Int32 Device_adv7844GetModeIndexSD(Device_Adv7844Obj *pObj, int *inMode)
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

static char Device_adv7844GetLineWidth(Device_Adv7844Obj *pObj, short int  *w_value)
{
	char b_data;
	b_data=Device_adv7844_Read8_Hdmimap(pObj, 0x07);
	*w_value = ((b_data & 0xF) << 8);
	b_data=Device_adv7844_Read8_Hdmimap(pObj,  0x08);

	*w_value += (b_data & 0xFF);
	return 0;
}

static Int32 Device_adv7844InitComm(Device_Adv7844Obj *pObj, int tmds)
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
		Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0x7F) ; //Disable Tristate of Pins
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

	return retVal;
}

static Int32 Device_adv7844SDInterFaceInit(Device_Adv7844Obj *pObj)
{
	Int32 		retVal=0;
//	unsigned char Reg0, data;
//	unsigned int	standard;
	int var ;
	var = Device_adv7844_Read16_CPLD(pObj,  0x18);
	var = var|0x01;
	Device_adv7844_Write16_CPLD(pObj,  0x18 ,var) ;

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
	Device_adv7844_Write8_IO_MAP(pObj,  0x14 ,0x77) ; //Power up pads
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

static Int32 Device_adv7844DigitalInterFaceInit(Device_Adv7844Obj *pObj)
{
#if 1
	Device_adv7844_Write8_IO_MAP(pObj, 0x00,0X05) ; //Prim_Mode =110b HDMI-GR
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
	Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0x7F) ; //Disable Tristate of Pins
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

static Int32 Device_adv7844DigitalInterFaceSetRes(Device_Adv7844Obj *pObj, int inMode)
{

OSA_printf("Get Information ArgMode_7844_2[inMode].config[1]= %x \n",ArgMode_7844_2[inMode].config[1]);
	Device_adv7844_Write8_IO_MAP(pObj,  0x00,ArgMode_7844_2[inMode].config[3]);//; PLL Divide         
	//Device_adv7844_Write8_IO_MAP(pObj, 0x00,0x013); //Prim_Mode =110b HDMI-GR
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
	Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0x7F) ; //Disable Tristate of Pins
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
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[5]= %x \n",ArgMode_7844_2[inMode].config[4]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[6]= %x \n",ArgMode_7844_2[inMode].config[5]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[7]= %x \n",ArgMode_7844_2[inMode].config[7]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[8]= %x \n",ArgMode_7844_2[inMode].config[8]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[9]= %x \n",ArgMode_7844_2[inMode].config[9]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[10]= %x \n",ArgMode_7844_2[inMode].config[10]);

	if(ArgMode_7844_2[inMode].config[1])
		Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,ArgMode_7844_2[inMode].config[2]) ; //Prim_Mode =110b HDMI-GR
	if(!ArgMode_7844_2[inMode].config[1]){
		Device_adv7844_Write8_IO_MAP(pObj,  0x00,0x00);
		Device_adv7844_Write8_IO_MAP(pObj,  0x16,ArgMode_7844_2[inMode].config[4]);//; PLL Divide                                                              
		Device_adv7844_Write8_IO_MAP(pObj,  0x17,ArgMode_7844_2[inMode].config[5]);//; PLL Divide 
		Device_adv7844_Write8_CP_MAP(pObj,  0x8F,ArgMode_7844_2[inMode].config[7]);//; Set Free                                                         
		Device_adv7844_Write8_CP_MAP(pObj,  0x90,ArgMode_7844_2[inMode].config[8]);//; Set Free 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAB,ArgMode_7844_2[inMode].config[9]);//; Set line 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAC,ArgMode_7844_2[inMode].config[10]);//; Set line 
		}
	return 0;
}

static Int32 Device_adv7844AnalogInterFaceInit(Device_Adv7844Obj *pObj)
{
	Int32 	retVal = 0;
	Device_adv7844_Write8_IO_MAP(pObj, 0XFF ,0Xff );// I2C reset
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

static Int32 Device_adv7844AnalogInterFaceSetRes(Device_Adv7844Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
	unsigned char regVal=0;
	OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[0]= %x \n",ArgMode_7844_2[inMode].config[0]);
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
	Device_adv7844_Write8_IO_MAP(pObj, 0X00 ,ArgMode_7844_2[inMode].config[0] );// VID_STD=01000b for VGA60
	Device_adv7844_Write8_IO_MAP(pObj, 0X01 ,0X02 );// Prim_Mode to graphics input
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
	if(inMode < DEVICE_STD_VGA_640X480X60){
		Device_adv7844_Write8_IO_MAP(pObj, 0x01 ,ArgMode_7844_2[inMode].config[2]) ; //Prim_Mode =110b HDMI-GR
		Device_adv7844_Write8_IO_MAP(pObj, 0X02 ,0XF4 );// Auto input color space, Limited Range RGB Output
		Device_adv7844_Write8_CP_MAP(pObj , 0X3e ,0X0c );// Set manual gain of 0x2A8
		}
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[5]= %x \n",ArgMode_7844_2[inMode].config[4]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[6]= %x \n",ArgMode_7844_2[inMode].config[5]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[7]= %x \n",ArgMode_7844_2[inMode].config[7]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[8]= %x \n",ArgMode_7844_2[inMode].config[8]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[9]= %x \n",ArgMode_7844_2[inMode].config[9]);
OSA_printf("Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[10]= %x \n",ArgMode_7844_2[inMode].config[10]);

	if(!ArgMode_7844_2[inMode].config[0]){
		Device_adv7844_Write8_IO_MAP(pObj, 0x02, 0xF6) ; //Auto input color space, Limited Range RGB Output
		Device_adv7844_Write8_IO_MAP(pObj, 0x03, 0x40) ; //24 bit SDR 444
		//Device_adv7844_Write8_IO_MAP(pObj, 0x05, 0x28) ; //Disable AV Codes
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
		Device_adv7844_Write8_IO_MAP(pObj,  0x16, 0xC6) ; //set PLL for 1600 samples per line
		Device_adv7844_Write8_IO_MAP(pObj,  0x17, 0x40) ; //set PLL for 1600 samples per line
		Device_adv7844_Write8_CP_MAP(pObj ,0x81, 0xD0) ; //enable blue screen freerun in autographics mode
		Device_adv7844_Write8_CP_MAP(pObj , 0x8B, 0x4C) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj , 0x8C, 0x02) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj , 0x8B, 0x4C) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj ,0x8D, 0x40) ; //set horizontal DE start/end position
		Device_adv7844_Write8_CP_MAP(pObj ,0x8F, 0x02) ; //configure freerun parameters FR_LL
		Device_adv7844_Write8_CP_MAP(pObj , 0x90, 0x04) ; //configure freerun parameters FR_LL
		Device_adv7844_Write8_CP_MAP(pObj , 0x91, 0x00) ; //set CP core to progressive mode
		Device_adv7844_Write8_CP_MAP(pObj , 0xA5, 0x39) ; //set vertical DE start/end position (odd/prog)
		Device_adv7844_Write8_CP_MAP(pObj , 0xA6, 0xC0) ; //set vertical DE start/end position (odd/prog)
		Device_adv7844_Write8_CP_MAP(pObj ,0xA7, 0x18) ; //set vertical DE start/end position (odd/prog)
		Device_adv7844_Write8_CP_MAP(pObj ,0xA8, 0x1C) ; //set vertical DE start/end position (even)
		Device_adv7844_Write8_CP_MAP(pObj , 0xA9, 0xD1) ; //set vertical DE start/end position (even)
		Device_adv7844_Write8_CP_MAP(pObj , 0xAA, 0xDA) ; //set vertical DE start/end position (even)
		Device_adv7844_Write8_CP_MAP(pObj , 0xAB, 0x39) ; //configure freerun parameter LCOUNT_MAX
		Device_adv7844_Write8_CP_MAP(pObj , 0xAC, 0xE0) ; //configure freerun parameter LCOUNT_MAX
		Device_adv7844_Write8_IO_MAP(pObj,  0x16,ArgMode_7844_2[inMode].config[4]);//; PLL Divide                                                              
		Device_adv7844_Write8_IO_MAP(pObj,  0x17,ArgMode_7844_2[inMode].config[5]);//; PLL Divide 
		Device_adv7844_Write8_CP_MAP(pObj,  0x8F,ArgMode_7844_2[inMode].config[7]);//; Set Free                                                         
		Device_adv7844_Write8_CP_MAP(pObj,  0x90,ArgMode_7844_2[inMode].config[8]);//; Set Free 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAB,ArgMode_7844_2[inMode].config[9]);//; Set line 
		Device_adv7844_Write8_CP_MAP(pObj,  0xAC,ArgMode_7844_2[inMode].config[10]);//; Set line 
		}
	return retVal;
}

static Int32 Device_adv7844YPbPrInterFaceSetRes(Device_Adv7844Obj *pObj, int inMode)
{
	Int32 	retVal = 0;
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
	Device_adv7844_Write8_IO_MAP(pObj, 0X00 ,ArgMode_7844_2[inMode].config[0] );// VID_STD=01000b for VGA60
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


static Int32 Device_adv7844Config(Device_Adv7844Obj *pObj, int inMode, int tmds, ADV7844_INPUT InterFace)
{
	Int32 	retVal = 0;
//	unsigned int regVal=0;
 	int val;
	ADV_OSA_printf("call Device_adv7844Config() inMode = %d, tmds = %d, InterFace = %d\n",inMode,tmds,InterFace);


	switch(InterFace){
		case ADV7844_DIGITAL_INPUT:
			 Device_adv7844Reset(pObj);
			Device_adv7844DigitalInterFaceInit(pObj);
			Device_adv7844DigitalInterFaceSetRes(pObj, inMode);
			break;
		case ADV7844_ANALOG_INPUT:
			if(ArgMode_7844_2[inMode].config[0]){
				//Device_adv7844Reset(pObj);
				usleep(100000);
				Device_adv7844AnalogInterFaceInit(pObj);
				usleep(100000);
			}
			Device_adv7844AnalogInterFaceSetRes(pObj, inMode);
			break;
		case ADV7844_YPbPr_INPUT:
			Device_adv7844YPbPrInterFaceSetRes(pObj, inMode);
				break;
		default:
			ADV_OSA_printf("ADV7844 Unknow Interface in Device_adv7844Config()!\n");
	}


	val = Device_adv7844_Read16_CPLD(pObj,  0x18);
	if((inMode==DEVICE_STD_1080I_60)||(inMode==DEVICE_STD_1080I_50)){
		val=val|0x02;
		Device_adv7844_Write8_IO_MAP(pObj, 0x05 ,0x2c) ; //AV Codes On
		}
	else{
		val = val&0xffed;
		Device_adv7844_Write8_IO_MAP(pObj, 0x05 ,0x28) ; //AV Codes On
		}
	val = val&0xfff6;
	Device_adv7844_Write16_CPLD(pObj,  0x18 ,val) ;


	return retVal;
}

#if 0
static Int32 Device_adv7844GetResolution_CPLD(Device_Adv7844Obj *pObj)
{
	Int32 	hsfq = 0, linenum=0;
	int i=0, inMode=0;;
	hsfq  = Device_adv7844_Read16_CPLD(pObj, 8);
	linenum  = Device_adv7844_Read16_CPLD(pObj, 10);

	for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
		if(((hsfq>adv7844_gLineNum[i].lps-10)&&(hsfq<adv7844_gLineNum[i].lps+10))&&((linenum>adv7844_gLineNum[i].hpv-4)&&(linenum<adv7844_gLineNum[i].hpv+4))){
			break;
		}

	}

	if(i == DEVICE_STD_REACH_LAST){
		for(i = 0;i<DEVICE_STD_REACH_LAST;i++){
			if(((hsfq>adv7844_gLineNum[i].lps-10)&&(hsfq<adv7844_gLineNum[i].lps+10))&&((linenum>adv7844_gLineNum[i].hpv-10)&&(linenum<adv7844_gLineNum[i].hpv+10))){
				break;
			}
	    	}
	}

	if(i==DEVICE_STD_REACH_LAST)
		inMode = -1;
	else
		inMode = i;

	gADV7844_CPLD_Hpv = linenum;
	gADV7844_CPLD_Lps = hsfq;
	gADV7844_CPLD_Mode = inMode;

	return inMode;
}

static Int32 Device_adv7844GetResolutionCVBS(Device_Adv7844Obj *pObj)
{
//	Int32	tmds_a=-1;
	int 		inMode;
//	unsigned char Fdata;
//	unsigned char data;
		return 0;

	gADV7844_Mode = inMode;
}

static Int32 Device_adv7844GetResolutionCP(Device_Adv7844Obj *pObj)
{
//	Int32 	retVal = 0;
//	Int32	hpv=0, vps=0, tmds_a=-1;
//	int 		inMode;
//	int 		retry = RETRYNUMS;
//	ADV7844_SyncInfo sysinformation;
//	unsigned char regVal=0;
//	short int  width;
//	unsigned short Fdata;
//	unsigned char data;
	return 0;

//	return (retVal);
	
}
#endif

static Int32 Device_adv7844GetResolutionALL(Device_Adv7844Obj *pObj)
{
	Int32 	retVal = 0;
	Int32	hpv=0, vps=0;
	int 		inMode;
	int 		retry = RETRYNUMS;
	ADV7844_SyncInfo sysinformation;
	unsigned char regVal=0;
	short int   width; 
 	int var ;
	//gADV7844_Mode=DEVICE_STD_480I;
	printf("Call Device_adv78##########################################44_2GetResolutionALL()!\n");
	var = Device_adv7844_Read8_CP_MAP(pObj, 0xff);
	if(var & 0x10){
		ADV_OSA_printf("We Must Reset ADV7442 Becase of FreeRun!var=%x @@@@@@@@\n",var);
		gADV7844_CurStatus = -1;
		gADV7844_Mode = -1;
		gADV7844_TMDS = -1;
		gADV7844_Hpv= -1;
		gADV7844_Vps= -1;
		gADV7844_InterFace = ADV7844_INPUT_UNKNOW;

		gADV7844_CPLD_Mode = -1;
		gADV7844_CPLD_Hpv= -1;
		gADV7844_CPLD_Lps= -1;
	}


	while(retry--){
 

		

		Device_adv7844GetInfo_D(pObj , &sysinformation);
		if(gADV7844_TMDS != sysinformation.TMDS_A){
			gADV7844_TMDS = sysinformation.TMDS_A;
			Device_adv7844InitComm(pObj, sysinformation.TMDS_A);
					usleep(100000);
		}

		OSA_printf("Get Information gADV7844_TMDS A= %x \n",gADV7844_TMDS);
		if(gADV7844_TMDS){
			//Digital input
			Device_adv7844GetInfo_D(pObj , &sysinformation);
			hpv=sysinformation.VTotPix;
			vps=sysinformation.VFreq;
	//		OSA_printf("Get Information From Digital input!\n");
		}else{
			//Analog input
			Device_adv7844GetInfo_A(pObj,&hpv,&vps);
			OSA_printf("Get Information From Analog input!\n");
		}

		gADV7844_Hpv = hpv;
		gADV7844_Vps = vps;
		OSA_printf("Get Information hpv = %d  vps=%d  \n",hpv,vps);
		Device_adv7844GetModeIndex(gADV7844_Hpv, gADV7844_Vps, &inMode);

//////////////////////////////////
		if((inMode == DEVICE_STD_VGA_1366X768X60)||(inMode == DEVICE_STD_VGA_1280X768X60))
			Device_adv7844_check_1366Or1280(pObj, &inMode);
		if((inMode == DEVICE_STD_VGA_1280X720X60)||(inMode == DEVICE_STD_720P_60))
			Device_adv7844_check_VGAorVideo(pObj, &inMode);
		if((inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_VGA_1920X1080X60_DMT)||(inMode == DEVICE_STD_1080P_60))
			Device_adv7844_check_1080_DMTorGTF(pObj, &inMode);
//////////////////////////////////	

		if((inMode == gADV7844_Mode)&&(inMode != -1))
			break;
		usleep(200000);
	}

	if(gADV7844_TMDS&&(inMode==-1)){
		Device_adv7844GetLineWidth(pObj,&width);
		if(((width>637)&&(width<645))){
			inMode=gADV7844_Mode;
		}
		if(((width>795)&&(width<805))){
			inMode=gADV7844_Mode;
		}
	}

	if((gADV7844_Mode != inMode)&&(inMode != -1)){
		gADV7844_Mode = inMode;
		Device_adv7844GetInterface(pObj, &gADV7844_TMDS, &gADV7844_InterFace);
		Device_adv7844Config(pObj, gADV7844_Mode, gADV7844_TMDS, gADV7844_InterFace);
		if(gADV7844_Mode==DEVICE_STD_NTSC||gADV7844_Mode==DEVICE_STD_PAL){
				Device_adv7844SDInterFaceInit(pObj);
				Device_adv7844GetModeIndexSD(pObj, &inMode);
			}
		
	}else{
		if((inMode == -1)&&(gADV7844_TMDS == 0)){
			Device_adv7844SDInterFaceInit(pObj);
			Device_adv7844GetModeIndexSD(pObj, &inMode);
			if(inMode == -1){
				Device_adv7844Reset(pObj);
			}
		}
	
		gADV7844_Mode = inMode;
	}

	if(gADV7844_Mode != -1){
#ifdef DEVICE_ADV7844_DEBUG		
	//	Device_adv7844GetCPLDVersion(pObj);
		printf("###########################################\n");
		printf("Get ADV7844 Resolution!\n");
		printf("gADV7844_std = %s \n",ArgMode_7844_2[gADV7844_Mode].name);
		printf("gADV7844_hpv = %d gADV7844_vps = %d \n",gADV7844_Hpv,gADV7844_Vps);
		printf("gADV7844_CPLD_hpv = %d gADV7844_CPLD_lps = %d gADV7844_CPLD_Mode = %d\n",gADV7844_CPLD_Hpv,gADV7844_CPLD_Lps,gADV7844_CPLD_Mode);
		printf("gADV7844_Mode = %d gADV7844_TMDS = %d gADV7844_InterFace = %d\n",gADV7844_Mode,gADV7844_TMDS,gADV7844_InterFace);
		printf("###########################################\n");
#endif
	}else{
#ifdef 	DEVICE_ADV7844_DEBUG
		//Device_adv7844GetCPLDVersion(pObj);
		printf("###########################################\n");
		printf("Get ADV7844 Resolution!\n");
		printf("gADV7844_std = unknow \n");
		printf("gADV7844_hpv = %d gADV7844_vps = %d \n",gADV7844_Hpv,gADV7844_Vps);
		printf("gADV7844_CPLD_hpv = %d gADV7844_CPLD_lps = %d gADV7844_CPLD_Mode = %d\n",gADV7844_CPLD_Hpv,gADV7844_CPLD_Lps,gADV7844_CPLD_Mode);
		printf("gADV7844_Mode = %d gADV7844_TMDS = %d gADV7844_InterFace = %d\n",gADV7844_Mode,gADV7844_TMDS,gADV7844_InterFace);
		printf("###########################################\n");
#endif		
	}
if(gADV7844_Mode==DEVICE_STD_NTSC||gADV7844_Mode==DEVICE_STD_PAL){

	var = Device_adv7844_Read16_CPLD(pObj,  0x18);
	var = var|0x01;
	Device_adv7844_Write16_CPLD(pObj,  0x18 ,var) ;
	regVal = Device_adv7844_Read8_SDP_MAP(pObj, 0x56);

	if(regVal & 0x01){
		ADV_OSA_printf("We Must Reset ADV7442 Becase of FreeRun!\n");
		Device_adv7844Reset(pObj);
		gADV7844_CurStatus = -1;
		gADV7844_Mode = -1;
		gADV7844_TMDS = -1;
		gADV7844_Hpv= -1;
		gADV7844_Vps= -1;
		gADV7844_InterFace = ADV7844_INPUT_UNKNOW;

		gADV7844_CPLD_Mode = -1;
		gADV7844_CPLD_Hpv= -1;
		gADV7844_CPLD_Lps= -1;
	}
}
else{

	var = Device_adv7844_Read16_CPLD(pObj,  0x18);
	var= var&0xfff6;
	Device_adv7844_Write16_CPLD(pObj,  0x18 ,var) ;


}
	return (retVal);
}

static Int32 Device_adv7844GetResolution(Device_Adv7844Obj *pObj)
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
			printf("Error gADV7844_Source_Ch Index Unknow!\n");
	}
	return 0;
}

Int32 Device_adv7844GetExternInformation(Device_Adv7844Obj * pObj,
							Device_VideoDecoderExternInforms* extenInfor)
{
	Int32 status = 0;
	
	memcpy(extenInfor->DeviceName, "adv7844", sizeof(extenInfor->DeviceName));
	extenInfor->ModeID = gADV7844_Mode;
	extenInfor->SignalTmds = gADV7844_TMDS;
	extenInfor->SignalHpv = gADV7844_Hpv;
	extenInfor->SignalFreq = gADV7844_Vps;
	extenInfor->SignalYPbPr = gADV7844_InterFace;
	extenInfor->SignalHsfqFpga = gADV7844_CPLD_Lps;
	extenInfor->SignalLinenumFpga = gADV7844_CPLD_Hpv;
	
	return status;
}

Int32 Device_adv7844GetHDCP(Device_Adv7844Obj * pObj, Int32 *hdcp)
{
	UInt16           regVal = 0;
	regVal = Device_adv7844_Read8_Hdmimap(pObj, 0x05);
	*hdcp = (regVal&(1<<6));
	return		0;
}

/* Set ADV7844 mode based on

  - mux mode - line or pixel or no-mux
  - number of channels
  - resolution
  - 8/16-bit data width
  - NTSC/PAL standard
  - cascade mode of operation
*/
Int32 Device_adv7844SetVideoMode ( Device_Adv7844Obj * pObj,
                                Device_VideoDecoderVideoModeParams * pPrm )
{
    Int32 status = 0;

    return status;
}

Int32 Device_adv7844GetCPLDVersion(Device_Adv7844Obj * pObj)
{
	UInt16           regVal = 0;
	regVal = Device_adv7844_Read16_CPLD(pObj, 16);
//	OSA_printf("CPLD Version = 0x%x\n",regVal);	
	return 0;
}


/* reset ADV7844 OFM logic  */
Int32 Device_adv7844Reset ( Device_Adv7844Obj * pObj )
{
	Int32           retVal = 0;
//	UInt8           regVal;

	OSA_printf("###################Device_adv7844Reset########################\n");

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
	Device_adv7844_Write8_IO_MAP(pObj, 0x0C ,0x40) ; //Power up part and power down VDP
	Device_adv7844_Write8_IO_MAP(pObj, 0x14 ,0x7b) ; //Disable Tristate of Pins
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


return (retVal);    
}

/*
  Enable ADV7844 output port
*/
Int32 Device_adv7844OutputEnable ( Device_Adv7844Obj * pObj, UInt32 enable )
{
	Int32 status = 0;
	return status;
}

/* start/enable output  */
Int32 Device_adv7844Start ( Device_Adv7844Obj * pObj )
{
    Int32 status = 0;

    status = Device_adv7844OutputEnable ( pObj, TRUE );

    return status;
}

/* stop/disable output  */
Int32 Device_adv7844Stop ( Device_Adv7844Obj * pObj )
{
    Int32 status = 0;

    status = Device_adv7844OutputEnable ( pObj, FALSE );

    return status;
}

/*
  Get ADV7844 chip ID, revision ID and firmware patch ID
*/
Int32 Device_adv7844GetChipId ( Device_Adv7844Obj * pObj,
                             Device_VideoDecoderChipIdParams * pPrm,
                             Device_VideoDecoderChipIdStatus * pStatus )
{
    Int32 status = 0;

    return status;
}

Int32 Device_adv7844SetDirection ( Device_Adv7844Obj * pObj,
                             unsigned int *direction )
{
	Int32 status = 0;
	Int32 inMode = 0,tmds=0;
	Int32 val;
	Int32 sav,eav,s_vbi,e_vbi;

	inMode = gADV7844_Mode;
	tmds = gADV7844_TMDS;
	val = *direction;

	if((tmds)||(inMode == -1))return 0;

	sav = (val&0xFFFF0000)>>16;
	eav = sav+6+adv7844_gLineNum[inMode].width;
	e_vbi = val&0xFFFF;
	s_vbi = e_vbi+adv7844_gLineNum[inMode].hight;
	
	adv7844_SavEavVbi[inMode].sav = 	sav;
	adv7844_SavEavVbi[inMode].eav = 	eav;
	adv7844_SavEavVbi[inMode].s_vbi = s_vbi;
	adv7844_SavEavVbi[inMode].e_vbi = e_vbi;

	Device_ad7844_SetCPSAV_EAV(pObj, sav, eav);
	Device_ad7844_SetCPVBI(pObj, s_vbi, e_vbi);

	return status;
}

Int32 Device_adv7844GetDirection ( Device_Adv7844Obj * pObj,
                             unsigned int *direction )
{
	Int32 status = 0;
	Int32 inMode = 0,tmds=0;
	Int32 val;
	Int32 sav,e_vbi;

	inMode = gADV7844_Mode;
	tmds = gADV7844_TMDS;
	val = *direction;

	if((tmds)||(inMode == -1))return 0;
	
	sav = adv7844_SavEavVbi[inMode].sav;
	e_vbi = adv7844_SavEavVbi[inMode].e_vbi;
	val = (((sav&0xFFFF)<<16) | (e_vbi&0xFFFF));

	*direction = val;
	
	return status;
}

Int32 Device_adv7844SetSourceChan ( Device_Adv7844Obj * pObj,
						unsigned int *source)
{
	int ret=0;
	
	if(gADV7844_Source_Ch == *source){
		return 0;
	}
	
	gADV7844_Source_Ch = *source;
	switch(gADV7844_Source_Ch){
		case DEVICE_SOURCE_ALL:
			Device_adv7844Reset(pObj);
			gADV7844_Mode = -1;
			break;
		case DEVICE_SOURCE_CP:
			Device_adv7844Reset(pObj);
			gADV7844_Mode = -1;
			break;
		case DEVICE_SOURCE_CVBS:
			Device_adv7844Reset(pObj);
			Device_adv7844SDInterFaceInit(pObj);
			gADV7844_Mode = -1;
			break;
		default:
			ret = -1;
			printf("Device_adv7844SetSourceChan() Set Source Chan Error!\n");
	}

	return ret;
}

Int32 Device_adv7844GetSourceChan ( Device_Adv7844Obj * pObj,
						unsigned int *source)
{
	*source = gADV7844_Source_Ch;
	return 0;
}


Int32 Device_adv7844InvertCbCr ( Device_Adv7844Obj * pObj)
{
	unsigned int ret=0;
	unsigned char data;
	
	data = Device_adv7844_Read8_IO_MAP(pObj, 0x86);
	if(data & (1<<4)){
		data = data&(~(1<<4));
	}else{
		data = data|(1<<4);
	}
	Device_adv7844_Write8_IO_MAP(pObj, 0x86, data);

	return ret;
}

Int32 Device_adv7844_CSC ( Device_Adv7844Obj * pObj,unsigned char *data)
{
	unsigned int ret=0;


	Device_adv7844_Write8_IO_MAP(pObj, 0x68, *data);

	return ret;
}

/*
  Get ADV7844 detect vide standard status

  Can be called for each channel
*/
Int32 Device_adv7844GetVideoStatus ( Device_Adv7844Obj * pObj,
                                     VCAP_VIDEO_SOURCE_STATUS_PARAMS_S * pPrm,
                                     VCAP_VIDEO_SOURCE_CH_STATUS_S     * pStatus )
{
	Int32 status = 0;
	
	Device_adv7844GetResolution(pObj);
	if(gADV7844_Mode != -1){
		pStatus->isVideoDetect = TRUE;
		pStatus->frameWidth = adv7844_gLineNum[gADV7844_Mode].width;
		pStatus->frameHeight = adv7844_gLineNum[gADV7844_Mode].hight;
		pStatus->isInterlaced = FALSE;
		
		if(gADV7844_Mode < DEVICE_STD_CIF)	
			pStatus->isInterlaced = TRUE;
		if(gADV7844_Mode >DEVICE_STD_720P_50 &&  gADV7844_Mode <DEVICE_STD_1080P_60)	
			pStatus->isInterlaced = FALSE;
	}else{
		pStatus->isVideoDetect = FALSE;
	}

	return status;
}

/*
  Set video color related parameters
*/
Int32 Device_adv7844SetVideoColor ( Device_Adv7844Obj * pObj,
                                 Device_VideoDecoderColorParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/*
  Set video noise filter related parameters
*/
Int32 Device_adv7844SetVideoNf ( Device_Adv7844Obj * pObj,
                              Device_Adv7844VideoNfParams * pPrm )
{
    Int32 status = 0;
    return status;
}

/* write to I2C registers */
Int32 Device_adv7844RegWrite ( Device_Adv7844Obj * pObj,
                            Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cWrite8 (&gDevice_adv7844CommonObj.i2cHandle, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs );

    return status;
}

/* read from I2C registers */
Int32 Device_adv7844RegRead ( Device_Adv7844Obj * pObj,
                           Device_VideoDecoderRegRdWrParams * pPrm )
{
    Int32 status = 0;
    Device_VideoDecoderCreateParams *pCreateArgs;

    if ( pPrm == NULL )
        return -1;

    pCreateArgs = &pObj->createArgs;

    if ( pPrm->deviceNum > pCreateArgs->numDevicesAtPort )
        return -1;

    status = OSA_i2cRead8 (&gDevice_adv7844CommonObj.i2cHandle, pCreateArgs->deviceI2cAddr[0], pPrm->regAddr, pPrm->regValue8, pPrm->numRegs);

    return status;
}

