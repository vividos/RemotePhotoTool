	#ifndef _CD_FNC_TYPE_H_
#define _CD_FNC_TYPE_H_

#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=mac68k
 #endif
#endif


/****************************************************************************
 ****************************************************************************
 *              Canon Digital Camera SDK : Function Pointers                *
 *                            For Version 7.1                               *
 ****************************************************************************
 ****************************************************************************/

/*-----------------------------------------------------------------------
	Function Pointers
------------------------------------------------------------------------*/

/* Basic Functions */

typedef cdError cdSTDCALL cdStartSDK( 
	cdVersionInfo*	pVersion,
	cdUInt32		Option
);

typedef cdError cdSTDCALL cdFinishSDK();

typedef cdError cdSTDCALL cdGetSDKVersion(
	cdVersionInfo *	pVersion
);


/* Utility Functions */

typedef cdError cdSTDCALL cdCreateMemStream(
	cdUInt32	StartSize, 
	cdUInt32	MinAllocSize,
	cdStream*	pStream
);

typedef cdError  cdSTDCALL cdDestroyMemStream(
	cdStream*	pStream
);

typedef cdError cdSTDCALL cdGetStreamInfo(
	cdStream*	pStream,
	cdUInt32*	pSize,
	cdVoid**	ppMem
);

typedef cdError cdSTDCALL cdGetImagePropertyPart(
	cdHImageItem				hImgItem,
	cdStgMedium*				pStgMedium
);

/* Source Control Functions */

typedef cdError cdSTDCALL cdEnumDeviceReset(
	cdUInt32		Kind,
	cdHEnum*		phEnum	
);

typedef cdError cdSTDCALL cdEnumDeviceNext(
	cdHEnum			hEnum,	
	cdSourceInfo* 	pSourceInfo
);

typedef cdError cdSTDCALL cdGetDeviceCount (
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdEnumDeviceRelease (
	cdHEnum			hEnum
);

typedef cdError cdSTDCALL cdOpenSource( 
	cdSourceInfo*	pSourceInfo,
	cdHSource*		phSource
);

typedef cdError cdSTDCALL cdCloseSource(
	cdHSource		hSource
);

typedef cdError cdSTDCALL cdRegisterEventCallbackFunction(
	cdHSource					hSource,
	cdEventCallbackFunction*	pFunc,
	cdContext					Context,
	cdHandle*					phFunc
);

typedef cdError cdSTDCALL cdUnregisterEventCallbackFunction(
	cdHSource					hSource,
	cdHandle					hFunc
);


/* Device Management Functions */

typedef cdError cdSTDCALL cdEnumDevicePropertyReset (
	cdHSource				hSource,
	cdUInt32				Option,
	cdHEnum*				phEnum
);

typedef cdError cdSTDCALL cdEnumDevicePropertyNext (
	cdHEnum					hEnum,
	cdDevicePropertyStruct*	pDevPropStruct
);

typedef cdError cdSTDCALL cdGetDevicePropertyCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount
);

typedef cdError cdSTDCALL cdEnumDevicePropertyRelease (
	cdHEnum					hEnum
);

typedef cdError cdSTDCALL cdGetDevicePropertyData(
	cdHSource				hSource,
	cdDevicePropertyID 		DevPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf,
	cdUInt32				Option
);

typedef cdError cdSTDCALL cdSetDevicePropertyData(
	cdHSource				hSource,
	cdDevicePropertyID  	DevPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf
);

typedef cdError cdSTDCALL cdEnumSupportedImageSizeReset(
	cdHSource				hSource,
	cdUInt32				Option,
	cdHEnum*				phEnum
);

typedef cdError cdSTDCALL cdEnumSupportedImageSizeNext(
	cdHEnum					hEnum,
	cdImageSizeSpec*		pSpec
);

typedef cdError cdSTDCALL cdGetSupportedImageSizeCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount
);

typedef cdError cdSTDCALL cdEnumSupportedImageSizeRelease (
	cdHEnum					hEnum
);

typedef cdError cdSTDCALL cdLockUI(
	cdHSource	hSource
);

typedef cdError cdSTDCALL cdUnlockUI(
	cdHSource	hSource
);

