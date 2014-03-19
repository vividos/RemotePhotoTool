#ifndef _CD_TYPE_H_
#define _CD_TYPE_H_


#ifdef _MSC_VER
#	if	900 <= _MSC_VER
	//#	pragma message( "push, _CD_TYPE_H_PACK_, 1" )
	#	pragma pack( push, _CD_TYPE_H_PACK_, 1 )
	#	pragma warning( disable : 4250 4355 4244 4005)
#	endif	/* 900 <= _MSC_VER */
#elif defined __WATCOMC__
	#pragma pack(__push,1);
#endif

#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=mac68k
 #endif
#endif

/****************************************************************************
 ****************************************************************************
 *            Canon Digital Camera SDK : Data Type Definitions              *
 ****************************************************************************
 ****************************************************************************/

/* Basic Data Types */
typedef unsigned char		cdUInt8;
typedef			 char		cdInt8;
typedef          char		cdChar;
#ifndef macintosh
typedef unsigned short		cdWChar;
#endif
typedef unsigned short		cdUInt16;
typedef          short		cdInt16;
typedef unsigned long		cdUInt32;
typedef          long		cdInt32; 
typedef          float		cdFloat32;
#ifdef macintosh
	typedef UInt64				cdUInt64;
#else
	typedef unsigned __int64	cdUInt64;
#endif
typedef cdUInt16 			cdBoolean;
typedef void				cdVoid;
typedef cdUInt32			cdTime;
typedef cdUInt32		 	cdError;
typedef cdUInt32 			cdEventID;  
typedef cdUInt32			cdReleaseEventID;
typedef cdUInt32			cdContext;

#ifdef macintosh
typedef FSSpec				cdFSSpec;
#endif

/* Handles */
#ifdef macintosh
	typedef cdUInt32		cdHWND;
#else
	typedef HWND			cdHWND;
#endif
typedef cdUInt32			cdHandle;
typedef cdHandle 			cdHSource;
typedef cdHSource			cdHLocalHost;	/* Handle for local strage */
typedef cdHandle 			cdHItem;
typedef cdHItem				cdHVolume;
typedef cdHItem				cdHFolder;
typedef cdHItem				cdHImageItem;
typedef cdHandle			cdHImageData;
typedef cdHandle			cdHEnum;
typedef cdHandle			cdHRAWData;

/* Strings */
typedef cdChar	cdStr31[32];
typedef cdChar	cdStr63[64];
typedef cdChar	cdStr255[256];

/* Rationals */
typedef struct{
	cdUInt32	Numerator;
	cdUInt32	Denominator;
}cdURational;

typedef struct{
	cdInt32		Numerator;
	cdInt32		Denominator;
}cdRational;


#ifdef macintosh
	#define cdSTDCALL	
#else
	#define cdSTDCALL	__stdcall
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CDSDK_EXPORT
#define cdCAPI cdError __declspec(dllexport) cdSTDCALL
#else
#define cdCAPI cdError __declspec(dllimport) cdSTDCALL
#endif

#ifdef __cplusplus
}
#endif


/****************************************************************************
 ****************************************************************************
 *                 Canon Digital Camera SDK : Constants                     *
 ****************************************************************************
 ****************************************************************************/

/* Max */
#define cdMAX_VOL						64
#define cdMAX_PATH						512
#define cdANY							1
#define cdMAX_TRANSFER_PATH_LENGTHT		2 /*(32)*/
#define CD_MAX_VOL						cdMAX_VOL
#define CD_MAX_PATH						cdMAX_PATH

typedef cdChar	cdVolName[cdMAX_VOL];

/* cdSourceType 
    Type of the sorce device */
typedef cdUInt32 cdSourceType;
	#define cdSRC_TYPE_INVALID 			((cdSourceType)0)
	#define cdSRC_TYPE_HOST  			((cdSourceType)1)
	#define cdSRC_TYPE_CAMERA  			((cdSourceType)2)

/* cdPortType 
    Type of the port. */
typedef cdUInt32  cdPortType;
	#define cdPORT_TYPE_NO_PORT  		((cdPortType )0)
	#ifdef macintosh 
	#define cdPORT_TYPE_USB   			((cdPortType )1)
	#define cdPORT_TYPE_IEEE1394 		((cdPortType )2)
	#define cdPORT_TYPE_USB_PLURAL   	((cdPortType )5)
	#define cdPORT_TYPE_IEEE1394_PLURAL	((cdPortType )6)
	#else
	#define cdPORT_TYPE_STI				((cdPortType )3)
	#define cdPORT_TYPE_WIA				((cdPortType )4)
	#endif
	#define cdPORT_TYPE_ANY				((cdPortType )0x7FFFFFFF)

/* cdDevDataType  */
typedef cdUInt32	cdDevDataType;	
	#define cdDEV_DATA_TYPE_UNKNOWN		0xffffffff	
	#define cdDEV_DATA_TYPE_UNDECIDED	0x00000000	
	#define cdDEV_DATA_TYPE_1			0x00000001	
	#define cdDEV_DATA_TYPE_2			0x00000002	

/* cdPortSelector 
    Port selection type used by CDGetPreviousDeviceInfo() */
typedef cdUInt16 cdPortSelector;  
	#define cdPORT_SELECT_DEFAULT		((cdPortSelector)1)
	#define cdPORT_SELECT_LAST_CHOICE	((cdPortSelector)3)

/* cdModelID 
   4 character camera model identifier  */
typedef  cdUInt32  cdModelID ;

/* cdVolHWType 
   The hardware type of a file system volume*/
typedef cdUInt16  cdVolHWType;  
	#define cdHW_TYPE_RAM    			0x0001
 	#define cdHW_TYPE_DISK   			0x0002
 	#define cdHW_TYPE_CDROM  			0x0003
 	#define cdHW_TYPE_REMOTE 			0x0004
	#define cdHW_TYPE_ROM				0x0005
	#define cdHW_TYPE_INVALID			0xFFFF

/* cdImageFormat 
    Image data format  */
typedef cdUInt16  cdImageFormat;  
	#define cdFORMAT_UNKNOWN			0x0000
	#define cdFORMAT_JPEG				0x0001
	#define cdFORMAT_JPEG_DCF			0x0002
	#define cdFORMAT_JPEG_DCF_THUMB		0x0003
	#define cdFORMAT_TIFF				0x0004
	#define cdFORMAT_TIFF_16			0x0005  
	#define cdFORMAT_TIFF_DCF			0x0006
	#define cdFORMAT_PIXMAP				0x0007
 	#define cdFORMAT_PIXMAP_16			0x0008
	#define cdFORMAT_OPENDML			0x0009 
	#define cdFORMAT_WAV				0x000a
	#define cdFORMAT_DIB				0x000b
	#define cdFORMAT_BMP_TOPDOWN		0x000c
	#define cdFORMAT_RAW				0x0101
	#define cdFORMAT_RAW_SPATIAL		0x0102
	#define cdFORMAT_RAW_CR2			0x000f

/* cdCompQuality
    Image data quality. */
typedef cdUInt16  cdCompQuality;  
	#define cdCOMP_QUALITY_UNKNOWN		0xffff
	#define cdCOMP_QUALITY_ECONOMY		0x0001
	#define cdCOMP_QUALITY_NORMAL		0x0002
	#define cdCOMP_QUALITY_FINE			0x0003
	#define cdCOMP_QUALITY_LOSSLESS		0x0004
	#define cdCOMP_QUALITY_RAW			0x0004
	#define cdCOMP_QUALITY_SUPERFINE	0x0005

#define MOVIE_MASK	0x000f
#define MOVIE_COMP_QUALITY(q)		(q&MOVIE_MASK)

#define SUB_JPEG_MASK	0xff00
#define SUB_JPEG_COMP_QUALITY(q)	((q&SUB_JPEG_MASK)>>8)

/* cdImageSize 
    Image size */
typedef cdUInt16 cdImageSize ;  
	#define cdIMAGE_SIZE_UNKNOWN		0xFFFF
	#define cdIMAGE_SIZE_LARGE			0x0000
	#define cdIMAGE_SIZE_MEDIUM			0x0001
	#define cdIMAGE_SIZE_SMALL			0x0002
	#define cdIMAGE_SIZE_MEDIUM1		0x0005
	#define cdIMAGE_SIZE_MEDIUM2		0x0006
	#define cdIMAGE_SIZE_MEDIUM3		0x0007

#define SUB_JPEG_SIZE(s)			((s&SUB_JPEG_MASK)>>8)
#define MOVIE_SIZE(s)				(s&MOVIE_MASK)

/* cdAttribute 
    Attributes for data. */
typedef cdUInt32 cdAttribute ;  
	#define cdATTRIB_READ				0x00000001
	#define cdATTRIB_WRITE				0x00000002
	#define cdATTRIB_READ_WRITE			(cdATTRIB_READ|cdATTRIB_WRITE)

	#define cdATTRIB_HIDDEN				0x00000010
	#define cdATTRIB_ARCHIVE			0x00000020
	#define cdATTRIB_SYSTEM				0x00000040

/* cdItemType 
	Type of the Item. */
typedef cdUInt32 cdItemType ;  
	#define cdITEM_TYPE_SUB_FOLDER		0x00000001
	#define cdITEM_TYPE_IMAGE_ITEM		0x00000002
	#define cdITEM_TYPE_FILE			0x00000003

/* cdDataType 
	Type of the image data. */
typedef cdUInt32 cdDataType;  
	#define cdDATA_TYPE_THUMBNAIL		0x00000010 /* Thumbnail image data */
	#define cdDATA_TYPE_PICTURE			0x00000020 /* Picture (Full resolution image) data */
	#define cdDATA_TYPE_SOUND			0x00000030 /* Sound data */
	#define cdDATA_TYPE_MOVIE			0x00000040 /* Movie data */
	#define cdDATA_TYPE_PLUS_JPEG		0x00000050 /* +Jpg image data */
	#define cdDATA_TYPE_PREVIEW_JPEG	0x00000060 /* Jpeg data included in RAW file */

/* cdAddPictureFlags
*/
typedef cdUInt32 cdAddPictureFlags;
	#define cdADD_FLAG_COPY				0x00010000


/* cdEnumItemOption
	Options for enumeration of items. */
typedef cdUInt32 cdEnumItemOption;  
	#define cdENUM_HAS_THUMBNAIL		0x00000001	/* Not enumerate items which don't have thumbnail image. */
	#define cdENUM_MOVIE_TOO			0x00000002	/* Enumerate items which have movies, too. */
	#define cdENUM_MOVIE_ONLY			0x00000004	/* Enumerate movie items only. */
	#define cdENUM_NAME_CHECK_OP_1		0x00000008	/* Enumerate same file name files as one item. */
	#define cdENUM_ALL_ITEM				0x00010000	/* Enumerate the item which is not image item.*/

/* cdProgressOption
	Options for progress callbadk functions. */
typedef cdUInt32 cdProgressOption;  
	#define cdPROG_NO_REPORT			0x00000001	/* Not called callback function. */
	#define cdPROG_REPORT_DONE			0x00000002	/* Called only when operation is finished */
	#define cdPROG_REPORT_PERIODICALLY	0x00000003	/* Called periodically during the operation */

/* cdProgressStatus
	Status of the current operation during progress callback function.*/
typedef cdUInt32 cdProgressStatus;  
	#define cdSTATUS_TRANSFER			0x00000001	/* Status is data transfer between camera and host computer. */
	#define cdSTATUS_CONVERT			0x00000002	/* Status is image data converting.	 */
	#define cdSTATUS_DEVELOPMENT		0x00000003	/* Status is image data development. */

/* cdSlideShowCap
	Capability of slide show function of camera.*/
typedef cdUInt32 cdSlideShowCap;
	#define cdSLIDE_SHOW_CAP_CANON_FORMAT 	0x00000001  /* Supports the slide show file of Canon format. */ 
	#define cdSLIDE_SHOW_CAP_DPOF			0x00000002  /* Supports the DPOF slide show file. */

/* cdReleaseControlCap
	Capability of remote release control function of the camera.
	OR value of the followings.                                */
typedef cdUInt32 cdReleaseControlCap;
	#define cdRELEASE_CONTROL_CAP_SUPPORT 		0x40000000  /* Supports the remote release control functions. */
	#define cdRELEASE_CONTROL_CAP_ZOOM			0x00000001	/* Supports the zoom control. */
	#define cdRELEASE_CONTROL_CAP_SETPRM		0x00000002	/* Supports the setting of the shooting parameters. */
	#define cdRELEASE_CONTROL_CAP_VIEWFINDER	0x00000004  /* Supports the viewfinder functions. */
	#define cdRELEASE_CONTROL_CAP_ABORT_VIEWFINDER	0x00000008	/* Needs to terminate the Viewfinder mode when the CDRelease() function is executed. */
	#define cdRELEASE_CONTROL_CAP_AF_LOCK		0x00000010  /* Supports the AF lock/unlock functions. */	
	#define cdRELEASE_CONTROL_CAP_RCFVER_1_5	0x00000020	/* Remote Release Control Function Version 1.5 */

	typedef cdReleaseControlCap	cdReleaseControlFaculty;

/*cdParseFaculty
*/
typedef cdUInt32 cdParseFaculty;
	#define cdPARSE_SET_PROP_IN_CAMERA 			0x00000001   /* Supports the modifing the property of the item in camera.*/
	

/* Values of cdIMG_PROP_MLWEI_MODE image property
*/
	#define cdMLWEI_MODE_CENTER    				(0)
	#define cdMLWEI_MODE_SPOT   				(1)
	#define cdMLWEI_MODE_AVERAGE   				(2)
	#define cdMLWEI_MODE_ESTIMAT   				(3)
	#define cdMLWEI_MODE_PART    				(4)
	#define cdMLWEI_MODE_CENTER_WEI_AVERAGE 	(5) 
	#define cdMLWEI_MODE_UNKNOWN   				(0xffff)

/* cdFocusMode 
	AF mode setting */
typedef cdUInt16 cdFocusMode; 
	#define cdFOCUS_MODE_SINGLE_AF  			(0x0000) /* Single AF     */
	#define cdFOCUS_MODE_CONTINUOUS_AF			(0x0001) /* Continuous AF */
	#define cdFOCUS_MODE_PAN_FOCUS  			(0x0010) /* Pan Focus     */
	#define cdFOCUS_MODE_ONE_SHOT_AF 			(0x1000) /* One-Shot AF	  */
	#define cdFOCUS_MODE_AI_SERVO_AF 			(0x1001) /* AI Servo AF   */
	#define cdFOCUS_MODE_AI_FOCUS_AF 			(0x1002) /* AI Focus AF   */
	#define cdFOCUS_MODE_MF   					(0x0008) /* Manual Focus  */
	#define cdFOCUS_MODE_UNKNOWN  				(0xffff) /* unknown       */

/* cdCompressionType
    picture view compression type (including NONE!)*/
typedef cdUInt8 cdCompressionType; 
	#define cdLOSSY    1
	#define cdNONE     2
	#define cdLOSSLESS 3

/* cdDriveMode
*/
typedef cdUInt16 cdDriveMode;  
	#define cdDRIVE_MODE_UNKNOWN				0xFFFF
	#define cdDRIVE_MODE_SINGLE					0x0000
	#define cdDRIVE_MODE_CONT					0x0001
	#define cdDRIVE_MODE_MOVIE					0x0002
	#define cdDRIVE_MODE_CONT_SPEED_PRIORITY	0x0003
	#define cdDRIVE_MODE_CONT_LOW				0x0004
	#define cdDRIVE_MODE_CONT_HIGHT				0x0005
	#define cdDRIVE_MODE_SELFTIMER				0x0010