typedef cdError cdSTDCALL cdSetUILockTimeOutTime(
	cdHSource	hSource,
	cdUInt32	Time
);

typedef cdError cdSTDCALL cdFormat(
	cdHVolume	hVolume
);


/* Collect Functions */
typedef cdError cdSTDCALL cdEnumVolumeReset(		
	cdHSource			hSource,
	cdHEnum*			phEnum
);

typedef cdError cdSTDCALL cdEnumVolumeNext(
	cdHEnum				hEnum,
	cdHVolume *			phVolume
);

typedef cdError cdSTDCALL cdGetVolumeCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount		
);

typedef cdError cdSTDCALL cdEnumVolumeRelease(
	cdHEnum				hEnum
);

typedef cdError cdSTDCALL cdGetVolumeName(
	cdHVolume			hVolume,
	cdVolName*			pVolName
);

typedef cdError cdSTDCALL cdGetVolumeInfo(
	cdHVolume			hVolume,
	cdVolumeInfo*		pVolInfo
);

typedef cdError cdSTDCALL cdEnumItemReset(
	cdHItem				hParent,
	cdEnumItemOption	Option,
	cdHEnum*			phEnum
);

typedef cdError cdSTDCALL cdEnumItemNext(
	cdHEnum				hEnum,
	cdHItem*			phItem
);

typedef cdError cdSTDCALL cdGetItemCount (
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

typedef cdError cdSTDCALL cdEnumItemRelease (
	cdHEnum				hEnum
);

typedef cdError cdSTDCALL cdEnumImageItemReset(
	cdHItem				hItem,
	cdUInt32			Depth,
	cdEnumItemOption	Option,
	cdHEnum*			phEnum
);

typedef cdError cdSTDCALL cdEnumImageItemNext( 
	cdHEnum				hEnum,
	cdHImageItem*		phImage
);

typedef cdError cdSTDCALL cdGetImageItemCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

typedef cdError cdSTDCALL cdEnumImageItemRelease (
	cdHEnum				hEnum
);

typedef cdError cdSTDCALL cdGetItemInfo(
	cdHItem				hItem,
	cdItemInfo*			pItemInfo
);

typedef cdError cdSTDCALL cdMakeImageItem(
	cdStgMedium			StgMedium,
	cdImageFormat		Format,
	cdHImageItem*		phImgItem
);

typedef cdError cdSTDCALL cdFreeImageItem(
	cdHImageItem	hImgItem
);

/* Image Management Functions */ 

typedef cdError cdSTDCALL cdOpenImage(
	cdHImageItem	hImgItem
);

typedef cdError cdSTDCALL cdCloseImage(
	cdHImageItem	hImgItem
);

typedef cdError cdSTDCALL cdGetThumbnail(
	cdHImageItem	hImgItem,
	cdHImageData*	phImgData
);

typedef cdError cdSTDCALL cdGetPicture(
	cdHImageItem	hImgItem,
	cdHImageData*	phImgData
);

typedef cdError cdSTDCALL cdGetMovie (
	cdHImageItem	hImgItem,
	cdHImageData*	phImgData
);

typedef cdError cdSTDCALL cdGetSound (
	cdHImageItem	hImgItem,
	cdHImageData*	phImgData
);

typedef cdError cdSTDCALL cdEnumImageDataInItemReset(
	cdHImageItem	hImgItem,
	cdHEnum*		phEnum
);

typedef cdError cdSTDCALL cdEnumImageDataInItemNext(
	cdHEnum			hEnum,
	cdHImageData*	phImgData
);

typedef cdError cdSTDCALL cdGetImageDataInItemCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdEnumImageDataInItemRelease (
	cdHEnum			hEnum
);

typedef cdError cdSTDCALL cdGetImageDataInfo(
	cdHImageData		hImgData,
	cdImageDataInfo*	pImgDataInfo
);

typedef cdError cdSTDCALL cdGetImageData(
	cdHImageData 				hImgData,
	cdStgMedium*				pStgMediumDest,
	cdProgressCallbackFunction*	pCallbackFunc,
	cdContext					Context,
	cdProgressOption			ProgressOption
);


typedef cdError cdSTDCALL cdAddPicture(
	cdStgMedium*				pSrc, 
	cdImageFormatStruct*		pSrcFileFormat,

	cdHVolume					hVolume,
	cdHFolder					hFolder,

	cdCompQuality				CompQuality,

	cdChar*						pModelName,

	cdAddPictureFlags			OptionFlags,
	cdAddPictureProps*			pAddImageProps,	
	
	cdProgressCallbackFunction*	pCallbackFunc,
	cdContext					Context,
	cdProgressOption			ProgressOption,

	cdAddPictureInfo*			pAddInfo
);

typedef cdError cdSTDCALL cdDeleteImage(
	cdHItem			hParent,
	cdHImageItem 	hImgItem
);


/* Markfile Functions */

typedef cdError cdSTDCALL cdIsPrintMarked( 
	cdHImageItem	hImgItem,
	cdBoolean*  	pbMarked
);

typedef cdError cdSTDCALL cdIsSlideMarked( 
	cdHImageItem	hImgItem,
	cdBoolean*  	pbMarked
);

typedef cdError cdSTDCALL cdIsTransferMarked( 
	cdHImageItem	hImgItem,
	cdBoolean*  	pbMarked
);


/* Image Property Functions */

typedef cdError cdSTDCALL cdEnumBaseImageDataPropertyReset (
	cdHImageData				hImgData,
	cdHEnum*					phEnum
);

typedef cdError cdSTDCALL cdEnumBaseImageDataPropertyNext(
	cdHEnum						hEnum,
	cdBaseImagePropertyStruct*	pBaseImgPropStruct
);

typedef cdError cdSTDCALL cdGetBaseImageDataPropertyCount (
	cdHEnum						hEnum,
	cdUInt32*					pCount		
);

typedef cdError cdSTDCALL cdEnumBaseImageDataPropertyRelease (
	cdHEnum						hEnum
);

typedef cdError cdSTDCALL cdGetBaseImageDataProperty (
	cdHImageData			hImgData,
	cdBaseImagePropertyID 	BaseImgPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf
);

typedef cdError cdSTDCALL cdEnumImageItemPropertyReset (
	cdHImageItem			hImgItem,
	cdHEnum*				phEnum
);

typedef cdError cdSTDCALL cdEnumImageItemPropertyNext (
	cdHEnum					hEnum,
	cdImagePropertyStruct*	pImgPropStruct
);

typedef cdError cdSTDCALL cdGetImageItemPropertyCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount		
);