/* cdShootingMode
*/
typedef cdUInt16 cdShootingMode;  
	#define cdSHOOTING_MODE_INVALID			0xFFFF
	#define cdSHOOTING_MODE_AUTO			0x0000
	#define cdSHOOTING_MODE_PROGRAM			0x0001
	#define cdSHOOTING_MODE_TV				0x0002
	#define cdSHOOTING_MODE_AV				0x0003
	#define cdSHOOTING_MODE_MANUAL			0x0004
	#define cdSHOOTING_MODE_A_DEP			0x0005
	#define cdSHOOTING_MODE_M_DEP			0x0006
	#define cdSHOOTING_MODE_BULB			0x0007
	#define cdSHOOTING_MODE_MANUAL_2		0x0065
	#define cdSHOOTING_MODE_FAR_SCENE		0x0066
	#define cdSHOOTING_MODE_FAST_SHUTTER	0x0067
	#define cdSHOOTING_MODE_SLOW_SHUTTER	0x0068
	#define cdSHOOTING_MODE_NIGHT_SCENE		0x0069
	#define cdSHOOTING_MODE_GRAY_SCALE		0x006a
	#define cdSHOOTING_MODE_SEPIA			0x006b
	#define cdSHOOTING_MODE_PORTRAIT		0x006c
	#define cdSHOOTING_MODE_SPOT			0x006d
	#define cdSHOOTING_MODE_MACRO			0x006e
	#define cdSHOOTING_MODE_BW				0x006f
	#define cdSHOOTING_MODE_PANFOCUS		0x0070
	#define cdSHOOTING_MODE_VIVID			0x0071
	#define cdSHOOTING_MODE_NEUTRAL			0x0072
	#define cdSHOOTING_MODE_FLASH_OFF		0x0073
	#define cdSHOOTING_MODE_LONG_SHUTTER	0x0074
	#define cdSHOOTING_MODE_SUPER_MACRO		0x0075
	#define cdSHOOTING_MODE_FOLIAGE			0x0076
	#define cdSHOOTING_MODE_INDOOR			0x0077
	#define cdSHOOTING_MODE_FIREWORKS		0x0078
	#define cdSHOOTING_MODE_BEACH			0x0079
	#define cdSHOOTING_MODE_UNDERWATER		0x007a
	#define cdSHOOTING_MODE_SNOW			0x007b
	#define cdSHOOTING_MODE_KIDS_AND_PETS	0x007c
	#define cdSHOOTING_MODE_NIGHT_SNAPSHOT	0x007d
	#define cdSHOOTING_MODE_DIGITAL_MACRO	0x007e
	#define cdSHOOTING_MODE_MYCOLORS		0x007f
	#define cdSHOOTING_MODE_PHOTO_IN_MOVIE	0x0080


/* cdAFDistance
	Auto focus mode by which distance is measured */
typedef cdUInt16 cdAFDistance;  
	#define cdAF_DISTANCE_MANUAL			0x0000	
	#define cdAF_DISTANCE_AUTO				0x0001
	#define cdAF_DISTANCE_UNKNOWN			0x0002
	#define cdAF_DISTANCE_CLOSE_UP			0x0003	
	#define cdAF_DISTANCE_VERY_CLOSE		0x0004
	#define cdAF_DISTANCE_CLOSE				0x0005
	#define cdAF_DISTANCE_MIDDLE			0x0006	
	#define cdAF_DISTANCE_FAR				0x0007
	#define cdAF_DISTANCE_PAN_FOCUS			0x0008
	#define cdAF_DISTANCE_SUPER_MACRO		0x0009
	#define cdAF_DISTANCE_INFINITY			0x000a
	#define cdAF_DISTANCE_SUPER_MACRO_0CM	0x000b
	#define cdAF_DISTANCE_NA				0x00ff

/* cdPhotoEffect
	Photo effect setting */
typedef cdUInt16 cdPhotoEffect;  
	#define cdPHOTO_EFFECT_OFF				0x0000	/* Off				*/
	#define cdPHOTO_EFFECT_VIVID			0x0001	/* Vivid			*/
	#define cdPHOTO_EFFECT_NEUTRAL			0x0002	/* Neutral			*/
	#define cdPHOTO_EFFECT_LOW_SHARPENING	0x0003	/* Low sharpening	*/
	#define cdPHOTO_EFFECT_SEPIA			0x0004	/* Sepia			*/
	#define cdPHOTO_EFFECT_BW				0x0005	/* Black & white	*/
	#define cdPHOTO_EFFECT_CUSTOM			0x0006	/* Custom			*/
	#define cdPHOTO_EFFECT_MY_COLOR			0x0064	/* My color data    */
	#define cdPHOTO_EFFECT_UNKNOWN			0xffff	/* Unknown			*/

/* cdColorMatrix
	Color matrix number */
typedef cdUInt16 cdColorMatrix;  
	#define cdCOLOR_MATRIX_ATCAPTURE		0x0000	/* As shot value. Use for cdSetDevParam_ColorMatrix() only */
	#define cdCOLOR_MATRIX_1				0x0001	/* Color matrix 1	*/
	#define cdCOLOR_MATRIX_2				0x0002	/* Color matrix 2	*/
	#define cdCOLOR_MATRIX_3				0x0003	/* Color matrix 3	*/
	#define cdCOLOR_MATRIX_4				0x0004	/* Color matrix 4	*/
	#define cdCOLOR_MATRIX_5				0x0005	/* Color matrix 5	*/
	#define cdCOLOR_MATRIX_UNKNOWN			0xffff	/* Unknown			*/