typedef cdError cdSTDCALL cdEnumImageItemPropertyRelease (
	cdHEnum					hEnum
);

typedef cdError cdSTDCALL cdGetImageItemProperty(
	cdHImageItem		hImgItem,
	cdImagePropertyID 	ImgPropID,
	cdUInt32*			pBufSize,
	cdVoid*				pBuf
);

typedef cdError cdSTDCALL cdSetImageItemProperty(
	cdHImageItem		hImgItem,
	cdImagePropertyID 	ImgPropID,
	cdUInt32*			pBufSize,
	cdVoid*				pBuf
);

typedef cdError cdSTDCALL cdFlashImageItemProperty(
	cdHImageItem		hImgItem
);



typedef cdError cdSTDCALL cdGetThumbnailValidArea(
	cdHImageItem	hImgItem,
	cdRect*			pFrame
);



/* Remote Release Control Functions */ 

typedef cdError cdSTDCALL cdEnterReleaseControl(
	cdHSource						hSource,
	cdReleaseEventCallbackFunction* pFunc,
	cdContext						Context
);

typedef cdError cdSTDCALL cdExitReleaseControl(
	cdHSource						hSource
);

typedef cdError cdSTDCALL cdGetReleaseControlFaculty(
	cdHSource						hSource,
	cdReleaseControlFaculty *	pFaculty
);

typedef cdError cdSTDCALL cdSelectReleaseDataKind( 
	cdHSource		hSource,
	cdRelDataKind	Kind
);

typedef cdError cdSTDCALL cdRelease(
	cdHSource							hSource,
	cdBoolean							bSync,
	cdProgressCallbackFunction*			pCallbackFunc,
	cdContext							Context,
	cdProgressOption					ProgressOption,
	cdUInt32*							pNumData
);

typedef cdError cdSTDCALL cdGetReleasedData(
	cdHSource						hSource,
	cdProgressCallbackFunction*		pCallbackFunc,
	cdContext						Context,
	cdProgressOption				ProgressOption,
	cdReleaseImageInfo*				pInfo,
	cdStgMedium*					pStgMedium
);

typedef cdError cdSTDCALL cdStartViewfinder(
	cdHSource						hSource,
	cdUInt32						Format,
	cdViewfinderCallbackFunction*	pFunc,     
	cdContext						Context
);

typedef cdError cdSTDCALL cdTermViewfinder(
	cdHSource						hSource
);

typedef cdError cdSTDCALL cdSelectViewfinderCameraOutput( 
	cdHSource						hSource,
	cdRelViewfinderOutput			cameraOutput
);

typedef cdError cdSTDCALL cdActViewfinderAutoFunctions(
	cdHSource						hSource,
	cdUInt32  						ActivateFlag
);

typedef cdError cdSTDCALL cdGetMaximumZoomPos( 
	cdHSource	hSource,
	cdUInt32*	pMaxZoomPos,
	cdUInt32*	pMaxOpticalZoomPos
);

typedef cdError cdSTDCALL cdGetZoomPos(
	cdHSource	hSource,  
	cdUInt32* 	pZoomPos
);

typedef cdError cdSTDCALL cdGetDZoomMagnification (
	cdHSource	hSource,  
	cdURational*	pDZoomMag
);

typedef cdError cdSTDCALL cdSetZoomPos(
	cdHSource	hSource,  
	cdUInt32 	ZoomPos
);

typedef cdError cdSTDCALL cdAFLock(	
	cdHSource		hSource,
	cdBoolean		bLock
);

typedef cdError cdSTDCALL cdSetImageFormatAttribute(
	cdHSource		hSource,
	cdCompQuality	Quality,
	cdImageSize		Size
);

typedef cdError cdSTDCALL cdGetImageFormatAttribute(
	cdHSource		hSource,
	cdCompQuality*	pQuality,
	cdImageSize*	pSize
);

typedef cdError cdSTDCALL cdEnumImageFormatAttributeReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);

typedef cdError cdSTDCALL cdEnumImageFormatAttributeNext(
	cdHEnum			hEnum,
	cdCompQuality *	pQuality,
	cdImageSize*	pSize
);

typedef cdError cdSTDCALL cdEnumImageFormatAttributeRelease(
	cdHEnum			hEnum
);

typedef cdError cdSTDCALL cdGetImageFormatAttributeCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);


typedef cdError cdSTDCALL cdSetDriveMode(
	cdHSource		hSource,
	cdDriveMode		DriveMode
);

typedef cdError cdSTDCALL cdGetDriveMode(
	cdHSource		hSource,
	cdDriveMode*	pDriveMode
);

typedef cdError cdSTDCALL cdEnumDriveModeReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumDriveModeNext(
	cdHEnum			hEnum,
	cdDriveMode*	pDriveMode
);
typedef cdError cdSTDCALL cdEnumDriveModeRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetDriveModeCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdSetShootingMode(	
	cdHSource		hSource,
	cdShootingMode	ShootingMode

);

typedef cdError cdSTDCALL cdGetShootingMode(
	cdHSource			hSource,
	cdShootingMode*		pShootingMode
);

typedef cdError cdSTDCALL cdEnumShootingModeReset(
	cdHSource			hSource,
	cdHEnum*			phEnum
);
typedef cdError cdSTDCALL cdEnumShootingModeNext(
	cdHEnum				hEnum,
	cdShootingMode*		pShootingMode
);
typedef cdError cdSTDCALL cdEnumShootingModeRelease(
	cdHEnum				hEnum
);
typedef cdError cdSTDCALL cdGetShootingModeCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

typedef cdError cdSTDCALL cdSetAvValue (
	cdHSource		hSource,
	cdRemoteSetAv	Av
);