/*-----------------------------------------------------------------------
   Constants for Image property  
------------------------------------------------------------------------*/
/* cdImagePropertyID 
*/
typedef cdUInt32 cdImagePropertyID;  
	#define cdIMG_PROP_INVALID						0xFFFFFFFF
	/* Invalid ID
	*/

  /*----------------------------------------------------------                  
	  Common Property 
  */
	#define cdIMG_PROP_SERIAL_NUMBER				0x00000100
	/* The number used to generate the folder and file names.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_MODEL_NAME					0x00000200
	/* Camera model name.
	       Attribute : Read only
	       Size/Type : 32 / cdStr31
	*/
	#define cdIMG_PROP_OWNER_NAME					0x08000300
	/* Camera owner's name
	       Attribute : Read/Write
	       Size/Type : 32 / cdStr31
	*/
	#define cdIMG_PROP_CAPTURE_TIME					0x00000400
	/* When the picture was taken
	       Attribute : Read only
	       Size/Type : 4 / cdTime
	*/
	#define cdIMG_PROP_CONFIG_NAME					0x00000500
	/* How the camera was configured (example: "IMG:PowerShot JPEG")
	       Attribute : Read only
	       Size/Type : 32 / cdStr31
	*/
	#define cdIMG_PROP_AV							0x00000600
	/* Aperture value. The unit is APEX (Additive system of Photographic Exposure).
	       Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_TV							0x00000700
	/* Shutter speed. "Tv" stands for "time value".
	       Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_EXPOSURE_COMP				0x00000800
	/* Exposure compensation value. This value will range from -3 to +3. 
	       Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_EF_GUIDE_NUMBER				0x00000900
	/* Flash guide number. 0 means flash not used.
	       Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_SELF_TIMER_TIME				0x00000a00
	/* Self-timer elapsed time in milliseconds.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/ 
	#define cdIMG_PROP_FOCUSING_SETTING				0x00000b00
	/* Focusing setting: 0 means normal range scan, 1 means macro range scan.
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_COMMENT						0x08000c00
	/* User comment string termination by NULL.
	       Attribute : Read/Write
	       Size/Type : any / cdChar
	*/
	#define cdIMG_PROP_COMMENT_CODE					0x08000d00
	/* User comment character code. Same as Exif specification.
	       Attribute : Read/Write
	       Size/Type : 8 / cdChar[8]
	*/
	#define cdIMG_PROP_COMMENT_BUF_SIZE				0x00000e00
	/* Buffer size of the user comment in image file.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_RELEASE_METHOD				0x00000f00
	/* Release method. (Drive mode)
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Value:	
	    0: normal
	    1: continuous shooting
	    2: movie
	    3: continuous shooting(Speed priority)
	    4: continuous low
	    5: continuous high
	*/
	#define cdIMG_PROP_CONT_NUMBER					0x00001000
	/* 0-based frame number on continous shooting.
	   Some camera does not set this value.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_STITCH_DESC					0x00001100
	/* Stitch assist description.
	       Attribute : Read only
	       Size/Type : 12 / cdStitchDesc
	*/
	#define cdIMG_PROP_LENS_TYPE					0x00001200
	/* Lens type.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    1: Non zoom lens
		2: Zoom lens
	*/
	#define cdIMG_PROP_FOCUL_LENGTH					0x00001300
	/* The focal length of the lens (mm)*32
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_SENSOR_SIZE_X				0x00001400
	/* Sensor size (horizontal: inches size * 1024)
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_SENSOR_SIZE_Y				0x00001500
	/* Sensor size (vertical: inches size * 1024)
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WHITE_BALANCE				0x00001600
	/* White balance setting.
	       Attribute : Read only
	       Size/Type : 2 / cdWBLightSrc
	*/
	#define cdIMG_PROP_SLOW_SHUTTER					0x00001700
	/* Slow shutter setting
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: off
	    1: night scene
	    2: on
	    3: not existing
	*/
	#define cdIMG_PROP_ROTATION_ANGLE				0x08001800
	/* Rotation angle. (0, 90, 180, 270)
	       Attribute : Read/Write
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_FOCAL_LENGTH_DCF				0x00001a00
	/* The actual focal length of the lens, in mm.
	       Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_FOCAL_PLANE_RES_X			0x00001b00	
	/* The number of pixels in the image width(X) direction 
	   per unit derived by cdIMG_PROP_FOCAL_PLANE_RES_UNIT 
	   on the camera focal plane.
	       Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_FOCAL_PLANE_RES_Y			0x00001c00	
	/* The number of pixels in the image height(Y) direction 
	   per unit derived by cdIMG_PROP_FOCAL_PLANE_RES_UNIT 
	   on the camera focal plane.
	       Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_FOCAL_PLANE_RES_UNIT			0x00001d00	
	/* The unit for measuring the value derived by 
	   cdIMG_PROP_FOCAL_PLANE_RES_X and cdIMG_PROP_FOCAL_PLANE_RES_Y.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Value:
	    2    : inches
		3    : centimeters
		Other: reserved
	*/
	#define cdIMG_PROP_COMP_QUALITY					0x00001e00
	/* Image quality setting.
	       Attribute : Read only
	       Size/Type : 2 / cdCompQuality
	*/
	#define cdIMG_PROP_IMAGE_SIZE					0x00001f00
	/* Image size setting.
	       Attribute : Read only
	       Size/Type : 2 / cdImageSize
	*/
	#define cdIMG_PROP_SHOOTING_MODE				0x00002000
	/* Shooting mode.
	       Attribute : Read only
	       Size/Type : 2 / cdShootingMode
	*/
	#define cdIMG_PROP_EZOOM						0x00002100
	/* Digital zoom mode.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Valies:
	    0: Not digital zoomed.
		1: x2
		2: x4
		3: Smooth (continuous) digital zoom.
		0xffff: Unknown
	*/
	#define cdIMG_PROP_CONTRAST						0x00002200
	/* Contrast. 
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	   Values:
	    -2:low2 -1:low1 0:default 1:high1 2:high2 0x7fff:unknown
	*/
	#define cdIMG_PROP_COLOR_GAIN					0x00002300
	/* Color gain.
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	   Valuds:
	    -2:low2 -1:low1 0:default 1:high1 2:high2 0x7fff:unknown
	*/
	#define cdIMG_PROP_SHARPNESS					0x00002400
	/* Sharpness.
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	   Valuds:
	    -2:low2 -1:low1 0:default 1:high1 2:high2 0x7fff:unknown
	*/
	#define cdIMG_PROP_SENSITIVITY					0x00002500
	/* Sensitivity.
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	   Values:
	    0:default 1:up_1 2:up_2 0xf:Auto 0x10:ISO50 0x11:ISO100 0x12:ISO200 
		0x13:ISO400 0x14:ISO800 0x7fff:unknown
		If 0x4000 bit is set, lower 10 bits shows ISO value.
		 e.g. 0x4040 means ISO64.
	*/
	#define cdIMG_PROP_MLWEI_MODE					0x00002600
	/* Measure light weight mode.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    cdMLWEI_MODE_CENTER (0)            : Center-weighted averaging
		cdMLWEI_MODE_SPOT   (1)            : Spot
		cdMLWEI_MODE_AVERAGE(2)            : Averaging
		cdMLWEI_MODE_ESTIMAT(3)            : Evaluative
		cdMLWEI_MODE_PART   (4)            : Partial
		cdMLWEI_MODE_CENTER_WEI_AVERAGE(5) : Center-weighted averaging
		cdMLWEI_MODE_UNKNOWN(0xffff)       : Unknown
	*/
	#define cdIMG_PROP_ISO_SPEED_RATINGS			0x00002700
	/* ISO Speed.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_CUSTOM_FUNC1					0x00002900
	/* Custom function settings. 
	       Attribute : Read only
	       Size/Type : any (depend on camera model) / cdCustomFunction
	*/
	#define cdIMG_PROP_CUSTOM_FUNC2					0x00002b00
	/* Custom function settings. 
	       Attribute : Read only
	       Size/Type : sizeof(cdCustomFunction2) / cdCustomFinction2
	*/
	#define cdIMG_PROP_PERSONAL_FUNC				0x00002d00
	/* Personal fnction settngs. 
	       Attribute : Read only
	       Size/Type : sizeof(cdPersonalFunction) / cdPersonalFunction
	*/
	#define cdIMG_PROP_PERSONAL_FUNC_VAL			0x00002f00
	/* Personal fnction settng values.
	       Attribute : Read only
	       Size/Type : sizeof(cdPersonalFunctionValue) / cdPersonalFunctionValue
	*/
	#define cdIMG_PROP_BODY_ID						0x00003000
	/* Camera body ID number.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_EF							0x00003100
	/* EF.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0     : off
	    1     : auto
	    2     : on
	    3     : red eye reduction
	    4     : slow synchro
	    5     : auto + red eye reduction
	    6     : on + red eye reduction
	    0x10  : External Strobe
	    0xffff: Unknown
	*/
	#define cdIMG_PROP_FOCUS_POSITION				0x00003200
	/* Focusing point.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Meaning:
	    Upper 4bits is 0: Means this value is invalid.
		Upper 4bits is 1: Means center focus point selection.
		Upper 4bits is 3: Means 3 points focus selection. Checks lower 12bits 
		                  further;   
					       0: No focus point.    
						   1. Right AF point is selected.   
						   2. Center AF point is selected.   
						   4. Left AF point is selected.
	*/
	#define cdIMG_PROP_FOCUS_POSITION2				0x00003300
	/* Focusing point.
	       Attribute : Read only
	       Size/Type : 8 / cdUInt32
	   Meaning:
	    First 1 byte shows AF point.
	      Upper 3 bits represent Y coordinates in the Table 1 shown below.
		  Lower 5 bits represent X coordinates in the Table 1 shown below.


        Next 7 bytes show focus points.
          The Table 2 shows the meaning of each bit included in these 7 bytes.
		  The number which has been described to table 2 corresponds to the 
		  number of the focus point of table 1. 
		  If the bit is set, it means that the point correspond to the bit 
		  was focused.


	                     Table 1: AF Point and Focus Point Map	
              X 
         |  0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20  |
       --|------------------------------------------------------------------|
       0 |             [02]  [03]  [04]  [05]  [06]  [07]  [10]             |
     Y   |                                                                  |
	   1  |    [13]  [14]  [15]  [16]  [17]  [20]  [21]  [22]  [23]  [24]    |   
         |                                                                  |
	   2  | [25]  [26]  [27]  [30]  [31]  [32]  [33]  [34]  [35]  [36]  [37] |
         |                                                                  |
	   3  |    [40]  [41]  [42]  [43]  [44]  [45]  [46]  [47]  [50]  [51]    |  
         |                                                                  |
	   4  |             [54]  [55]  [56]  [57]  [60]  [61]  [62]             |
       --|------------------------------------------------------------------|


	                     Table 2: AF Point (Last 7 bytes)				  

	           | Bit7 | Bit6 | Bit5 | Bit4 | Bit3 | Bit2 | Bit1 | Bit0 |
	     ------|------|------|------|------|------|------|------|------|
	     BYTE0 | [07] | [06] | [05] | [04] | [03] | [02] |  --  |  --  |
	     BYTE1 | [17] | [16] | [15] | [14] | [13] |  --  |  --  | [10] |
	     BYTE2 | [27] | [26] | [25] | [24] | [23] | [22] | [21] | [20] |
	     BYTE3 | [37] | [36] | [35] | [34] | [33] | [32] | [31] | [30] |
	     BYTE4 | [47] | [46] | [45] | [44] | [43] | [42] | [41] | [40] |
	     BYTE5 | [57] | [56] | [55] | [54] |  --  |  --  | [51] | [50] |
	     BYTE6 |  --  |  --  |  --  |  --  |  --  | [62] | [61] | [69] |
	*/
	#define cdIMG_PROP_BULB_TIME					0x00003400
	/* Bulb time in Bulb release (100msec).
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_FOCUS_MODE					0x00003500
	/* Focus mode.
	       Attribute : Read only
	       Size/Type : 2 / cdFocusMode
	*/
	#define cdIMG_PROP_FOCAL_LENGTH_TELE			0x00003600
	/* Focal length of tele end (mm).
	       Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_FOCAL_LENGTH_WIDE			0x00003700
	/* Focal length of wide end (mm).
	       Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_FLASH_MODE					0x00003800
	/* Flash mode.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    (bit assigned)
	     bit0 : Manual
	     bit1 : TTL
	     bit2 : A-TTL
	     bit3 : E-TTL
	     bit4 : Flat
	     bit7 : 2nd-curtain sync.
	     bit13: Built-in Flash
	     bit14: External Flash	    
	*/
	#define cdIMG_PROP_AEB_SHOT_NUMBER				0x00003900
	/* Shootign number in AEB.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0:no-AEB  1:1st-AEB  2:2nd-AEB  3:3rd-AEB
	*/
	#define cdIMG_PROP_AEB_EXPOSURE_COMP			0x00003a00
	/* Exposure compensation value in AEB (APPEX).
	       Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_FLASH_COMP					0x00003b00
	/* Flash compensation value (APPEX).
	       Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_AIAE							0x00003d00	
	/* AE setting in shooting.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: Nomal AE
		1: Exposure compensation.
		2: AE lock.
		3: Exposure compensation and AE lock.
		4: No AE.
		0xffff: Unknown.
	*/
	#define cdIMG_PROP_IS_SETTING					0x00003e00	
	/* Image stabilizer setting of the PowerShot Pro90IS. 
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: off  1: on   0xffff: unknown
	*/
	#define cdIMG_PROP_DISP_AV_OPEN					0x00003f00	
	/* Aperture value for display (/10)
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_EZOOM_SRC_SIZE				0x00004000
	/* The number of horizontal direction logging pixels when smooth 
	   electronic zoom.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_EZOOM_DST_SIZE				0x00004100
	/* The number of horizontal direction enlargement target pixels
	   when smooth electronic zoom.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Note:
	    Electronic zoom magnification in smooth electronic zoom
           = EZoomDstSize / EZoomSrcSize
	*/
	#define cdIMG_PROP_AF_DISTANCE					0x00004200
	/* AF distance setting.
	       Attribute : Read only
	       Size/Type : 2 / cdAFDistance
	*/
	#define cdIMG_PROP_AV_OPEN						0x00004300
	/* Minimum aperture value.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_AV_MAX						0x00004400
	/* Maximum aperture value.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_SHOOTING_NUM_H				0x00004500
	/* Upper-word of shooting number.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_SHOOTING_NUM_L				0x00004600
	/* Lower-word of shooting number.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_BRACKET_MODE					0x00004700
	/* Bracket mode
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0:non-bracket 
		1:AEB  2:FEB  3:ISO  4:WB
	*/
	#define cdIMG_PROP_BRACKET_VALUE				0x00004800
	/* Bracket comp value. (APPEX value)
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_BRACKET_SHOT_NUMBER			0x00004900
	/* Bracked shooting number.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_RAW_MODE_QUALITY				0x00004a00
	/* JPEG quality in RAW+JPEG rec.
	       Attribute : Read only
	       Size/Type : 2 / cdCompQuality
	*/
	#define cdIMG_PROP_RAW_MODE_SIZE				0x00004b00
	/* JPEG size in RAW+JPEG rec.
	       Attribute : Read only
	       Size/Type : 2 / cdImageSize
	*/
	#define cdIMG_PROP_NOISE_REDUCTION				0x00004c00
	/* Noise reduction mode.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: off   1: type1   2: type2
	*/
	#define cdIMG_PROP_GAMMA						0x00004d00
	/* Tone curve 
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: standard  1: manual set  2: custom set
	*/
	#define cdIMG_PROP_EDGE_LEVEL					0x00004e00
	/* Sharpness level.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: off  1: 1  2: 2  3: 3  4: 4  5: 5
	*/
	#define cdIMG_PROP_EDGE_FREQ					0x00004f00
	/* Sharpness object
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: invalid  1: very coarse  2: coarse  3: standard  4: fine  
		5: very fine
	*/
	#define cdIMG_PROP_SENSOR_DATA_RG				0x00005000
	/* WB sensor data.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_SENSOR_DATA_BG				0x00005100
	/* WB sensor data.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WHITE_DATA_R					0x00005200
	/* Custom WB data
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WHITE_DATA_B					0x00005300
	/* Custom WB data
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WB_COLOR_TEMP				0x00005400
	/* WB color temperature.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_COLOR_MATRIX					0x00005500
	/* Color matrix
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    1: 1  2: 2  3: 3  4: 4  5: 5  6: 6  7: 7  0xffff: Unknown
	*/
	#define cdIMG_PROP_DIGITAL_GAIN					0x00005600
	/* Digital exposure compensation value (x10)
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	*/
	#define cdIMG_PROP_GAMMA_TABLE					0x00005700
	/* Gamma table.
	       Attribute : Read only
	       Size/Type : sizeof(cdDevGammaLUTTable) / cdDevGammaLUTTable
	*/
	#define cdIMG_PROP_EDGE_LEVEL_TABLE				0x00005800
	/* Edge level table.
	       Attribute : Read only
	       Size/Type : sizeof(cdDevEdgeLevelTable) / cdDevEdgeLevelTable
	*/
	#define cdIMG_PROP_EDGE_FREQ_TABLE				0x00005900
	/* Edge frequency table.
	       Attribute : Read only
	       Size/Type : sizeof(cdDevEdgeFreqTable) / cdDevEdgeFreqTable
	*/
	#define cdIMG_PROP_WB_TABLE						0x00005a00
	/* WB table.
	       Attribute : Read only
	       Size/Type : sizeof(cdDevWBParamTable) / cdDevWBParamTable
	*/
	#define cdIMG_PROP_MODIFIED_PARAM_FLAG			0x00005b00
	/* Modified flag.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt32	
	   Values:
	    0:DevelopParam is not modified  1: Modified
	*/
	#define cdIMG_PROP_GAMMA_MATCHING				0x00005d00	
	/* Gamma Matching table.
	       Attribute : Read only
	       Size/Type : sizeof(cdDevGammaLUTMatching) / cdDevGammaLUTMatching
	*/
	#define cdIMG_PROP_WB_MATCHING					0x00005e00	
	/* WB Matching 
	       Attribute : Read only
	       Size/Type : sizeof(cdDevWBParamMatching) / cdDevWBParamMatching
	*/
	#define cdIMG_PROP_PHOTO_EFFECT					0x00005f00
	/* Photo effect setting.
	       Attribute : Read only
	       Size/Type : sizeof(cdPhotoEffect) / cdPhotoEffect
	*/
	#define cdIMG_PROP_PHOTO_HUE					0x00006000
	/* Hue / Color tone.
	       Attribute : Read only
	       Size/Type : sizeof(cdInt16) / cdInt16
	   Values:
	    -2: -2   -1: -1   0: Normal  1: 2   2: 2   0x7fff: Unknown
	*/
	#define cdIMG_PROP_EXPOSURETIME_DCF				0x00006100
	/* Exposure Time (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdURational) / cdURational
	*/
	#define cdIMG_PROP_FNUMBER_DCF					0x00006200
	/* F number (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdURational) / cdURational
	*/
	#define cdIMG_PROP_SHUTTERSPEED_DCF				0x00006300
	/* Shutter speed (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdRational) / cdRational
	*/
	#define cdIMG_PROP_APERTUREVALUE_DCF			0x00006400
	/* Aperture value(Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdURational) / cdURational
	*/
	#define cdIMG_PROP_EXPOSUREBIASVALUE_DCF		0x00006500
	/* Exposure bias value (Exif)
	       Attribute : Read only
		   Size/Type : sizeof(cdRational) / cdRational
	*/
	#define cdIMG_PROP_FLASH_DCF					0x00006600
	/* Flash (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdUInt16) / cdUInt16
	*/
	#define cdIMG_PROP_EXIFVERSION					0x00006700
	/* Exif version
	       Attribute : Read only
	       Size/Type : 4 / cdChar[4]
	   Note:
	    NOT NULL termnated.
	*/
	#define cdIMG_PROP_PROCESSCOMPLETED_DCF			0x00006800
	/* Process completed (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdUInt16) / cdUInt16
	*/
	#define cdIMG_PROP_EXPOSUREMODE_DCF				0x00006900
	/* Exposure mode (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdUInt16) / cdUInt16
	*/
	#define cdIMG_PROP_AUTOWHITEBALANCE_DCF			0x00006a00
	/* Auto white balance (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdUInt16) / cdUInt16
	*/
	#define cdIMG_PROP_DIGITALZOOMRATIO_DCF			0x00006b00
	/* Digital zoom ratio.
	       Attribute : Read only
	       Size/Type : sizeof(cdURational) / cdURational
	*/
	#define cdIMG_PROP_SCENECAPTURETYPE_DCF			0x00006c00
	/* Scene capture type (Exif)
	       Attribute : Read only
	       Size/Type : sizeof(cdUInt16) / cdUInt16
	*/

	#define cdIMG_PROP_MODIFIED_GAMMA				0x00006d00	
	/* Modified tone curve 
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: standard  1: manual set  2: custom set
	*/
	#define cdIMG_PROP_MODIFIED_EDGE_LEVEL			0x00006e00
	/* Modified sharpness level.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: off  1: 1  2: 2  3: 3  4: 4  5: 5
	*/
	#define cdIMG_PROP_MODIFIED_EDGE_FREQ			0x00006f00
	/* Modified sharpness object
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: invalid  1: very coarse  2: coarse  3: standard  4: fine  
		5: very fine
	*/
	#define cdIMG_PROP_MODIFIED_SENSOR_DATA_RG		0x00007000
	/* Modified WB sensor data.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MODIFIED_SENSOR_DATA_BG		0x00007100
	/* Modified WB sensor data.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MODIFIED_WHITE_DATA_R		0x00007200
	/* Modified custom WB data
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MODIFIED_WHITE_DATA_B		0x00007300
	/* Modified custom WB data
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MODIFIED_WB_COLOR_TEMP		0x00007400
	/* Modified WB color temperature.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MODIFIED_COLOR_MATRIX		0x00007500
	/* Modified color matrix
	       Attribute : Read only
	       Size/Type : 2 / cdColorMatrix
	*/
	#define cdIMG_PROP_MODIFIED_DIGITAL_GAIN		0x00007600
	/* Modified digital exposure compensation value (x10)
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	*/
	#define cdIMG_PROP_MODIFIED_WHITE_BALANCE		0x00007700
	/* Modified white balance setting.
	       Attribute : Read only
	       Size/Type : 2 / cdWBLightSrc
	*/
	#define cdIMG_PROP_COLORSPACE_DCF				0x00007800
	/* Color space  (Exif)
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_BODY_ID_DESC					0x00007900
	/* Display format of the camera body ID number got by cdIMG_PROP_BODY_ID.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_FILE_TYPE_ID					0x00007a00
	/* File type ID.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_MAKER						0x00008000
	/* The manufacturer of the camera.
	       Attribute : Read only
	       Size/Type : any / cdChar
	   Note:
	    NOT NULL termnated.
	*/
	#define cdIMG_PROP_CUSTOM_FUNC1_EX				0x00008100
	/* Custom function settings. 
	       Attribute : Read only
	       Size/Type : any (depend on camera model) / cdUInt16
	*/
	#define cdIMG_PROP_CUSTOM_FUNC2_EX				0x00008200
	/* Custom function settings. 
	       Attribute : Read only
	       Size/Type : any (depend on camera model) / cdUInt16
	*/
	#define cdIMG_PROP_LOSSLESS_ROTATION_ANGLE		0x08008300
	/* Lossless rotation angle. (0, 90, 180, 270)
	       Attribute : Read/Write
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_INPUT_COLOR_TEMP				0x00008400
	/* WB color temperature.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_COLORSPACE_EX				0x00008500
	/* Color space
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    1: sRGB  2: Adobe RGB  0xffff: Unknown
	*/
	#define cdIMG_PROP_JPEG_SIZE_IN_CRW				0x00008600
	/* Size of the Jpeg data recorded in the CRAW file.
	       Attribute : Read only
	       Size/Type : 10 / cdUInt16
	   Values:
	    Bytes 0-1  Length of this property data.
		 Bytes 2-3  Quality setting.
		 Bytes 4-5  Data size.
		 Bytes 6-7  Width in pixels.
		 Bytes 8-9	Height in pixels.
	*/
	#define cdIMG_PROP_FOCUS_POSITION3				0x00008700
	/* Focus point and AF point information.
	       Attribute : Read only
	       Size/Type : any / any
	   Values:
		 (Order)   (Type)     (Name)
		    1     cdUInt16    Frames 
		    2     cdUInt16    FocusFrames
		    3     cdUInt16    ImageWidth
		    4     cdUint16    ImageHeight
		    5	    cdUInt16    FrameWDenominator
		    6     cdUInt16    FrameHDenominator
		    7     cdUInt16    FrameWNumerator
		    8     cdUInt16    FrameHNumerator
		    9     cdInt16[]   FrameXNumerator
		   10     cdInt16[]   FrameYNumerator
		   11     cdUInt16[]  JustFocusFramePattern
		   12     cdUInt16    JustFocusFrame

			
			Frames:
			     Represents the total number of elements contained in the array 
				  of the FrameXNumerator and FrameYNumerator.

			FocusFrames:
				  Represents the number of effective arrangement of the 
				  FrameXNumerator and FrameYNumerator array.

         ----------

         The width of the AF frame to the image (Wf) can be get by the next formula:
            Wf = ImageWidth * (FrameWNumerator/FrameWDenominator)


         The height of the AF frame to the image (Hf) can be get by the next formula:
            Wf = ImageHeight * (FrameHNumerator/FrameHDenominator)

         X coordinates of the focused frame to the image (fx) can be get by the next formula:
            fx = ImageWidth / FrameWDenominator * (FrameXNumerator + (ImageWidth - FrameWNumerator)/2)

         Y coordinates of the focused frame to the image (fy)  can be get by the next formula:
            fy = ImageHeight / FrameHDenominator * (FrameYNumerator + (ImageHeight - FrameHNumerator)/2)
	*/
	#define cdIMG_PROP_WHITEPOINT_DCF				0x00008800
	/* Chromaticities of reference whitepoint. 
	       Attribute : Read only
	       Size/Type : 16 / cdURational[2]
	   Values:
        Chromaticities of (X, Y)
	*/
	#define cdIMG_PROP_PRIMARYCHROMATICITIES_DCF	0x00008900
	/* Chromaticities of the three primary colors. 
	       Attribute : Read only
	       Size/Type : 48 / cdURational[6]
	   Values:
	 	 R chromaticities: (X, Y)
		 G chromaticities: (X, Y)
		 B chromaticities: (X, Y)
	*/
	#define cdIMG_PROP_YCBCRCOEFFICIENTS_DCF		0x00008a00
	/* Color transform matrix coefficients for going from RGB to YCbCr image data.
	       Attribute : Read only
	       Size/Type : 24 / cdURational[3]
	*/
	#define cdIMG_PROP_GAMMA_DCF					0x00008b00
	/* Gamma compensation coefficient used by the transformation function in playback.
	       Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_INTER_OPERABILITY_FOOTER		0x00008c00
	/* Value for internal use.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_SUPER_MACRO					0x00008d00
	/* Super macro setting.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: non super macro   1: super macro   0xffff: unknown  
	*/
	#define cdIMG_PROP_APP_SET_PARAM				0x00008e00
	/* Values for internal use.
	       Attribute : Read only
	       Size/Type : any / any
	*/
	#define cdIMG_PROP_TOTAL_FRAME_32				0x00008f00
	/* The number of total frames.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_FRAME_RATE_EX				0x00009800
	/* Frame rate (frames/sec)
	 	   Attribute : Read only
	       Size/Type : 8 / cdURational
	*/
	#define cdIMG_PROP_INTER_COLOR_PROFILE			0x00010600
	/* Inter-colorprofile embedded in the picture.
	       Attribute : Read only
	       Size/Type : any / cdUInt8
	*/


   /*----------------------------------------------------------                  
	  Image Data Property for EOS
  */
	#define cdIMG_PROP_WB_BRACKET_MODE			0x00009100
	/* WhiteBalance Bracket mode
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0:non-WBbracket	1:WBbracket(shiftAB)	2:WBbracket(shiftGM)	0xffff:Unknown
	*/
	#define cdIMG_PROP_WB_ACTBRACKET_VALUE_AB		0x00009200
	/* WhiteBalance ActBracketAB value
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WB_ACTBRACKET_VALUE_GM		0x00009300
	/* WhiteBalance ActBracketGM value
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WB_BALANCESHIFT_AB			0x00009400
	/* WhiteBalance shiftAB value
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_WB_BALANCESHIFT_GM			0x00009500
	/* WhiteBalance shiftGM value
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/

  /*----------------------------------------------------------                
	  Thumbnail Image Data Property 
  */
	#define cdIMG_PROP_THUMB_FORMAT					0x10000001
	/* Image data format.
	 	   Attribute : Read only
	       Size/Type : sizeof(cdImageFormat) / cdImageFormat
	*/
	#define cdIMG_PROP_THUMB_COMPONENT_BIT_DEPTH	0x10000002
	/* The number of bits per color component (such as 8).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_THUMB_COLOR_BIT_DEPTH		0x10000003
	/* The number of bits per color (such as 24).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_THUMB_WIDHT					0x10000004
	/* The width in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_THUMB_HEIGHT					0x10000005
	/* The height in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_THUMB_DENSITY_X				0x10000006
	/* The horizontal dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_THUMB_DENSITY_Y				0x10000007
	/* The vertical dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/

  /*----------------------------------------------------------                  
	  Picture Image Data (full resolution image data) Property 
  */
	#define cdIMG_PROP_PICT_FORMAT					0x20000001
	/* Image data format.
	 	   Attribute : Read only
	       Size/Type : sizeof(cdImageFormat) / cdImageFormat
	*/
	#define cdIMG_PROP_PICT_COMPONENT_BIT_DEPTH		0x20000002
	/*  The number of bits per color component (such as 8).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_PICT_COLOR_BIT_DEPTH			0x20000003
	/* The number of bits per color (such as 24).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_PICT_WIDHT					0x20000004
	/* The width in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_PICT_HEIGHT					0x20000005
	/* The height in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_PICT_DENSITY_X				0x20000006
	/* The horizontal dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_PICT_DENSITY_Y				0x20000007
	/* The vertical dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_PICT_NEED_DEVELOP			0x20000008	
	/* Indicates that the picture image data needs development. 
	   If this property is TRUE, client applications should develop
	   the picture image data by CDGetDevelopedImageData().
	 	   Attribute : Read only
	       Size/Type : sizeof(cdBoolean) / cdBoolean
	*/
	#define cdIMG_PROP_PICT_COMPRESSION_RATIO		0x20000010
	/* The compression ratio compared to 24-bit RGB.
	 	   Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_PICT_COMPRESSION_TYPE		0x20000011
	/* The type of compression used on the image data.
	 	   Attribute : Read only
	       Size/Type : 1 / cdCompressionType
	*/

  /*----------------------------------------------------------                  
	  Movie Data Property 
  */
	#define cdIMG_PROP_MOVIE_FORMAT					0x30000001
	/* Movie data format.
	 	   Attribute : Read only
	       Size/Type : sizeof(cdImageFormat) / cdImageFormat
	*/
	#define cdIMG_PROP_MOVIE_COMPONENT_BIT_DEPTH	0x30000002
	/* The number of bits per color component (such as 8).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_MOVIE_COLOR_BIT_DEPTH		0x30000003
	/* The number of bits per color (such as 24).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_MOVIE_WIDHT					0x30000004
	/* Width in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MOVIE_HEIGHT					0x30000005
	/* Height in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MOVIE_DENSITY_X				0x30000006
	/* The horizontal dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MOVIE_DENSITY_Y				0x30000007
	/* The vertical dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MOVIE_FRAME_RATE				0x30000009
	/* Frame rate (frames/sec)
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MOVIE_TOTAL_FRAME			0x3000000a
	/* The number of total frames.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdIMG_PROP_MOVIE_CREATOR_VERSION		0x3000000b
	/* Creator version that is the version number of the movie 
	   file generated by the Canon Digital Camera.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/

  /*----------------------------------------------------------                  
	  Sound Data Property 
  */
	#define cdIMG_PROP_SOUND_FORMAT					0x40000001
	/* Sound data format.
	 	   Attribute : Read only
	       Size/Type : sizeof(cdImageFormat) / cdImageFormat
	*/
	#define cdIMG_PROP_SOUND_SOUND_DURATION			0x4000000c
	/* The duration in seconds.
	 	   Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdIMG_PROP_SOUND_SAMPLING_FREQUENCY 	0x4000000d
	/* The number of samples per second.
	 	   Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdIMG_PROP_SOUND_SOUND_CHANNELS			0x4000000e
	/* The number of interleaved channels.
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdIMG_PROP_SOUND_BITS_PER_SAMPLE		0x4000000f
	/* The number of bits per sample.
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/

/* cdBaseImagePropertyID 
*/
typedef cdImagePropertyID cdBaseImagePropertyID;  
	#define cdBASE_IMG_PROP_FORMAT				0x00000001
	/* Data format.
	 	   Attribute : Read only
	       Size/Type : sizeof(cdImageFormat) / cdImageFormat
	*/
	#define cdBASE_IMG_PROP_COMPONENT_BIT_DEPTH	0x00000002
	/* The number of bits per color component (such as 8).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdBASE_IMG_PROP_COLOR_BIT_DEPTH		0x00000003
	/* The number of bits per color (such as 24).
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdBASE_IMG_PROP_WIDHT				0x00000004
	/* The width in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdBASE_IMG_PROP_HEIGHT				0x00000005
	/* The height in pixels.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdBASE_IMG_PROP_DENSITY_X			0x00000006
	/* The horizontal dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdBASE_IMG_PROP_DENSITY_Y			0x00000007
	/* The vertical dots per inch.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdBASE_IMG_PROP_NEED_DEVELOP		0x00000008	
	/* Indicates that the data needs development. 
	   If this property is TRUE, client applications should develop
	   the data by CDGetDevelopedImageData().
	 	   Attribute : Read only
	       Size/Type : sizeof(cdBoolean) / cdBoolean
	*/

	#define cdBASE_IMG_PROP_FRAME_RATE			0x00000009
	/* Flame rate (flames/sec)
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdBASE_IMG_PROP_TOTAL_FRAME			0x0000000a
	/* The number of total frames.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdBASE_IMG_PROP_CREATOR_VERSION		0x0000000b
	/* Creator version that is the version number of the movie 
	   file generated by the Canon Digital Camera.
	 	   Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/	

	#define cdBASE_IMG_PROP_SOUND_DURATION		0x0000000c
	/* The duration in seconds.
	 	   Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdBASE_IMG_PROP_SAMPLING_FREQUENCY 	0x0000000d
	/* The number of samples per second.
	 	   Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdBASE_IMG_PROP_SOUND_CHANNELS		0x0000000e
	/* The number of interleaved channels.
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdBASE_IMG_PROP_BITS_PER_SAMPLE		0x0000000f	
	/* The number of bits per sample.
	 	   Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/

	#define cdBASE_IMG_PROP_COMPRESSION_RATIO	0x00000010
	/* The compression ratio compared to 24-bit RGB.
	 	   Attribute : Read only
	       Size/Type : 4 / cdFloat
	*/
	#define cdBASE_IMG_PROP_COMPRESSION_TYPE	0x00000011
	/* The type of compression used on the image data.
	 	   Attribute : Read only
	       Size/Type : 1 / cdCompressionType
	*/
	#define cdBASE_IMG_PROP_ROTATION_ANGLE		0x00001800
	/* Rotation angle. (0, 90, 180, 270)
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/

/* cdDevicePropertyID 
*/
typedef cdUInt32 cdDevicePropertyID;  
	#define cdDEVICE_PROP_INVALID				0xFFFFFFFF
	/* Invalid ID
	*/

	#define cdDEVICE_PROP_MODEL_ID				0x00000001
	/* Camera model ID.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdDEVICE_PROP_MODEL_NAME			0x00000002
	/* Camera model name.
	       Attribute : Read only
	       Size/Type : 32 / cdChar[32]
	*/
	#define cdDEVICE_PROP_SLIDE_SHOW_CAP		0x00000003
	/* Slide show capacity.
	       Attribute : Read only
	       Size/Type : sizeof(cdSlideShowCap) / cdSlideShowCap
	*/
	#define cdDEVICE_PROP_UPLOAD_FILE_CAP		0x00000004
	/* AddPicture suport.
	       Attribute : Read only
	       Size/Type : sizeof(cdBoolean) / cdBoolean
	   Values:
	     TRUE : The device supports the CDAddPicture.
		 FALSE: The device does not support the CDAddPicture.
	*/
	#define cdDEVICE_PROP_ROTATION_CAP			0x00000005
	/* Image rotation suport.
	       Attribute : Read only
	       Size/Type : sizeof(cdBoolean) / cdBoolean
	   Values:
	     TRUE : The device supports the image rotation function.
		 FALSE: The device does not support the image rotation function.
	*/
	#define cdDEVICE_PROP_DPOF_CAP				0x00000006
	/* DPOF version number supported by the device.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	*/
	#define cdDEVICE_PROP_THUMB_VALID_AREA		0x00000007
	/* Valid area in the DCF thumbnail data.
	       Attribute : Read only
	       Size/Type :  / 
	*/
	#define cdDEVICE_PROP_RELEASE_CONTROL_CAP	0x00000008
	/* Remote release control capacity.
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	   Meaning:
		  If 0x40000000 bit is set, the camera supports the remote release control functions.
	*/
	#define cdDEVICE_PROP_RAW_DEVELOP_FACULTIES	0x00000009
	/* RAW file specification generated by the device.
	       Attribute : Read only
	       Size/Type : sizeof(cdRawDevelopFaculty) / cdRawDevelopFaculty
	*/
	#define cdDEVICE_PROP_PARSE_FACULTIES		0x0000000a
	/* File parser faculty. 
	       Attribute : Read only
	       Size/Type : sizeof(cdParseFaculty) / cdParseFaculty
	*/
	#define cdDEVICE_PROP_OWNER_NAME			0x0800000c	
	/* Owner's name. 
	       Attribute : Read / Write
	       Size/Type : 32 / cdChar[32]
	*/
	#define cdDEVICE_PROP_TIME					0x0800000d	
	/* Time set in the device. 
	       Attribute : Read / Write
	       Size/Type : 4 / cdTime
	*/
	#define cdDEVICE_PROP_FIRMWARE_VERSION		0x0000000e	
	/* Firmware version in the device. 
	       Attribute : Read only
	       Size/Type : 32 / cdChar[32]
	*/
	#define cdDEVICE_PROP_BATTERY_STATUS		0x0000000f	
	/* Battery status of the device. 
	       Attribute : Read only
	       Size/Type : 4 / cdUInt32
	   Values:
	    Refer to the description of the cdEVENT_BATTERY_LEVEL_CHANGED event 
		in the cdEvent.h.
	*/
	#define cdDEVICE_PROP_DIRECT_TRANSFER_STATUS		0x00000010	
	/* Direct transfer status of the device. 
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/


/*-----------------------------------------------------------------------
   Constants for remote release control  
------------------------------------------------------------------------*/
/* cdRelCamSettingID 
*/
typedef cdUInt32 cdRelCamSettingID;  
	#define cdREL_SET_SELF_TIMER				0x00000001
	/* Self timer setting. This value is valid when drive mode value get by 
	   CDGetDriveMode is cdDRIVE_MODE_SELFTIMER.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0     : No self timer shooting
		100   : Self timer shooting (10sec).
		0xffff: Invalid
	*/
	#define cdREL_SET_BEEP						0x08000002
	/* Beep setting.
	       Attribute : Depends on the camera model.
	       Size/Type : 1 / cdUInt8
	   Values:
	    0: off   1: on   0xff:Invalid
	*/
	#define cdREL_SET_EZOOM						0x00000003
	/* Digital Teleconverter.
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	   Values:
	    0   : No teleconverter.   
		1   : Continuous (Smooth) 
		0xff: Invalid
	*/
	#define cdREL_SET_MLWEI_MODE				0x08000004
	/* Measure light weight mode.
	       Attribute : Depends on the camera model.
	       Size/Type : 1 / cdUInt8
	   Values:								*/
	    #define cdREL_VAL_MLWEI_MODE_CENTER_WEI_AVERAGE   	(0)		/* Center-weighted averaging	*/
		#define cdREL_VAL_MLWEI_MODE_SPOT   				(1)		/* Spot							*/
		#define cdREL_VAL_MLWEI_MODE_AVERAGE   				(2)		/* Averaging					*/	
		#define cdREL_VAL_MLWEI_MODE_EVALUATIVE   			(3)		/* Evaluative					*/	
		#define cdREL_VAL_MLWEI_MODE_PART    				(4)		/* Partial						*/
		#define cdREL_VAL_MLWEI_MODE_CENTER_WEI_AVERAGE_2 	(5)		/* Center-weighted averaging	*/	
		#define cdREL_VAL_MLWEI_MODE_UNKNOWN   				(0xff)	/* Invalid						*/
	
	#define cdREL_SET_FOCUS_POINT				0x08000005
	/* Measure light weight mode.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdUInt16
	   Meaning:
	    Upper 4bits is 0: Means this value is invalid.
		Upper 4bits is 1: Means center focus point selection.
		Upper 4bits is 3: Means 3 points focus selection. Checks lower 12bits 
		                  further;   
					       0: No focus point is selected. 
						      Manual focus point selection.   
						   1: Automatic focus point selection.    
						   2. Right focus point selection.   
						   3. Center focus point selection.   
						   4. Left focus point selection
	   Useful definitions:                        */
		/* Upper 4bits */
		#define cdREL_VAL_FOCUS_POINT_NA				(0x0000)
		#define cdREL_VAL_FOCUS_POINT_CENTER_ONLY		(0x1000)
		#define cdREL_VAL_FOCUS_POINT_3_POINTS			(0x3000)
		/* Lower 12bits when cdFOCUS_POINT_3_POINTS */
		#define cdREL_VAL_FOCUS_POINT_MF				(0x0000)
		#define cdREL_VAL_FOCUS_POINT_AUTO				(0x0001)
		#define cdREL_VAL_FOCUS_POINT_RIGHT				(0x0002)
		#define cdREL_VAL_FOCUS_POINT_CENTER			(0x0003)
		#define cdREL_VAL_FOCUS_POINT_LEFT				(0x0004)

			#define cdREL_VAL_FOCUS_POINT_AF_AUTO		(cdREL_VAL_FOCUS_POINT_3_POINTS | cdREL_VAL_FOCUS_POINT_AUTO)
			#define cdREL_VAL_FOCUS_POINT_AF_RIGHT		(cdREL_VAL_FOCUS_POINT_3_POINTS | cdREL_VAL_FOCUS_POINT_RIGHT)
			#define cdREL_VAL_FOCUS_POINT_AF_CENTER		(cdREL_VAL_FOCUS_POINT_3_POINTS | cdREL_VAL_FOCUS_POINT_CENTER)
			#define cdREL_VAL_FOCUS_POINT_AF_LEFT		(cdREL_VAL_FOCUS_POINT_3_POINTS | cdREL_VAL_FOCUS_POINT_LEFT)


	#define cdREL_SET_SLOW_SHUTTER				0x00000006
	/* Slow shutter setting
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	   Values:
	    0: off
	    1: night scene
	    2: on
	    3: not existing
		0xff: invalid
	*/
	#define cdREL_SET_AF_MODE					0x00000007
	/* Fucus mode setting
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	   Values:
	    0: One-Shot AF / Single AF	
		1: AI Servo AF / Continuous AF	
		2: AI Focus	AF
		3: Manual Focus	
		0xff: Invalid
	*/
	#define cdREL_SET_CONTRAST					0x08000008
	/* Contrast setting
	       Attribute : Depends on the camera model.
	       Size/Type : 1 / cdInt8
	   Values:														*/
		#define cdREL_VAL_CONTRAST_LOW			(-1)	/* Low		*/
		#define cdREL_VAL_CONTRAST_NORMAL		(0)		/* Normal	*/
		#define cdREL_VAL_CONTRAST_HIGH			(1)		/* High		*/
		#define cdREL_VAL_CONTRAST_INVALID		(0x7f)	/* Invalid	*/
	
	#define cdREL_SET_COLOR_GAIN				0x08000009
	/* Color saturation setting
	       Attribute : Depends on the camera model.
	       Size/Type : 1 / cdInt8
	   Values:														*/
		#define cdREL_VAL_COLOR_GAIN_LOW		(-1)	/* Low		*/
		#define cdREL_VAL_COLOR_GAIN_NORMAL		(0)		/* Normal	*/
		#define cdREL_VAL_COLOR_GAIN_HIGH		(1)		/* High		*/
		#define cdREL_VAL_COLOR_GAIN_INVALID	(0x7f)	/* Invalid	*/

	#define cdREL_SET_SHARPNESS					0x0800000a
	/* Sharpness setting
	       Attribute : Depends on the camera model.
	       Size/Type : 1 / cdInt8
	   Values:														*/
		#define cdREL_VAL_SHARPNESS_LOW			(-1)	/* Low		*/
		#define cdREL_VAL_SHARPNESS_NORMAL		(0)		/* Normal	*/
		#define cdREL_VAL_SHARPNESS_HIGH		(1)		/* High		*/
		#define cdREL_VAL_SHARPNESS_INVALID		(0x7f)	/* Invalid	*/

	#define cdREL_SET_PARAMETER_SET				0x0000000b
	/* Parameters Set information.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Meaning:
		Lower 4bit: Means the current selection number of the parameter set 
		            in the camera. 	
		               0: Standard set.	
					   1: Set 1	
					   2: Set 2	
					   3: Set 3
		bit4: Means if the standard parameter set is selectable. 
		bit5: Means if the parameter set 1 is selectable
		bit6: Means if the parameter set 2 is selectable
		bit7: Means if the parameter set 3 is selectable        
		
		  0xffff:	Invalid
	*/
	#define cdREL_SET_ISO_SPEED_RATINGS			0x0800000c
	/* ISO speed setting.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdUInt16
	   Values:													*/
	    #define cdREL_VAL_ISO_AUTO		(0x0000)	/* Auto		*/
	    #define cdREL_VAL_ISO_6			(0x0028)	/* ISO 6	*/
	    #define cdREL_VAL_ISO_8			(0x002b)	/* ISO 8	*/
	    #define cdREL_VAL_ISO_10		(0x002d)	/* ISO 10	*/
	    #define cdREL_VAL_ISO_12		(0x0030)	/* ISO 12	*/
	    #define cdREL_VAL_ISO_16		(0x0033)	/* ISO 16	*/
	    #define cdREL_VAL_ISO_20		(0x0035)	/* ISO 20	*/
	    #define cdREL_VAL_ISO_25		(0x0038)	/* ISO 25	*/
	    #define cdREL_VAL_ISO_32		(0x003b)	/* ISO 32	*/
	    #define cdREL_VAL_ISO_40		(0x003d)	/* ISO 40	*/
	    #define cdREL_VAL_ISO_50		(0x0040)	/* ISO 50	*/
	    #define cdREL_VAL_ISO_64		(0x0043)	/* ISO 64	*/
	    #define cdREL_VAL_ISO_80		(0x0045)	/* ISO 80	*/
	    #define cdREL_VAL_ISO_100		(0x0048)	/* ISO 100	*/
	    #define cdREL_VAL_ISO_125		(0x004b)	/* ISO 125	*/
	    #define cdREL_VAL_ISO_160		(0x004d)	/* ISO 160	*/
	    #define cdREL_VAL_ISO_200		(0x0050)	/* ISO 200	*/
	    #define cdREL_VAL_ISO_250		(0x0053)	/* ISO 250	*/
	    #define cdREL_VAL_ISO_320		(0x0055)	/* ISO 320	*/
	    #define cdREL_VAL_ISO_400		(0x0058)	/* ISO 400	*/
	    #define cdREL_VAL_ISO_500		(0x005b)	/* ISO 500	*/
	    #define cdREL_VAL_ISO_640		(0x005d)	/* ISO 640	*/
	    #define cdREL_VAL_ISO_800		(0x0060)	/* ISO 800	*/
	    #define cdREL_VAL_ISO_1000		(0x0063)	/* ISO 1000	*/
	    #define cdREL_VAL_ISO_1250		(0x0065)	/* ISO 1250	*/
	    #define cdREL_VAL_ISO_1600		(0x0068)	/* ISO 1600	*/
	    #define cdREL_VAL_ISO_2000		(0x006b)	/* ISO 2000	*/
	    #define cdREL_VAL_ISO_2500		(0x006d)	/* ISO 2500	*/
	    #define cdREL_VAL_ISO_3200		(0x0070)	/* ISO 3200	*/
	    #define cdREL_VAL_ISO_4000		(0x0073)	/* ISO 4000	*/
	    #define cdREL_VAL_ISO_5000		(0x0075)	/* ISO 5000	*/
	    #define cdREL_VAL_ISO_6400		(0x0078)	/* ISO 6400	*/
		#define cdREL_VAL_ISO_NA		(0xffff)	/* Invalid	*/
	
	#define cdREL_SET_FLASH_COMP				0x0800000d
	/* ISO speed setting.
	       Attribute : Depends on the camera model.
	       Size/Type : 1 / cdUInt8
	   Values:
	    Same as lower 1 byte of cdCompensation.
	*/
	#define cdREL_SET_AEB_EXPOSURE_COMP			0x0000000e
	/* AEB exposure compensation value.
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	   Values:
	    Same as lower 1 byte of cdCompensation.
	*/
	#define cdREL_SET_EF_LENS_ID				0x0000000f
	/* EF lens ID.
	       Attribute : Read only
	       Size/Type : 1 / cdUInt8
	*/
	#define cdREL_SET_AV_OPEN					0x00000020
	/* This ID and its value is invalid.
	*/
	#define cdREL_SET_AV_MAX					0x00000021
	/* Maximum Av value.
	       Attribute : Read only
	       Size/Type : 2 / cdRemoteSetAv
	*/
	#define cdREL_SET_FOCAL_LENGTH				0x00000022
	/* The numerator value of the current focal length value. 
	   The denominator value is the value gotten by 
	   cdREL_SET_FOCAL_LENGTH_DENOMINATOR.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0xffff: Invalid
	*/
	#define cdREL_SET_FOCAL_LENGTH_TELE			0x00000023
	/* The numerator value of the focal length value of the tele end. 
	   The denominator value is the value gotten by 
	   cdREL_SET_FOCAL_LENGTH_DENOMINATOR.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0xffff: Invalid
	*/
	#define cdREL_SET_FOCAL_LENGTH_WIDE			0x00000024
	/* The numerator value of the focal length value of the wide end. 
	   The denominator value is the value gotten by 
	   cdREL_SET_FOCAL_LENGTH_DENOMINATOR.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0xffff: Invalid
	*/
	#define cdREL_SET_FOCAL_LENGTH_DENOMINATOR	0x00000025
	/* The denominator value for the focal length values.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0xffff: Invalid
	*/
	#define cdREL_SET_DISP_AV					0x00000100
	/* The meaning of the value gotten by cdREL_SET_AV_OPEN_APEX.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Meaning:
	    See next.
	*/
	#define cdREL_SET_AV_OPEN_APEX				0x00000200
	/* The smallest aperture value (larger aperture) for display.
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	   Values:
	    If the value gotten by cdREL_SET_DISP_AV is 0, 
		this value shows the APEX value of the aperture value.
		
	    If the value gotten by cdREL_SET_DISP_AV is 1, 
		this value shows the display value by which ten times are done. 
		For instance, if the value gotten by cdREL_SET_DISP_AV is 1, 
		and this value is 27, AV becomes 2.7.
	*/
	#define cdREL_SET_EZOOM_SIZE				0x00000300
	/* The image horizontal size cut out from the CCD at current digital 
	   zoom position.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Value:
	    This value will be maximum when camera is not digital zoom position.
	*/
	#define cdREL_SET_FOCAL_ML_SPOT_POS			0x08000400
	/* Spot AE point.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Value:
	    0: Center.
		    Meters the area within the spot AE point frame at the center.
	    1: AF point.
	        Meters the spot AF point corresponding to the selected AF frame.
	*/
	#define cdREL_SET_DISP_AV_MAX				0x00000500
	/* The meaning of the value gotten by cdREL_SET_AV_MAX_APEX.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Meaning:
	    See next.
	*/
	#define cdREL_SET_AV_MAX_APEX				0x00000600
	/* The maximum aperture value (smaller aperture) for display.
	       Attribute : Read only
	       Size/Type : 2 / cdInt16
	   Values:
	    If the value gotten by cdREL_SET_DISP_AV_MAX is 0, 
		this value shows the APEX value of the aperture value.
		
	    If the value gotten by cdREL_SET_DISP_AV_MAX is 1, 
		this value shows the display value by which ten times are done. 
		For instance, if the value gotten by cdREL_SET_DISP_AV_MAX is 1, 
		and this value is 80, AV becomes 8.0.
	*/
	#define cdREL_SET_FOCAL_LENGTH_OF_TELE		0x00000900
	/* Focal length at the optical tele end.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdREL_SET_EZOOM_SIZE_TELE			0x00000a00
	/* The image horizontal size cut out from the CCD at tele end of 
	   the digital zoom position.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdREL_SET_PHOTO_EFFECT				0x08000b00
	/* Photo effect mode.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdPhotoEffect
	*/
	#define cdREL_SET_AF_LIGHT					0x08000c00
	/* AF-assist beam setting.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: Off
		1: On
	*/
	#define cdREL_SET_FLASH_QUANTITY_COUNT		0x00000d00
	/* Number of levels of quantity of flash luminescence which can be set 
	   in the Manual mode.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	*/
	#define cdREL_SET_FLASH_QUANTITY			0x08000e00
	/* Quantity of flash luminescence.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdUInt16
	   Meaning:
	    The range of this value is between from 0 to (FlashLevelCount - 1).
		0 means maximum luminescence.
		(FlashLevelCount - 1) means minimum luminescence.
		This is valid when flash setting is cdFLASH_MODE_ON or cdFLASH_MODE_ON_PLUS_RED_EYE
		with shooting mode is cdSHOOTING_MODE_MANUAL.
	*/
	#define cdREL_SET_ROTATION_ANGLE			0x00000f00
	/* Rotation angle detected with the Auto Rotate Sensor.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: 0 degree
		1: 90 degrees
		2: 180 degrees
		3: 270 degrees
		0xffff: Unknown
	*/
	#define cdREL_SET_ROTATION_SENSE			0x00001000
	/* Auto Rotate Senser setting.
	       Attribute : Read only
	       Size/Type : 2 / cdUInt16
	   Values:
	    0: Enable
		1: Disable
		0xffff: Not available.
	*/
	#define cdREL_SET_KELVIN_VALUE				0x00001100
	/* Kelvin value of WB setting.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdUInt16
	   Values:
	    Kelvin value.
	*/
	#define cdREL_SET_COLOR_MATRIX				0x00001200
	/* Color matrix.
	       Attribute : Depends on the camera model.
	       Size/Type : 2 / cdColorMatrix
	*/



/*----------------------------------------------*/

/* cdRelDataKind
	The type and location for the data to be generated in remote release mode. */
typedef cdUInt16 cdRelDataKind;  
	#define cdREL_KIND_THUMB_TO_PC				0x0001
	#define cdREL_KIND_PICT_TO_PC				0x0002
	#define cdREL_KIND_THUMB_TO_CAM				0x0004
	#define cdREL_KIND_PICT_TO_CAM				0x0008

/* cdRelViewfinderOutput
	The output destination for the Viewfinder data from the camera. */
typedef cdUInt32 cdRelViewfinderOutput;  
	#define cdREL_FINDER_TO_LCD					0x00000001
	#define cdREL_FINDER_TO_VIDEO				0x00000002
	#define cdREL_FINDER_TO_OFF					0x00000003

/* cdFlashMode
*/
typedef cdUInt16 cdFlashMode;  
	#define cdFLASH_MODE_OFF				(0x0000)
	#define cdFLASH_MODE_AUTO				(0x0001)
	#define cdFLASH_MODE_ON					(0x0002)
	#define cdFLASH_MODE_RED_EYE			(0x0003)
	#define cdFLASH_MODE_SLOW_SYNC			(0x0004)
	#define cdFLASH_MODE_AUTO_PLUS_RED_EYE	(0x0005)
	#define cdFLASH_MODE_ON_PLUS_RED_EYE	(0x0006)

	#define cdFLASH_MODE_NA					(0x00ff)