typedef cdError cdSTDCALL cdGetAvValue (
	cdHSource		hSource,
	cdRemoteSetAv*	pAv
);

typedef cdError cdSTDCALL cdSetTvValue (
	cdHSource		hSource,
	cdRemoteSetTv	Tv
);

typedef cdError cdSTDCALL cdGetTvValue (
	cdHSource		hSource,
	cdRemoteSetTv*	pTv
);

typedef cdError cdSTDCALL cdEnumAvValueReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumAvValueNext(
	cdHEnum			hEnum,
	cdRemoteSetAv *	pAv
);
typedef cdError cdSTDCALL cdEnumAvValueRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetAvValueCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdEnumTvValueReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumTvValueNext(
	cdHEnum			hEnum,
	cdRemoteSetTv *	pTv
);
typedef cdError cdSTDCALL cdEnumTvValueRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetTvValueCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdSetExposureComp(
	cdHSource		hSource,
	cdCompensation	ExpoComp
);

typedef cdError cdSTDCALL cdGetExposureComp(
	cdHSource		hSource,
	cdCompensation*	pExpoComp
);

typedef cdError cdSTDCALL cdEnumExposureCompReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumExposureCompNext(
	cdHEnum			hEnum,
	cdCompensation*	pComp
);
typedef cdError cdSTDCALL cdEnumExposureCompRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetExposureCompCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdSetWBSetting(
	cdHSource		hSource,
	cdWBLightSrc	WhiteBalance
);

typedef cdError cdSTDCALL cdGetWBSetting(
	cdHSource		hSource,
	cdWBLightSrc*	pWhiteBalance
);

typedef cdError cdSTDCALL cdEnumWBSettingReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumWBSettingNext(
	cdHEnum			hEnum,
	cdWBLightSrc *	pWBLightSrc
);
typedef cdError cdSTDCALL cdEnumWBSettingRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetWBSettingCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdSetAFDistanceSetting (
	cdHSource		hSource,
	cdAFDistance	AFDistance
);

typedef cdError cdSTDCALL cdGetAFDistanceSetting (
	cdHSource		hSource,
	cdAFDistance*	pAFDistance
);

typedef cdError cdSTDCALL cdEnumAFDistanceSettingReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumAFDistanceSettingNext(
	cdHEnum			hEnum,
	cdAFDistance *	pAFDistance
);
typedef cdError cdSTDCALL cdEnumAFDistanceSettingRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetAFDistanceSettingCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdSetFlashSetting(
	cdHSource		hSource,
	cdFlashMode		Mode,
	cdCompensation	Compensation
);

typedef cdError cdSTDCALL cdGetFlashSetting(
	cdHSource		hSource,
	cdFlashMode*	pMode,
	cdCompensation*	pCompensation
);