/* cdCompensation
	Amount of compensation. */
typedef cdUInt16 cdCompensation;  
	#define cdCOMP_300_PLUS			(0x0000)
	#define cdCOMP_266_PLUS 		(0x0003)
	#define cdCOMP_250_PLUS			(0x0004)
	#define cdCOMP_233_PLUS			(0x0005)
	#define cdCOMP_200_PLUS			(0x0008)
	#define cdCOMP_166_PLUS			(0x000b)
	#define cdCOMP_150_PLUS			(0x000c)
	#define cdCOMP_133_PLUS			(0x000d)
	#define cdCOMP_100_PLUS			(0x0010)
	#define cdCOMP_066_PLUS			(0x0013)
	#define cdCOMP_050_PLUS			(0x0014)
	#define cdCOMP_033_PLUS			(0x0015)
	#define cdCOMP_000_PLUS			(0x0018)
	#define cdCOMP_033_MINUS		(0x001b)
	#define cdCOMP_050_MINUS		(0x001c)
	#define cdCOMP_066_MINUS		(0x001d)
	#define cdCOMP_100_MINUS		(0x0020)
	#define cdCOMP_133_MINUS		(0x0023)
	#define cdCOMP_150_MINUS		(0x0024)
	#define cdCOMP_166_MINUS		(0x0025)
	#define cdCOMP_200_MINUS		(0x0028)
	#define cdCOMP_233_MINUS		(0x002b)
	#define cdCOMP_250_MINUS		(0x002c)
	#define cdCOMP_266_MINUS		(0x002d)
	#define cdCOMP_300_MINUS		(0x0030)
	#define cdCOMP_NA				(0x00ff)

/* cdRemoteSetAv
	Av in remote release control mode. */
typedef cdUInt16 cdRemoteSetAv;
	#define cdREMOTE_SET_AV_00		(0x0000)	/* No Lens */
	#define cdREMOTE_SET_AV_NA 		(0xffff)
	#define cdREMOTE_SET_AV_Open 	(0x7fff)
	#define cdREMOTE_SET_AV_Max		(0x7ffe)

	/* 1/3 stop */
	#define cdREMOTE_SET_AV3_10		(0x0008)
	#define cdREMOTE_SET_AV3_11		(0x000b)
	#define cdREMOTE_SET_AV3_12		(0x000d)
	#define cdREMOTE_SET_AV3_14		(0x0010)
	#define cdREMOTE_SET_AV3_16		(0x0013)
	#define cdREMOTE_SET_AV3_18		(0x0015)
	#define cdREMOTE_SET_AV3_20		(0x0018)
	#define cdREMOTE_SET_AV3_22		(0x001b)
	#define cdREMOTE_SET_AV3_25		(0x001d)
	#define cdREMOTE_SET_AV3_28		(0x0020)
	#define cdREMOTE_SET_AV3_32		(0x0023)
	#define cdREMOTE_SET_AV3_35		(0x0025)
	#define cdREMOTE_SET_AV3_40		(0x0028)
	#define cdREMOTE_SET_AV3_45		(0x002b)
	#define cdREMOTE_SET_AV3_50		(0x002d)
	#define cdREMOTE_SET_AV3_56		(0x0030)
	#define cdREMOTE_SET_AV3_63		(0x0033)
	#define cdREMOTE_SET_AV3_71		(0x0035)
	#define cdREMOTE_SET_AV3_80		(0x0038)
	#define cdREMOTE_SET_AV3_90		(0x003b)
	#define cdREMOTE_SET_AV3_100	(0x003d)
	#define cdREMOTE_SET_AV3_110	(0x0040)
	#define cdREMOTE_SET_AV3_130	(0x0043)
	#define cdREMOTE_SET_AV3_140	(0x0045)
	#define cdREMOTE_SET_AV3_160	(0x0048)
	#define cdREMOTE_SET_AV3_180	(0x004b)
	#define cdREMOTE_SET_AV3_200	(0x004d)
	#define cdREMOTE_SET_AV3_220	(0x0050)
	#define cdREMOTE_SET_AV3_250	(0x0053)
	#define cdREMOTE_SET_AV3_290	(0x0055)
	#define cdREMOTE_SET_AV3_320	(0x0058)
	#define cdREMOTE_SET_AV3_360	(0x005b)
	#define cdREMOTE_SET_AV3_400	(0x005d)
	#define cdREMOTE_SET_AV3_450	(0x0060)
	#define cdREMOTE_SET_AV3_510	(0x0063)
	#define cdREMOTE_SET_AV3_570	(0x0065)
	#define cdREMOTE_SET_AV3_640	(0x0068)
	#define cdREMOTE_SET_AV3_720	(0x006b)
	#define cdREMOTE_SET_AV3_810	(0x006d)
	#define cdREMOTE_SET_AV3_910	(0x0070)

	/* 1/2 stop */
	#define cdREMOTE_SET_AV2_10		(0x0008)
	#define cdREMOTE_SET_AV2_12		(0x000c)
	#define cdREMOTE_SET_AV2_14		(0x0010)
	#define cdREMOTE_SET_AV2_18		(0x0014)
	#define cdREMOTE_SET_AV2_20		(0x0018)
	#define cdREMOTE_SET_AV2_25		(0x001c)
	#define cdREMOTE_SET_AV2_28		(0x0020)
	#define cdREMOTE_SET_AV2_35		(0x0024)
	#define cdREMOTE_SET_AV2_40		(0x0028)
	#define cdREMOTE_SET_AV2_45		(0x002c)
	#define cdREMOTE_SET_AV2_56		(0x0030)
	#define cdREMOTE_SET_AV2_67		(0x0034)
	#define cdREMOTE_SET_AV2_80		(0x0038)
	#define cdREMOTE_SET_AV2_95		(0x003c)
	#define cdREMOTE_SET_AV2_110	(0x0040)
	#define cdREMOTE_SET_AV2_130	(0x0044)
	#define cdREMOTE_SET_AV2_160	(0x0048)
	#define cdREMOTE_SET_AV2_190	(0x004c)
	#define cdREMOTE_SET_AV2_220	(0x0050)
	#define cdREMOTE_SET_AV2_270	(0x0054)
	#define cdREMOTE_SET_AV2_320	(0x0058)
	#define cdREMOTE_SET_AV2_380	(0x005c)
	#define cdREMOTE_SET_AV2_450	(0x0060)
	#define cdREMOTE_SET_AV2_540	(0x0064)
	#define cdREMOTE_SET_AV2_640	(0x0068)
	#define cdREMOTE_SET_AV2_760	(0x006c)
	#define cdREMOTE_SET_AV2_910	(0x0070)

	/* 1 stop */
	#define cdREMOTE_SET_AV1_10		(0x0008)
	#define cdREMOTE_SET_AV1_14		(0x0010)
	#define cdREMOTE_SET_AV1_20		(0x0018)
	#define cdREMOTE_SET_AV1_28		(0x0020)
	#define cdREMOTE_SET_AV1_40		(0x0028)
	#define cdREMOTE_SET_AV1_56		(0x0030)
	#define cdREMOTE_SET_AV1_80		(0x0038)
	#define cdREMOTE_SET_AV1_110	(0x0040)
	#define cdREMOTE_SET_AV1_160	(0x0048)
	#define cdREMOTE_SET_AV1_220	(0x0050)
	#define cdREMOTE_SET_AV1_320	(0x0058)
	#define cdREMOTE_SET_AV1_450	(0x0060)
	#define cdREMOTE_SET_AV1_640	(0x0068)
	#define cdREMOTE_SET_AV1_910	(0x0070)

/* cdRemoteSetTv
	Tv in remote release control mode. */
typedef cdUInt16 cdRemoteSetTv;

	#define cdREMOTE_SET_TV_NA    	(0xffff)
	#define cdREMOTE_SET_TV_BULB	(0x0004)

	/* 1/3 stop */
	#define cdREMOTE_SET_TV3_30sec	(0x0010)
	#define cdREMOTE_SET_TV3_25sec	(0x0013)
	#define cdREMOTE_SET_TV3_20sec	(0x0015)
	#define cdREMOTE_SET_TV3_15sec	(0x0018)
	#define cdREMOTE_SET_TV3_13sec	(0x001b)
	#define cdREMOTE_SET_TV3_10sec	(0x001d)
	#define cdREMOTE_SET_TV3_8sec	(0x0020)
	#define cdREMOTE_SET_TV3_6sec	(0x0023)
	#define cdREMOTE_SET_TV3_5sec	(0x0025)
	#define cdREMOTE_SET_TV3_4sec	(0x0028)
	#define cdREMOTE_SET_TV3_3sec2	(0x002b)
	#define cdREMOTE_SET_TV3_2sec5	(0x002d)
	#define cdREMOTE_SET_TV3_2sec	(0x0030)
	#define cdREMOTE_SET_TV3_1sec6	(0x0033)
	#define cdREMOTE_SET_TV3_1sec3	(0x0035)
	#define cdREMOTE_SET_TV3_1sec	(0x0038)
	#define cdREMOTE_SET_TV3_0sec8	(0x003b)
	#define cdREMOTE_SET_TV3_0sec6	(0x003d)
	#define cdREMOTE_SET_TV3_0sec5	(0x0040)
	#define cdREMOTE_SET_TV3_0sec4	(0x0043)
	#define cdREMOTE_SET_TV3_0sec3	(0x0045)
	#define cdREMOTE_SET_TV3_4		(0x0048)
	#define cdREMOTE_SET_TV3_5		(0x004b)
	#define cdREMOTE_SET_TV3_6		(0x004d)
	#define cdREMOTE_SET_TV3_8		(0x0050)
	#define cdREMOTE_SET_TV3_10		(0x0053)
	#define cdREMOTE_SET_TV3_13		(0x0055)
	#define cdREMOTE_SET_TV3_15		(0x0058)
	#define cdREMOTE_SET_TV3_20		(0x005b)
	#define cdREMOTE_SET_TV3_25		(0x005d)
	#define cdREMOTE_SET_TV3_30		(0x0060)
	#define cdREMOTE_SET_TV3_40		(0x0063)
	#define cdREMOTE_SET_TV3_50		(0x0065)
	#define cdREMOTE_SET_TV3_60		(0x0068)
	#define cdREMOTE_SET_TV3_80		(0x006b)
	#define cdREMOTE_SET_TV3_100	(0x006d)
	#define cdREMOTE_SET_TV3_125	(0x0070)
	#define cdREMOTE_SET_TV3_160	(0x0073)
	#define cdREMOTE_SET_TV3_200	(0x0075)
	#define cdREMOTE_SET_TV3_250	(0x0078)
	#define cdREMOTE_SET_TV3_320	(0x007b)
	#define cdREMOTE_SET_TV3_400	(0x007d)
	#define cdREMOTE_SET_TV3_500	(0x0080)
	#define cdREMOTE_SET_TV3_640	(0x0083)
	#define cdREMOTE_SET_TV3_800	(0x0085)
	#define cdREMOTE_SET_TV3_1000	(0x0088)
	#define cdREMOTE_SET_TV3_1250	(0x008b)
	#define cdREMOTE_SET_TV3_1600	(0x008d)
	#define cdREMOTE_SET_TV3_2000	(0x0090)
	#define cdREMOTE_SET_TV3_2500	(0x0093)
	#define cdREMOTE_SET_TV3_3200	(0x0095)
	#define cdREMOTE_SET_TV3_4000	(0x0098)
	#define cdREMOTE_SET_TV3_5000	(0x009b)
	#define cdREMOTE_SET_TV3_6400	(0x009d)
	#define cdREMOTE_SET_TV3_8000	(0x00a0)
	#define cdREMOTE_SET_TV3_10000	(0x00a3)
	#define cdREMOTE_SET_TV3_12800	(0x00a5)
	#define cdREMOTE_SET_TV3_16000	(0x00a8)

	/* 1/2 stop */
	#define cdREMOTE_SET_TV2_30sec	(0x0010)
	#define cdREMOTE_SET_TV2_20sec	(0x0014)
	#define cdREMOTE_SET_TV2_15sec	(0x0018)
	#define cdREMOTE_SET_TV2_10sec	(0x001c)
	#define cdREMOTE_SET_TV2_8sec	(0x0020)
	#define cdREMOTE_SET_TV2_6sec	(0x0024)
	#define cdREMOTE_SET_TV2_4sec	(0x0028)
	#define cdREMOTE_SET_TV2_3sec	(0x002c)
	#define cdREMOTE_SET_TV2_2sec	(0x0030)
	#define cdREMOTE_SET_TV2_1sec5	(0x0034)
	#define cdREMOTE_SET_TV2_1sec	(0x0038)
	#define cdREMOTE_SET_TV2_0sec7	(0x003C)
	#define cdREMOTE_SET_TV2_0sec5	(0x0040)
	#define cdREMOTE_SET_TV2_0sec3	(0x0044)
	#define cdREMOTE_SET_TV2_4		(0x0048)
	#define cdREMOTE_SET_TV2_6		(0x004c)
	#define cdREMOTE_SET_TV2_8		(0x0050)
	#define cdREMOTE_SET_TV2_10		(0x0054)
	#define cdREMOTE_SET_TV2_15		(0x0058)
	#define cdREMOTE_SET_TV2_20		(0x005c)
	#define cdREMOTE_SET_TV2_30		(0x0060)
	#define cdREMOTE_SET_TV2_45		(0x0064)
	#define cdREMOTE_SET_TV2_60		(0x0068)
	#define cdREMOTE_SET_TV2_90		(0x006c)
	#define cdREMOTE_SET_TV2_125	(0x0070)
	#define cdREMOTE_SET_TV2_180	(0x0074)
	#define cdREMOTE_SET_TV2_250	(0x0078)
	#define cdREMOTE_SET_TV2_350	(0x007c)
	#define cdREMOTE_SET_TV2_500	(0x0080)
	#define cdREMOTE_SET_TV2_750	(0x0084)
	#define cdREMOTE_SET_TV2_1000	(0x0088)
	#define cdREMOTE_SET_TV2_1500	(0x008c)
	#define cdREMOTE_SET_TV2_2000	(0x0090)
	#define cdREMOTE_SET_TV2_3000	(0x0094)
	#define cdREMOTE_SET_TV2_4000	(0x0098)
	#define cdREMOTE_SET_TV2_6000	(0x009c)
	#define cdREMOTE_SET_TV2_8000	(0x00a0)
	#define cdREMOTE_SET_TV2_12000	(0x00a4)
	#define cdREMOTE_SET_TV2_16000	(0x00a8)

	/* 1 stop */
	#define cdREMOTE_SET_TV1_30sec	(0x0010)
	#define cdREMOTE_SET_TV1_15sec	(0x0018)
	#define cdREMOTE_SET_TV1_8sec	(0x0020)
	#define cdREMOTE_SET_TV1_4sec	(0x0028)
	#define cdREMOTE_SET_TV1_2sec	(0x0030)
	#define cdREMOTE_SET_TV1_1sec	(0x0038)
	#define cdREMOTE_SET_TV1_0sec5	(0x0040)
	#define cdREMOTE_SET_TV1_4		(0x0048)
	#define cdREMOTE_SET_TV1_8		(0x0050)
	#define cdREMOTE_SET_TV1_15		(0x0058)
	#define cdREMOTE_SET_TV1_30		(0x0060)
	#define cdREMOTE_SET_TV1_60		(0x0068)
	#define cdREMOTE_SET_TV1_125	(0x0070)
	#define cdREMOTE_SET_TV1_250	(0x0078)
	#define cdREMOTE_SET_TV1_500	(0x0080)
	#define cdREMOTE_SET_TV1_1000	(0x0088)
	#define cdREMOTE_SET_TV1_2000	(0x0090)
	#define cdREMOTE_SET_TV1_4000	(0x0098)
	#define cdREMOTE_SET_TV1_8000	(0x00a0)
	#define cdREMOTE_SET_TV1_16000	(0x00a8)



/*-----------------------------------------------------------------------
   Constants  for RAW development   
------------------------------------------------------------------------*/
/* cdWBLightSrc
	Light source of the white balance. */
typedef cdUInt16   cdWBLightSrc;
	#define cdWB_ATCAPTURE 			(0x00ff)  /* As shot value	*/
	#define cdWB_BY_POS				(0x8000)  /* White Point position */
	#define cdWB_USE_COEFFICIENT	(0x0fff)  /* WB coefficient	*/
	#define cdWB_INVALID			(0x7fff)  /* Invalid Value  */

	#define cdWB_AUTO				(0x0000)  /* Auto */
	#define cdWB_DAY_LIGHT			(0x0001)  /* Daylight */
	#define cdWB_CLOUDY				(0x0002)  /* Cloudy */
	#define cdWB_TUNGSTEN			(0x0003)  /* Tungsten */
	#define cdWB_FLUORESCENT		(0x0004)  /* Fluorescent */
	#define cdWB_FLASH				(0x0005)  /* Flash */
	#define cdWB_CUSTOM				(0x0006)  /* Custom */
	  /* ++ You can not specify cdWB_CUSTOM for the RAW which was not shot with Custom mode */
	#define cdWB_BW					(0x0007)    
	#define cdWB_SHADE				(0x0008)  /* Shade for Type B  (and Type C)		*/
	#define cdWB_KELVIN				(0x0009)  /* Kelvin for Type B  (and Type C) 	*/
	#define cdWB_PCSET1				(0x000a)  /* PC Set1 for Type B 				*/
	#define cdWB_PCSET2				(0x000b)  /* PC Set2 for Type B 				*/
	#define cdWB_PCSET3				(0x000c)  /* PC Set3 for Type B 				*/
	#define cdWB_FLUORESCENT_H		(0x000e)  /* Fluorescent H 						*/
	#define cdWB_CUSTOM_1			(0x000f)  /* Custom 1							*/
	#define cdWB_CUSTOM_2			(0x0010)  /* Custom 2							*/



/* cdRawDevelopFaculty
	Faculty of RAW development. */