typedef cdError cdSTDCALL cdEnumFlashSettingReset(
	cdHSource		hSource,
	cdHEnum*		phEnum
);
typedef cdError cdSTDCALL cdEnumFlashSettingNext(
	cdHEnum			hEnum,
	cdFlashMode *	pFlashMode
);
typedef cdError cdSTDCALL cdEnumFlashSettingRelease(
	cdHEnum			hEnum
);
typedef cdError cdSTDCALL cdGetFlashSettingCount(
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

typedef cdError cdSTDCALL cdGetNumAvailableShot( 
	cdHSource	hSource,
	cdUInt32* 	pNum
);

typedef cdError cdSTDCALL cdEnumRelCamSettingReset (
	cdHSource	hSource,
	cdHEnum*	phEnum
);

typedef cdError cdSTDCALL cdEnumRelCamSettingNext (
	cdHEnum					hEnum,
	cdRelCamSettingStruct*	pRelCamSettingStruct
);

typedef cdError cdSTDCALL cdGetRelCamSettingCount (
	cdHEnum		hEnum,
	cdUInt32*	pCount		
);

typedef cdError cdSTDCALL cdEnumRelCamSettingRelease (
	cdHEnum	hEnum
);

typedef cdError cdSTDCALL cdGetRelCamSettingData(
	cdHSource			hSource,
	cdRelCamSettingID	SettingID,
	cdUInt32*			pBufSize,
	cdVoid*				pBuf
);

typedef cdError cdSTDCALL cdSetRelCamSettingData(
	cdHSource			hSource,
	cdRelCamSettingID	SettingID,
	cdUInt32			BufSize,
	cdVoid*				pBuf
);

typedef cdError cdSTDCALL cdEnumRelCamSettingDataReset(
	cdHSource			hSource,
	cdRelCamSettingID	SettingID,
	cdHEnum*			phEnum,
	cdUInt32*			pBufSize
);
typedef cdError cdSTDCALL cdEnumRelCamSettingDataNext(
	cdHEnum				hEnum,
	cdUInt32			BufSize,
	cdVoid*				pBuf
);
typedef cdError cdSTDCALL cdEnumRelCamSettingDataRelease(
	cdHEnum				hEnum
);
typedef cdError cdSTDCALL cdGetRelCamSettingDataCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

/*-----------------------------------------------------------------------
	cdFunctions
	  Structure of function pointers
------------------------------------------------------------------------*/
typedef struct {
	cdUInt32		Version;	/* Version of this structure */

/* Basic Functions */
	cdStartSDK*							pStartSDK;						 
	cdFinishSDK*						pFinishSDK;
	cdGetSDKVersion*					pGetSDKVersion;
/* Utility Functions */
	cdCreateMemStream*					pCreateMemStream;
	cdDestroyMemStream*					pDestroyMemStream;
	cdGetStreamInfo*					pGetStreamInfo;
/* Source Control Functions */
	cdEnumDeviceReset*					pEnumDeviceReset;
	cdEnumDeviceNext*					pEnumDeviceNext;
	cdGetDeviceCount*					pGetDeviceCount;
	cdEnumDeviceRelease*				pEnumDeviceRelease;
	cdOpenSource*						pOpenSource;
	cdCloseSource*						pCloseSource;
	cdRegisterEventCallbackFunction*	pRegisterEventCallbackFunction;
	cdUnregisterEventCallbackFunction*	pUnregisterEventCallbackFunction;
/* Device Management Functions */
	cdEnumDevicePropertyReset*			pEnumDevicePropertyReset;
	cdEnumDevicePropertyNext*			pEnumDevicePropertyNext;
	cdGetDevicePropertyCount*			pGetDevicePropertyCount;
	cdEnumDevicePropertyRelease*		pEnumDevicePropertyRelease;
	cdGetDevicePropertyData*			pGetDevicePropertyData;
	cdSetDevicePropertyData*			pSetDevicePropertyData;
	cdEnumSupportedImageSizeReset*		pEnumSupportedImageSizeReset;
	cdEnumSupportedImageSizeNext*		pEnumSupportedImageSizeNext;
	cdGetSupportedImageSizeCount*		pGetSupportedImageSizeCount;
	cdEnumSupportedImageSizeRelease*	pEnumSupportedImageSizeRelease;
	cdLockUI*							pLockUI;
	cdUnlockUI*							pUnlockUI;
	cdSetUILockTimeOutTime*				pSetUILockTimeOutTime;
	cdFormat*							pFormat;
/* Collect Functions */
	cdEnumVolumeReset*					pEnumVolumeReset;
	cdEnumVolumeNext*					pEnumVolumeNext;
	cdGetVolumeCount*					pGetVolumeCount;
	cdEnumVolumeRelease*				pEnumVolumeRelease;
	cdGetVolumeInfo*					pGetVolumeInfo;
	cdEnumItemReset*					pEnumItemReset;
	cdEnumItemNext*						pEnumItemNext;
	cdGetItemCount*						pGetItemCount;
	cdEnumItemRelease*					pEnumItemRelease;
	cdEnumImageItemReset*				pEnumImageItemReset;
	cdEnumImageItemNext*				pEnumImageItemNext;
	cdGetImageItemCount*				pGetImageItemCount;
	cdEnumImageItemRelease*				pEnumImageItemRelease;
	cdGetItemInfo*						pGetItemInfo;
	cdMakeImageItem*					pMakeImageItem;
/* Image Management Functions */
	cdOpenImage*						pOpenImage;
	cdCloseImage*						pCloseImage;
	cdGetThumbnail*						pGetThumbnail;
	cdGetPicture*						pGetPicture;
	cdGetMovie*							pGetMovie;
	cdGetSound*							pGetSound;
	cdEnumImageDataInItemReset*			pEnumImageDataInItemReset;
	cdEnumImageDataInItemNext*			pEnumImageDataInItemNext;
	cdGetImageDataInItemCount*			pGetImageDataInItemCount;
	cdEnumImageDataInItemRelease*		pEnumImageDataInItemRelease;
	cdGetImageDataInfo*					pGetImageDataInfo;
	cdGetImageData*						pGetImageData;
	cdAddPicture*						pAddPicture;
	cdDeleteImage*						pDeleteImage;
/* Markfile Functions */
	cdIsPrintMarked*					pIsPrintMarked;
	cdIsSlideMarked*					pIsSlideMarked;
	cdIsTransferMarked*					pIsTransferMarked;
/* Image Property Functions */
	cdEnumBaseImageDataPropertyReset*	pEnumBaseImageDataPropertyReset;
	cdEnumBaseImageDataPropertyNext*	pEnumBaseImageDataPropertyNext;
	cdGetBaseImageDataPropertyCount*	pGetBaseImageDataPropertyCount;
	cdEnumBaseImageDataPropertyRelease*	pEnumBaseImageDataPropertyRelease;
	cdGetBaseImageDataProperty*			pGetBaseImageDataProperty;
	cdEnumImageItemPropertyReset*		pEnumImageItemPropertyReset;
	cdEnumImageItemPropertyNext*		pEnumImageItemPropertyNext;
	cdGetImageItemPropertyCount*		pGetImageItemPropertyCount;
	cdEnumImageItemPropertyRelease*		pEnumImageItemPropertyRelease;
	cdGetImageItemProperty*				pGetImageItemProperty;
	cdSetImageItemProperty*				pSetImageItemProperty;
	cdFlashImageItemProperty*			pFlashImageItemProperty;
	cdGetThumbnailValidArea*			pGetThumbnailValidArea;
/* Remote Release Control Functions */
	cdEnterReleaseControl*				pEnterReleaseControl;
	cdExitReleaseControl*				pExitReleaseControl;
	cdSelectReleaseDataKind*			pSelectReleaseDataKind;
	cdRelease*							pRelease;
	cdGetReleasedData*					pGetReleasedData;
	cdStartViewfinder*					pStartViewfinder;
	cdTermViewfinder*					pTermViewfinder;
	cdSelectViewfinderCameraOutput*		pSelectViewfinderCameraOutput;
	cdActViewfinderAutoFunctions*		pActViewfinderAutoFunctions;
	cdGetMaximumZoomPos*				pGetMaximumZoomPos;
	cdGetZoomPos*						pGetZoomPos;
	cdGetDZoomMagnification*			pGetDZoomMagnification;
	cdSetZoomPos*						pSetZoomPos;
	cdSetImageFormatAttribute*			pSetImageFormatAttribute;
	cdGetImageFormatAttribute*			pGetImageFormatAttribute;
	cdSetDriveMode*						pSetDriveMode;
	cdGetDriveMode*						pGetDriveMode;
	cdSetShootingMode*					pSetShootingMode;
	cdGetShootingMode*					pGetShootingMode;
	cdSetAvValue*						pSetAvValue;
	cdGetAvValue*						pGetAvValue;
	cdSetTvValue*						pSetTvValue;
	cdGetTvValue*						pGetTvValue;
	cdSetExposureComp*					pSetExposureComp;
	cdGetExposureComp*					pGetExposureComp;
	cdSetWBSetting*						pSetWBSetting;
	cdGetWBSetting*						pGetWBSetting;
	cdSetAFDistanceSetting*				pSetAFDistanceSetting;
	cdGetAFDistanceSetting*				pGetAFDistanceSetting;
	cdSetFlashSetting*					pSetFlashSetting;
	cdGetFlashSetting*					pGetFlashSetting;
	cdGetNumAvailableShot*				pGetNumAvailableShot;
	cdEnumRelCamSettingReset*			pEnumRelCamSettingReset;
	cdEnumRelCamSettingNext*			pEnumRelCamSettingNext;
	cdGetRelCamSettingCount*			pGetRelCamSettingCount;
	cdEnumRelCamSettingRelease*			pEnumRelCamSettingRelease;
	cdGetRelCamSettingData*				pGetRelCamSettingData;
	cdSetRelCamSettingData*				pSetRelCamSettingData;

	/* Added in V5.0 */
	cdEnumImageFormatAttributeReset*	pEnumImageFormatAttributeReset;
	cdEnumImageFormatAttributeNext*		pEnumImageFormatAttributeNext;
	cdEnumImageFormatAttributeRelease*	pEnumImageFormatAttributeRelease;
	cdGetImageFormatAttributeCount*		pGetImageFormatAttributeCount;

	cdEnumDriveModeReset*				pEnumDriveModeReset;
	cdEnumDriveModeNext*				pEnumDriveModeNext;
	cdEnumDriveModeRelease*				pEnumDriveModeRelease;
	cdGetDriveModeCount*				pGetDriveModeCount;

	cdEnumShootingModeReset*			pEnumShootingModeReset;
	cdEnumShootingModeNext*				pEnumShootingModeNext;
	cdEnumShootingModeRelease*			pEnumShootingModeRelease;
	cdGetShootingModeCount*				pGetShootingModeCount;

	cdEnumAvValueReset*					pEnumAvValueReset;
	cdEnumAvValueNext*					pEnumAvValueNext;
	cdEnumAvValueRelease*				pEnumAvValueRelease;
	cdGetAvValueCount*					pGetAvValueCount;
	cdEnumTvValueReset*					pEnumTvValueReset;
	cdEnumTvValueNext*					pEnumTvValueNext;
	cdEnumTvValueRelease*				pEnumTvValueRelease;
	cdGetTvValueCount*					pGetTvValueCount;

	cdEnumExposureCompReset*			pEnumExposureCompReset;
	cdEnumExposureCompNext*				pEnumExposureCompNext;
	cdEnumExposureCompRelease*			pEnumExposureCompRelease;
	cdGetExposureCompCount*				pGetExposureCompCount;

	cdEnumWBSettingReset*				pEnumWBSettingReset;
	cdEnumWBSettingNext*				pEnumWBSettingNext;
	cdEnumWBSettingRelease*				pEnumWBSettingRelease;
	cdGetWBSettingCount*				pGetWBSettingCount;

	cdEnumAFDistanceSettingReset*		pEnumAFDistanceSettingReset;
	cdEnumAFDistanceSettingNext*		pEnumAFDistanceSettingNext;
	cdEnumAFDistanceSettingRelease*		pEnumAFDistanceSettingRelease;
	cdGetAFDistanceSettingCount*		pGetAFDistanceSettingCount;

	cdEnumFlashSettingReset*			pEnumFlashSettingReset;
	cdEnumFlashSettingNext*				pEnumFlashSettingNext;
	cdEnumFlashSettingRelease*			pEnumFlashSettingRelease;
	cdGetFlashSettingCount*				pGetFlashSettingCount;

	cdEnumRelCamSettingDataReset*		pEnumRelCamSettingDataReset;
	cdEnumRelCamSettingDataNext*		pEnumRelCamSettingDataNext;
	cdEnumRelCamSettingDataRelease*		pEnumRelCamSettingDataRelease;
	cdGetRelCamSettingDataCount*		pGetRelCamSettingDataCount;

	cdAFLock*							pAFLock;
	
	cdGetVolumeName*					pGetVolumeName;

	/* Added in V6.0 */
	cdGetReleaseControlFaculty*		pGetReleaseControlFaculty;
	cdFreeImageItem*					pFreeImageItem;

	/* Added in V7.0 */
	cdGetImagePropertyPart*				pGetImagePropertyPart;

} cdFunctions;

/*------------------------------------------------------------------------
	Function Pointer to get the structure of function pointers
------------------------------------------------------------------------*/
typedef cdError cdSTDCALL cdGetFunctions(
	cdFunctions *	pFunctions
);


#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=reset
 #endif
#endif 

#endif