typedef cdUInt32   cdRawDevelopFaculty;
	#define cdDEV_RANGE_1				(0x00000001)
	#define cdDEV_RANGE_2				(0x00000002)
	#define cdDEV_RANGE_WB_1			(0x00000100)
	#define cdDEV_RANGE_WB_2			(0x00000200)
	#define cdDEV_RANGE_WB_3			(0x00000400)
	#define cdDEV_RANGE_PE_1			(0x00100000)
	#define cdDEV_FORMAT_16_BIT			(0x00010000)
	#define cdDEV_ADOBE_RGB				(0x00020000)	
	#define cdDEV_FALSE_COLOR_FILTER	(0x00001000)
	#define cdDEV_FALSE_RESO_FILTER		(0x00002000)
		
			#define cdDEV_TYPE_RANGE_1			cdDEV_RANGE_1	
			#define cdDEV_TYPE_RANGE_WB_1		cdDEV_RANGE_WB_1
			#define cdDEV_TYPE_RANGE_NOISE_1	cdDEV_FALSE_COLOR_FILTER

	/*
	cdDEV_RANGE_1:
		Contrast, colorDepth, enhance (and hue) can be set the value from -1 to 1. 

	cdDEV_RANGE_2:
		Contrast, colorDepth, enhance (and hue) can be set the value from -2 to 2. 

	cdDEV_RANGE_WB_1
		cdWB_FLUORESCENT_H can be set in CDSetDevParam_WB().

  	cdDEV_RANGE_WB_2
		cdWB_KELVIN can be set in CDSetDevParam_WB().

  	cdDEV_RANGE_WB_3
		cdWB_SHADE can be set in CDSetDevParam_WB().

	cdDEV_RANGE_PE_1
		CDSetDevParam_PhotoEffect() can be done to specify the photo effect parameter.

	cdDEV_FORMAT_16_BIT
		The image data of 16 bit format can be output as a development result. 

  	cdDEV_ADOBE_RGB	
		cdCOLOR_SPACE_ADOBE_RGB can be set in CDSetDevParam_ColorSpace().

	cdDEV_FALSE_COLOR_FILTER
		CDSetDevParam_FalseColorFilter() can be done not to execute the False Color Filter
		for RAW of Type A or Type C.

	cdDEV_FALSE_RESO_FILTER
		CDSetDevParam_FalseResolutionFilter() can be done to execute the False Resolution Filter
		for RAW of Type B.
	*/

/* cdRAWDevType
	Type of the RAW image data. */
typedef cdUInt32 cdRAWDevType;
	#define cdDEV_TYPE_A			0x00000010   /* Type A */
	#define cdDEV_TYPE_B			0x00000020   /* Type B */
	#define cdDEV_TYPE_C			0x00000040   /* Type C */

/* cdDevContrast
	Contrast setting */
typedef cdInt16   cdDevContrast;
	#define cdCONTRAST_WEAK_2		(-2)
	#define cdCONTRAST_WEAK			(-1)
	#define cdCONTRAST_STANDARD		(0)
	#define cdCONTRAST_STRONG		(1)
	#define cdCONTRAST_STRONG_2		(2)
	#define cdCONTRAST_ATCAPTURE	(0x00ff)

/* cdDevColorDepth
	Color depth setting */
typedef cdInt16   cdDevColorDepth;
	#define cdCOLOR_DEPTH_LIGHT_2	(-2)
	#define cdCOLOR_DEPTH_LIGHT		(-1)
	#define cdCOLOR_DEPTH_STANDARD	(0)
	#define cdCOLOR_DEPTH_DEEP		(1)	
	#define cdCOLOR_DEPTH_DEEP_2	(2)
	#define cdCOLOR_DEPTH_ATCAPTURE	(0x00ff)

/* cdDevEnhance
	Enhance setting  */
typedef cdInt16   cdDevEnhance;
	#define cdENHANCE_WEAK_2		(-2)
	#define cdENHANCE_WEAK			(-1)
	#define cdENHANCE_STANDARD		(0)
	#define cdENHANCE_STRONG		(1)
	#define cdENHANCE_STRONG_2		(2)
	#define cdENHANCE_OFF			(3)  
	#define cdENHANCE_ATCAPTURE		(0x00ff)

/* cdDevPhotoEffect
	Photo effect setting  */
typedef cdPhotoEffect	cdDevPhotoEffect;
	#define	cdDEV_PHOTO_EFFECT_ATCAPTURE	(0x00ff)

/* cdDevHue
   Hue / Color tone setting */
typedef cdInt16   cdDevHue;
	#define cdHUE_M_2				(-2)
	#define cdHUE_M_1				(-1)
	#define cdHUE_STANDARD			(0)
	#define cdHUE_P_1				(1)
	#define cdHUE_P_2				(2)
	#define cdHUE_ATCAPTURE			(0x00ff)

		#define cdHUE_1				cdHUE_M_1
		#define cdHUE_2				cdHUE_P_1

/* cdDevEdgeLevel
    Sharpness - Level  */
typedef cdInt16   cdDevEdgeLevel;
	#define cdEDGE_LEVEL_ATCAPTURE	0x00ff
	#define cdEDGE_LEVEL_0			(0)	/* off */
	#define cdEDGE_LEVEL_1			(1)	/* very weak  */
	#define cdEDGE_LEVEL_2			(2) /* weak */
	#define cdEDGE_LEVEL_3			(3) /* standard */
	#define cdEDGE_LEVEL_4			(4) /* strong */
	#define cdEDGE_LEVEL_5			(5) /* very strong */

/* cdDevEdgeFreq
    Sharpness - Pattern Type to be sharpened */
typedef cdInt16   cdDevEdgeFreq;
	#define cdEDGE_FREQ_ATCAPTURE	0x00ff
	#define cdEDGE_FREQ_0			(0) /* reserved..invalud value */
	#define cdEDGE_FREQ_1			(1) /* rough */
	#define cdEDGE_FREQ_2			(2) /* moderately rough */
	#define cdEDGE_FREQ_3			(3) /* standard */
	#define cdEDGE_FREQ_4			(4) /* moderately fine */
	#define cdEDGE_FREQ_5			(5) /* fine */


/* cdDevGammaSrc
	Gamma settings */
typedef cdInt16   cdDevGammaSrc;
	#define cdGAMMA_SRC_ATCAPTURE		0x00ff
	#define cdGAMMA_SRC_MODIFIED_PARAM	0x0000
	#define cdGAMMA_SRC_SPECIFIED_PARAM	0x0001


/* cdDevColorMatrix
	Color matrix  */
typedef cdColorMatrix   cdDevColorMatrix;


/* cdDevColorSpace
	Color space  */
typedef cdUInt16	  cdDevColorSpace;
	#define cdCOLOR_SPACE_ATCAPTURE		0x00ff
	#define cdCOLOR_SPACE_S_RGB			(1)	/* sRGB	        */
	#define cdCOLOR_SPACE_ADOBE_RGB		(2)	/* Adobe RGB    */


/****************************************************************************
 ****************************************************************************
 *                 Canon Digital Camera SDK : Structures                    *
 ****************************************************************************
 ****************************************************************************/

/*-----------------------------------------------------------------------
   Version 
------------------------------------------------------------------------*/
/* cdVersionInfo*/
typedef struct {
	cdUInt16		Size;			/* Size of this structure */
  	cdUInt16		MajorVersion;	/* Major version number. */
	cdUInt16		MinorVersion;	/* Minor version number. */
	cdUInt16		ReleaseVersion;	/* Release versionn number. 0 means unknown.*/
	cdChar			chVersion[32];	/* String of version number.  e.x "1.0.0.1" */
} cdVersionInfo;


/*-----------------------------------------------------------------------
   Data storage and transfer  
------------------------------------------------------------------------*/
/*---------------------------------------------------------- 
  Stream 
*/

/* cdWhence
	From whence to seek in stream. */
typedef cdUInt16 cdWhence;		
	#define cdSTART						1
	#define cdCURRENT					2
	#define cdEND						3

/* cdParmission
    Access permission with which to open stream. */
typedef cdUInt16 cdPermission;
	#define cdREAD   					1
	#define cdWRITE  					2
	#define cdUPDATE 					4

typedef void    cdSTDCALL cdSOpen(cdContext contextH, cdPermission, cdError* err);
typedef void    cdSTDCALL cdSClose(cdContext contextH, cdError* err); 
typedef void    cdSTDCALL cdSRead(cdContext contextH, void* buf, cdUInt32* bufsize, cdError* err); 
typedef void    cdSTDCALL cdSWrite(cdContext contextH, const void *buf, cdUInt32* bufsize, cdError *err);
typedef void    cdSTDCALL cdSSeek(cdContext contextH, cdWhence, cdInt32 offset, cdError* err);
typedef cdInt32 cdSTDCALL cdSTell(cdContext contextH, cdError* err);
typedef void    cdSTDCALL cdSProgress(cdContext contextH, cdUInt16 percentDone, cdError* err);

/* cdStream
*/
typedef struct {
	cdContext contextH;
	/* stream I/O function pointers */ 
	cdSOpen*		open;
	cdSClose*		close;
	cdSRead*		read;
	cdSWrite*		write;
	cdSSeek*		seek;
	cdSTell*		tell;
} cdStream;

/*---------------------------------------------------------- 
    StgMedium
*/

/* cdMemType
	Memory type of cdStgMedium 
*/
typedef cdUInt32 cdMemType;
	#define cdMEMTYPE_FILE				(0)		/* File. Only for Win.  */
	#define cdMEMTYPE_STREAM			(1)		/* Stream */
	#ifdef macintosh
	#define cdMEMTYPE_FSSPEC			(3)		/* FSSpec. Only for Mac. */
	#endif

/* cdStgMedium
*/
typedef struct {
  	cdMemType		Type;	/* Type of the medium (u). */
  	union {
		cdChar*		lpszFileName;	
		cdStream*	pStream;
		#ifdef	macintosh
		cdFSSpec*	pFSSpec;
		#endif
  	}u;						/* Union of all transfer medium */
} cdStgMedium;


/*-----------------------------------------------------------------------
   Basic structure 
------------------------------------------------------------------------*/

/* cdSize */
typedef struct{
	cdUInt32 		Width;
	cdUInt32		Height;	
} cdSize;

/* cdPosition */
typedef struct{
	cdInt32			X;
	cdInt32			Y;
} cdPosition;

/* cdRect */
typedef struct{
	cdUInt32 		Left;
	cdUInt32		Right;
	cdUInt32		Top;
	cdUInt32		Bottom;	
} cdRect;

/* cdImageFormatStruct
   Image format and size. */
typedef struct{
	cdImageFormat	Format;			/* Format */
	cdSize			ImageSize;		/* Valid if Format is 
								      cdFORMAT_PIXMAP or cdFORMAT_PIXMAP_16 */
} cdImageFormatStruct;

/* cdImageSizeSpec */
typedef struct{
	cdImageSize		ImageSize;		/* Size                    */
	cdSize			Size;			/* Image width and height. */
	cdChar			SizeName[64];	/* Nickname                */
}cdImageSizeSpec;


/*-----------------------------------------------------------
    Port descriptions
*/

#ifdef macintosh
/* Macintosh */

/* cdPortDescripUSB
   USB I/O port descriptor*/
typedef struct {
	cdUInt32		ModelID;
	cdChar			szLaunchedDeviceName[76];
} cdPortDescripUSB;

/* cdPortDescripIEEE1394
   IEEE1394 I/O port descriptor*/
typedef struct {
	cdUInt32		ModelID;
	cdChar			szLaunchedDeviceName[76];
} cdPortDescripIEEE1394;

#else

/* Windows */

/* cdPortDescripSTIPort
   Port descriptor for device using STI interface */
typedef struct {
	cdDevDataType	DataType;			
	cdWChar			DeviceInternalName[128];
}cdPortDescripSTI;

/* cdPortDescripWIA
   Port descriptor for device using WIA interface */
typedef struct {
	cdDevDataType	DataType;
	cdWChar*			pDIPDevID;
}cdPortDescripWIA;

#endif

/* cdPortDescripRsrvd
   Reserved port descriptor*/
typedef struct {
	cdUInt32		ModelID;
	cdChar			szLaunchedDeviceName[76];
} cdPortDescripRsrvd;

/* cdPortDescripUnion
   I/O port descriptor union - contains all I/O port descriptors*/
typedef union {
	#ifdef macintosh
	cdPortDescripUSB			USB;
	cdPortDescripIEEE1394	IEEE1394;
	#else
	cdPortDescripSTI		STI;
	cdPortDescripWIA		WIA;
	#endif
	cdPortDescripRsrvd		rsrvd;      
} cdPortDescripUnion;

/*----------------------------------------------*/

/* cdSourceInfo
    Source device information */
typedef struct{
	cdSourceType		SurceType;				/* Type of the device                   */
	cdUInt32			rsrvd;					/* Reserved		                        */
	cdStr63				Name;					/* Device model name 
												   (invalid if cdSRC_TYPE_HOST)         */
	#ifndef macintosh
	cdStr63				NameInOS;				/* Device model name in OS				*/
	#endif
	cdPortType			PortType;				/* Port type                            */
	cdPortDescripUnion	u;						/* Union of I/O port descriptor         */
} cdSourceInfo;

/* cdVolumeInfo */
typedef struct{
	cdVolHWType			HWtype;					/* Type of device that the volume is    */
	cdBoolean			isRemoveable;			/* Whether the volume is removeable     */
	cdUInt64			TotalSpace;				/* Total volume space in Kilobytes      */
	cdUInt64			FreeSpace;				/* Total free volume space in Kilobytes */
	cdChar				Name[cdMAX_VOL];		/* Volume name                          */
} cdVolumeInfo;

/* cdItemInfo */
typedef struct {
	cdItemType			Type;					/* Type of item                         */
	cdAttribute			Attributes;				/* Attributes of item                   */
	cdTime				TimeStamp;				/* Time                                 */
	cdChar				Name[cdMAX_PATH];		/* Name of item                         */
	cdUInt32			Size;					/* Data size of main image file in item */
	cdUInt32			numThumbnail;			/* Number of thumbnail data	            */
	cdUInt32			numPicture;				/* Number of picture data	            */
	cdUInt32			numMovie;				/* Number of movie data	                */
	cdUInt32			numSound;				/* Number of sound data	                */
} cdItemInfo;

/* cdImageDataInfo */
typedef struct {
	cdDataType			DataType;				/* Type of data                         */
	cdAttribute			Attributes;				/* Attributes of data                   */
	cdUInt32			DataSize;				/* Size of data in Kilobytes            */
	cdTime				TimeStamp;				/* Time                                 */
	cdChar				FileName[cdMAX_PATH];	/* File name                            */
} cdImageDataInfo;

/* cdReleaseImageInfo */
typedef struct {
	cdUInt32			SequenceID;				/* Capture ID					        */
	cdDataType			DataType;				/* Shows the type of image data         */
	cdUInt8				Format;					/* The format of the image data         */
	cdUInt32			DataSize;				/* The size of the image data.          */
	cdChar				FileName[cdMAX_TRANSFER_PATH_LENGTHT];	/* Not used             */
} cdReleaseImageInfo, *pcdReleaseImageInfo;

/*----------------------------------------------*/

/* cdDevicePropertyStruct
    Device property information */
typedef struct{
	cdDevicePropertyID		DevPropID;		/* Device property ID       */
	cdAttribute				Access;			/* Property data attributes */
	cdUInt32				DataSize;		/* Property data size       */
	cdUInt32				rsrvd;			/* Reserved                 */
} cdDevicePropertyStruct;

/* cdRelCamSettingStruct
    Camera setting information in remote release control mode */
typedef struct{
	cdRelCamSettingID		SettingID;		/* Camera setting ID		*/
	cdAttribute				Access;			/* Attribute				*/
	cdUInt32				rsrvd;			/* Reserved					*/
} cdRelCamSettingStruct;

/* cdImagePropertyStruct */
typedef struct{
	cdImagePropertyID		PicPropID;		/* Image property ID        */
	cdAttribute				Access;			/* Property data attributes */
	cdUInt32				DataSize;		/* Property data size       */
	cdUInt32				rsrvd;			/* Reserved                 */
} cdImagePropertyStruct;

/* cdBaseImagePropertyStruct */
typedef struct{
	cdBaseImagePropertyID	BaseImgPropID;	/* Base image property ID   */
	cdUInt32				DataSize;		/* Data size                */
	cdUInt32				rsrvd;			/* Reserved                 */
} cdBaseImagePropertyStruct;

/* cdIIMInfoStruct */
typedef struct{
	cdUInt8					RecordNo;		/* Record number            */
	cdUInt8					DataSetNo;		/* Data set number          */
	cdAttribute				Access;			/* Attributes               */
	cdUInt32				DataSize;		/* Data size                */
} cdIIMInfoStruct;

/*----------------------------------------------*/

/* cdAddPictureProps */
typedef struct {
	cdUInt32	Size;				/* Size of this structure             */
	cdChar		FilePrefix[4];		/* Prefix for the generated file name */                            
	cdInt16		RotationAngle;		/* Rotation angle to be set in the 
									   image being added.                 */
	cdChar		*pOwnerName;		/* Owner name to be set in the image 
									   being added. */
} cdAddPictureProps, *pcdAddPictureProps;

/* cdAddPictureInfo */
typedef struct {
	cdChar		Path[256];			/* Path	        */
	cdChar		FileName[256];		/* File name    */
	cdUInt32 	FileNumber;			/* File number  */
} cdAddPictureInfo, *pcdAddPictureInfo;


/*-----------------------------------------------------------
    Personal function and custom functions 
*/

/* cdPersonalFunction
    Status of personal function setting. */
typedef struct {
	cdUInt16	Length;
	cdUInt16	PFn_0;			
	cdUInt16	PFn_1;			
	cdUInt16	PFn_2;			
	cdUInt16	PFn_3;			
	cdUInt16	PFn_4;			
	cdUInt16	PFn_5;			
	cdUInt16	PFn_6;			
	cdUInt16	PFn_7;			
	cdUInt16	PFn_8;			
	cdUInt16	PFn_9;			
	cdUInt16	PFn_10;			
	cdUInt16	PFn_11;			
	cdUInt16	PFn_12;			
	cdUInt16	PFn_13;			
	cdUInt16	PFn_14;			
	cdUInt16	PFn_15;			
	cdUInt16	PFn_16;			
	cdUInt16	PFn_17;			
	cdUInt16	PFn_18;			
	cdUInt16	PFn_19;			
	cdUInt16	PFn_20;			
	cdUInt16	PFn_21;			
	cdUInt16	PFn_22;			
	cdUInt16	PFn_23;			
	cdUInt16	PFn_24;			
	cdUInt16	PFn_25;			
	cdUInt16	PFn_26;			
	cdUInt16	PFn_27;			
	cdUInt16	PFn_28;			
	cdUInt16	PFn_29;			
	cdUInt16	PFn_30;			
} cdPersonalFunction;

/* cdPersonalFunctionValue
    Value of personal function setting */
typedef struct {
	cdUInt16	Length;		
	cdUInt16	PFn_1_Value;
	cdUInt16	PFn_2_Value;
	cdUInt16	PFn_3_Value;
	cdUInt16	PFn_4_Value_High;
	cdUInt16	PFn_4_Value_Low;
	cdUInt16	PFn_5_Value_High;
	cdUInt16	PFn_5_Value_Low;
	cdUInt16	PFn_8_Value;
	cdUInt16	PFn_19_Value_High;
	cdUInt16	PFn_19_Value_Low;
	cdUInt16	PFn_20_Value;
	cdUInt16	PFn_23_Value_Timer6;
	cdUInt16	PFn_23_Value_Timer16;
	cdUInt16	PFn_23_Value_Release;
	cdUInt16	PFn_25_Value_AEmode;
	cdUInt16	PFn_25_Value_Metering;
	cdUInt16	PFn_25_Value_Shutter;
	cdUInt16	PFn_25_Value_AFmode;
	cdUInt16	PFn_25_Value_AFframe;
	cdUInt16	PFn_25_Value_Quality;
	cdUInt16	PFn_25_Value_WBmode;
	cdUInt16	PFn_25_Value_Develop;
	cdUInt16	PFn_25_Value_Matrix;
	cdUInt16	PFn_27_Value;
} cdPersonalFunctionValue;

/* cdCustomFunction */
typedef struct{
	cdUInt16	length;
	cdUInt16	CFn_1;
	cdUInt16	CFn_2;
	cdUInt16	CFn_3;
	cdUInt16	CFn_4;
	cdUInt16	CFn_5;
	cdUInt16	CFn_6;
	cdUInt16	CFn_7;
	cdUInt16	CFn_8;
	cdUInt16	CFn_9;
	cdUInt16	CFn_10;
	cdUInt16	CFn_11;
	cdUInt16	CFn_12;
	cdUInt16	CFn_13;
	cdUInt16	CFn_14;
	cdUInt16	CFn_15; 
}cdCustomFunction;

/* cdCustomFunction2 */
typedef struct{
	cdUInt16	length;
	cdUInt16	CFn_0;
	cdUInt16	CFn_1;
	cdUInt16	CFn_2;
	cdUInt16	CFn_3;
	cdUInt16	CFn_4;
	cdUInt16	CFn_5;
	cdUInt16	CFn_6;
	cdUInt16	CFn_7;
	cdUInt16	CFn_8;
	cdUInt16	CFn_9;
	cdUInt16	CFn_10;
	cdUInt16	CFn_11;
	cdUInt16	CFn_12;
	cdUInt16	CFn_13;
	cdUInt16	CFn_14;
	cdUInt16	CFn_15;
	cdUInt16	CFn_16;
	cdUInt16	CFn_17;
	cdUInt16	CFn_18;
	cdUInt16	CFn_19;
	cdUInt16	CFn_20;
	cdUInt16	CFn_21;			// K174 add new function
}cdCustomFunction2;


/*-----------------------------------------------------------
    Stitch assist 
*/

/* cdStitchDesc */
typedef struct{
    cdUInt16	NumImages; 
      /* high byte : number of images in H direction
         low byte : number of images in V direction
         Number of images in Stitch shooting mode
         0 indicates it is unknown how many images are exists in the direction        
	  */
    cdInt16		Position;   
      /* higher 8 bit signed integer:  position in H direction
         lower 8 bit signed integer : position in V direction
         Position indicates the image's position in the set of stitch images
	  */
    cdUInt16	Order;
      /* This field represents the shooting order. This number starts from 0.
	     There will be a missing 
         number if the user reshoot a picture again.
	  */
    cdUInt16	OverlapH;

    cdUInt16	OverLapV;

    cdUInt16	Configuration;
}cdStitchDesc;

/* Note: Cordinates: Increases in left to right in horizontal direction
         Increases in Top to bottom direction in vertical direction.
*/
/* Followings are constants related to Configuration member.
   configuration member bit assignment
*/
#define cdSTITCH_CONFIG_MASK_ORIENTATION	    (0xC000) 
#define cdSTITCH_CONFIG_MASK_SHOOTING_DIRECTION	(0x000F) 

 /* define orientation */
#define cdORIENTATION_PORTRAIT     (0x0000)
#define cdORIENTATION_LANDSCAPE    (0x4000)

 /* define shooting direction : panning direction in shooting */
#define cdSHOOTING_DIRECTION_HORIZONTAL_POSITIVE  (0x0000) /* horizontal positive */
#define cdSHOOTING_DIRECTION_HORIZONTAL_NEGATIVE  (0x0001) /* horizontal negative */
#define cdSHOOTING_DIRECTION_VERTICAL_POSITIVE    (0x0002) /* vertical positive */
#define cdSHOOTING_DIRECTION_VERTICAL_NEGATIVE    (0x0003) /* vertical negative */
#define cdSHOOTING_DIRECTION_TWO_DIMENSION        (0x0004) /* 2 dimension */

  /* macros */ 
#define cdSTITCH_IMAGE_ORIENTATION(d)  (cdSTITCH_CONFIG_MASK_ORIENTATION & d )
#define cdSTITCH_SHOOTING_DIRECTION(d)  (cdSTITCH_CONFIG_MASK_SHOOTING_DIRECTION & d )

#define cdSTITCH_NUM_IMAGE_H(d)     ( (cdUInt16)d >> 8)
#define cdSTITCH_NUM_IMAGE_V(d)     ( (cdUInt16)d & 0x00ff )
#define cdSTITCH_POSITION_H(d)     ( (cdChar) ( (psUInt16)d >> 8 )  ) 
#define cdSTITCH_POSITION_V(d)     ( (cdChar) ( (psUInt16)d & 0x00ff ) )


/*-----------------------------------------------------------
    Gamma parameters 
*/

/* cdDevPoint */
typedef struct {
	cdUInt16		x;	
	cdUInt16		y;
} cdDevPoint;

/* cdDevLUTPoint
   The four points used to express a single gamma curve. */
typedef struct {
	cdDevPoint		p0;	/* Specifies Point 0 of the gamma curve.  */
	cdDevPoint		p1;	/* Specifies Point 1 of the gamma curve.  */
	cdDevPoint		p2;	/* Specifies Point 2 of the gamma curve.  */
	cdDevPoint		p3;	/* Specifies Point 3 of the gamma curve.  */
} cdDevLUTPoint;

/* cdDevGammaLUT
   The gamma curve for the R, G, and B data respectively. */
typedef struct {
	cdDevLUTPoint	r;	/* Gamma curve for the Red data.  */
	cdDevLUTPoint	g;	/* Gamma curve for the Green data.*/
	cdDevLUTPoint	b;	/* Gamma curve for the Blue data. */
} cdDevGammaLUT;

/* cdDevGammaLUTTable */
typedef struct {
	cdUInt16		Num;
	cdDevGammaLUT	GammaLUT[cdANY];
} cdDevGammaLUTTable;

/* cdDevGammaLUTMatching
    Gamma table. */
typedef struct {
	cdDevGammaLUT	Right;	/* The gamma curve for the R, G, and B data */
	cdDevGammaLUT	Left;	/* Reserved. */
} cdDevGammaLUTMatching;

/*-----------------------------------------------------------
    Edge parameters
*/

/* cdEdgeLevel */
typedef struct {
	cdUInt16	xyz_hi;
	cdUInt16	xyz_lo;
	cdUInt16	level;
} cdEdgeLevel;

/* cdEdgeLevelTable */
typedef struct {
	cdEdgeLevel	param[5];
} cdDevEdgeLevelTable; 

/* cdEdgeFreq */
typedef struct {
	cdUInt16	filter1_hi;
	cdUInt16	filter1_lo;
	cdUInt16	filter2_hi;
	cdUInt16	filter2_lo;
	cdUInt16	x;
	cdUInt16	y;
} cdEdgeFreq;

/* cdDevEdgeFreqTable */
typedef struct {
	cdEdgeFreq	param[5];
} cdDevEdgeFreqTable; 

/*-----------------------------------------------------------
    White balance parameters
*/

/* cdWhiteBalance */
typedef struct {
	cdUInt16	r;
	cdUInt16	g;
	cdUInt16	b;
} cdWhiteBalance;

/* cdColorCollect */
typedef struct {
	cdUInt16	mat00;
	cdUInt16	mat01;
	cdUInt16	mat02;
	cdUInt16	mat10;
	cdUInt16	mat11;
	cdUInt16	mat12;
	cdUInt16	mat20;
	cdUInt16	mat21;
	cdUInt16	mat22;
} cdColorCollect;

/* cdWBParam */
typedef struct {
	cdWhiteBalance	wb1;
	cdColorCollect	cc1;
	cdWhiteBalance	wb2;
	cdColorCollect	cc2;
} cdWBParam;

/* cdDevWBParamTable */
typedef struct {
	cdWBParam	Auto;
	cdWBParam	Daylight;
	cdWBParam	Cloudy;
	cdWBParam	Tungsten;
	cdWBParam	Fluorescent;
	cdWBParam	Flash;
	cdWBParam	Manual;
	cdWBParam	Monochrome;
	cdWBParam	Shade;
	cdWBParam	Kelvin;
	cdWBParam	PCset1;
	cdWBParam	PCset2;
	cdWBParam	PCset3;
	cdWBParam	T2000K;
	cdWBParam	T3000K;
	cdWBParam	T4000K;
	cdWBParam	T5000K;
	cdWBParam	T6000K;
	cdWBParam	T7000K;
	cdWBParam	T8000K;
	cdWBParam	T9000K;
	cdWBParam	T10000K;
	cdWBParam	T11000K;
	cdWBParam	T12000K;
} cdDevWBParamTable;

/* cdDevWBParamMatching */
typedef struct {
	cdWBParam	Modified;
	cdInt16		OffsetR;
	cdInt16		OffsetG;
	cdInt16		OffsetB;
	cdUInt16	OffsetCenter;
} cdDevWBParamMatching;


typedef struct {
	cdUInt16	r;
	cdUInt16	g;
	cdUInt16	b;
} cdDevWhiteBalance;

typedef struct {
	cdUInt16	mat00;
	cdUInt16	mat01;
	cdUInt16	mat02;
	cdUInt16	mat10;
	cdUInt16	mat11;
	cdUInt16	mat12;
	cdUInt16	mat20;
	cdUInt16	mat21;
	cdUInt16	mat22;
} cdDevColorCollect;

typedef struct {
	cdDevWhiteBalance	wb1;
	cdDevColorCollect	cc1;
	cdDevWhiteBalance	wb2;
	cdDevColorCollect	cc2;
} cdDevWBParam;


/* cdDevParamWBCoefficient */
typedef struct {
	cdUInt32	Type;			/* RAW data type.                        */
	cdUInt32	Size;			/* Size of the white balance coefficient */
	cdUInt8		Data[cdANY];	/* White balance coefficient             */
} cdDevParamWBCoefficient, *pcdDevParamWBCoefficient;


/************* VALID FOR RCSDK8.0 ************/

/* cdUserColorMatrix */
typedef struct {
	cdUInt32	Index;
	cdUInt32	ColorSpace;
	cdInt32		ColorDepth;
	cdInt32		ColorTone;
} cdUserColorMatrix;

/* cdJpegQuality */
typedef struct {
	cdUInt16	LargeJpegQuality;
	cdUInt16	Medium1JpegQuality;
	cdUInt16	Medium2JpegQuality;
	cdUInt16	SmallJpegQuality;
} cdJpegQuality;

/* cdWhiteBalanceShift */
typedef struct {
	cdUInt32	ABLevel;
	cdUInt32	MGLevel;
} cdWhiteBalanceShift;


/****************************************************************************
 ****************************************************************************
 *            Canon Digital Camera SDK : Callback Functions                 *
 ****************************************************************************
 ****************************************************************************/

/* cdEventCallbackFunction
    Called when a camera event occurs. 
*/
typedef cdUInt32 cdSTDCALL cdEventCallbackFunction (
	cdEventID			EventID,	/* Event ID                            */
	const void*			pData,		/* Pointer to the event data           */
	cdUInt32 			DataSize,	/* Size of the event data              */
	cdContext			Context		/* Context handle                      */
);


/* cdReleaseEventCallbackFunction
    Called when a remote release control related event occurs. 
*/
typedef cdUInt32 cdSTDCALL cdReleaseEventCallbackFunction (
	cdReleaseEventID	EventID,	/* Event ID                            */
	const void *		pData, 		/* Pointer to the event data           */
	cdUInt32 			DataSize,	/* Size of the event data              */
	cdContext			Context		/* Context handle                      */
);

/* cdProgressCallbackFunction
    Called at regular intervals during processing to notify 
	the client application of the progress of various processes. 
*/
typedef cdUInt32 cdSTDCALL cdProgressCallbackFunction (
	cdUInt32			Progress,	/* Percentage                            */
	cdProgressStatus	Status,		/* Type of process currently in progress */
	cdContext			Context		/* Context                               */
);

/*cdViewfinderCallbackFunction
   Callback function for retrieving Viewfinder data. 
*/
typedef cdUInt32 cdSTDCALL cdViewfinderCallbackFunction (
	cdVoid*				pBuf,		/* Pointer to the buffer that contains 
									   one frame of Viewfinder data.       */
 	cdUInt32 			Size,		/* Buffer size indicated by pBuf       */
	cdUInt32 			Format,		/* Viewfinder data format              */
 	cdContext 			Context		/* Context                             */
);


/*---------------------------------------------------------------------------*/



#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=reset
 #endif
#endif 


#ifdef _MSC_VER
#	if	900 <= _MSC_VER
	//#	pragma message( "pop, _CD_TYPE_H_PACK_" )
	#	pragma warning( default : 4250 4355 4244 4005) 
	#	pragma pack( pop, _CD_TYPE_H_PACK_ )
#	endif	/* 900 <= _MSC_VER */
#elif defined __WATCOMC__
	#pragma pack(__pop);
#endif


#endif