#ifndef _CD_API_H_
#define _CD_API_H_

#include "cdType.h"
#include "cdFncTyp.h"
#include "cdEvent.h"
#include "cdError.h"

#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=mac68k
 #endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************
 ****************************************************************************
 *             Canon Digital Camera SDK : Function Definitions              *
 *                            For Version 7.1                               *
 ****************************************************************************
 ****************************************************************************/

/*-----------------------------------------------------------------------
   Basic Functions 
------------------------------------------------------------------------*/

cdCAPI CDStartSDK( 
	cdVersionInfo*	pVersion,
	cdUInt32		Option
);

cdCAPI CDFinishSDK();

cdCAPI CDGetSDKVersion(
	cdVersionInfo*	pVersion
);

cdCAPI CDGetFunctions (
	cdFunctions *	pFunctions
);


/*-----------------------------------------------------------------------
   Source Control Functions
------------------------------------------------------------------------*/

cdCAPI CDEnumDeviceReset(
	cdUInt32		Kind,
	cdHEnum*		phEnum	
);

cdCAPI CDEnumDeviceNext(
	cdHEnum			hEnum,	
	cdSourceInfo* 	pSourceInfo
);

cdCAPI CDGetDeviceCount (
	cdHEnum			hEnum,
	cdUInt32*		pCount
);

cdCAPI CDEnumDeviceRelease (
	cdHEnum			hEnum
);

cdCAPI CDOpenSource( 
	cdSourceInfo*	pSourceInfo,
	cdHSource*		phSource
);

cdCAPI CDCloseSource(
	cdHSource		hSource
);

cdCAPI CDRegisterEventCallbackFunction(
	cdHSource					hSource,
	cdEventCallbackFunction*	pFunc,
	cdContext					Context,
	cdHandle*					phFunc
);

cdCAPI CDUnregisterEventCallbackFunction(
	cdHSource					hSource,
	cdHandle					hFunc
);


/*-----------------------------------------------------------------------
   Device Management Functions 
------------------------------------------------------------------------*/

cdCAPI CDEnumDevicePropertyReset (
	cdHSource				hSource,
	cdUInt32				Option,
	cdHEnum*				phEnum
);

cdCAPI CDEnumDevicePropertyNext (
	cdHEnum					hEnum,
	cdDevicePropertyStruct*	pDevPropStruct
);

cdCAPI CDGetDevicePropertyCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount
);

cdCAPI CDEnumDevicePropertyRelease (
	cdHEnum					hEnum
);

cdCAPI CDGetDevicePropertyData(
	cdHSource				hSource,
	cdDevicePropertyID 		DevPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf,
	cdUInt32				Option
);

cdCAPI CDSetDevicePropertyData(
	cdHSource				hSource,
	cdDevicePropertyID  	DevPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf
);

cdCAPI CDEnumSupportedImageSizeReset(
	cdHSource				hSource,
	cdUInt32				Option,
	cdHEnum*				phEnum
);

cdCAPI CDEnumSupportedImageSizeNext(
	cdHEnum					hEnum,
	cdImageSizeSpec*		pSpec
);

cdCAPI CDGetSupportedImageSizeCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount
);

cdCAPI CDEnumSupportedImageSizeRelease (
	cdHEnum					hEnum
);

cdCAPI CDLockUI(
	cdHSource				hSource
);

cdCAPI CDUnlockUI(
	cdHSource				hSource
);

cdCAPI CDSetUILockTimeOutTime(
	cdHSource				hSource,
	cdUInt32				Time
);

cdCAPI CDFormat(
	cdHVolume				hVolume
);


/*-----------------------------------------------------------------------
   Collect Functions  
------------------------------------------------------------------------*/

cdCAPI CDEnumVolumeReset(		
	cdHSource			hSource,
	cdHEnum*			phEnum
);

cdCAPI CDEnumVolumeNext(
	cdHEnum				hEnum,
	cdHVolume *			phVolume
);

cdCAPI CDGetVolumeCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount		
);

cdCAPI CDEnumVolumeRelease(
	cdHEnum				hEnum
);

cdCAPI CDGetVolumeName(
	cdHVolume			hVolume,
	cdVolName*			pVolName
);

cdCAPI CDGetVolumeInfo(
	cdHVolume			hVolume,
	cdVolumeInfo*		pVolInfo
);

cdCAPI CDEnumItemReset(
	cdHItem				hParent,
	cdEnumItemOption	Option,
	cdHEnum*			phEnum
);

cdCAPI CDEnumItemNext(
	cdHEnum				hEnum,
	cdHItem*			phItem
);

cdCAPI CDGetItemCount (
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

cdCAPI CDEnumItemRelease (
	cdHEnum				hEnum
);

cdCAPI CDEnumImageItemReset(
	cdHItem				hItem,
	cdUInt32			Depth,
	cdEnumItemOption	Option,
	cdHEnum*			phEnum
);

cdCAPI CDEnumImageItemNext( 
	cdHEnum				hEnum,
	cdHImageItem*		phImage
);

cdCAPI CDGetImageItemCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

cdCAPI CDEnumImageItemRelease (
	cdHEnum				hEnum
);

cdCAPI CDGetItemInfo(
	cdHItem				hItem,
	cdItemInfo*			pItemInfo
);

cdCAPI CDMakeImageItem(
	cdStgMedium			StgMedium,
	cdImageFormat		Format,
	cdHImageItem*		phImgItem
);

cdCAPI CDFreeImageItem(
	cdHImageItem		hImgItem
);

/*-----------------------------------------------------------------------
   Image Management Functions  
------------------------------------------------------------------------*/

cdCAPI CDOpenImage(
	cdHImageItem		hImgItem
);

cdCAPI CDCloseImage(
	cdHImageItem		hImgItem
);

cdCAPI CDGetThumbnail(
	cdHImageItem		hImgItem,
	cdHImageData*		phImgData
);

cdCAPI CDGetPicture(
	cdHImageItem		hImgItem,
	cdHImageData*		phImgData
);

cdCAPI CDGetMovie (
	cdHImageItem		hImgItem,
	cdHImageData*		phImgData
);

cdCAPI CDGetSound (
	cdHImageItem		hImgItem,
	cdHImageData*		phImgData
);

cdCAPI CDEnumImageDataInItemReset(
	cdHImageItem		hImgItem,
	cdHEnum*			phEnum
);

cdCAPI CDEnumImageDataInItemNext(
	cdHEnum				hEnum,
	cdHImageData*		phImgData
);

cdCAPI CDGetImageDataInItemCount(
	cdHEnum				hEnum,
	cdUInt32*			pCount
);

cdCAPI CDEnumImageDataInItemRelease (
	cdHEnum				hEnum
);

cdCAPI CDGetImageDataInfo(
	cdHImageData		hImgData,
	cdImageDataInfo*	pImgDataInfo
);

cdCAPI CDGetImageData(
	cdHImageData 				hImgData,
	cdStgMedium*				pStgMediumDest,
	cdProgressCallbackFunction*	pCallbackFunc,
	cdContext					Context,
	cdProgressOption			ProgressOption
);

cdCAPI CDAddPicture(
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

cdCAPI CDDeleteImage(
	cdHItem				hParent,
	cdHImageItem 		hImgItem
);


/*-----------------------------------------------------------------------
   Markfile Functions     
------------------------------------------------------------------------*/

cdCAPI CDIsPrintMarked( 
	cdHImageItem		hImgItem,
	cdBoolean*  		pbMarked
);

cdCAPI CDIsSlideMarked( 
	cdHImageItem		hImgItem,
	cdBoolean*  		pbMarked
);

cdCAPI CDIsTransferMarked( 
	cdHImageItem		hImgItem,
	cdBoolean*  		pbMarked
);


/*-----------------------------------------------------------------------
   Image Property Functions
------------------------------------------------------------------------*/

cdCAPI CDEnumBaseImageDataPropertyReset (
	cdHImageData			hImgData,
	cdHEnum*				phEnum
);

cdCAPI CDEnumBaseImageDataPropertyNext(
	cdHEnum					hEnum,
	cdBaseImagePropertyStruct*	pBaseImgPropStruct
);

cdCAPI CDGetBaseImageDataPropertyCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount		
);

cdCAPI CDEnumBaseImageDataPropertyRelease (
	cdHEnum					hEnum
);

cdCAPI CDGetBaseImageDataProperty (
	cdHImageData			hImgData,
	cdBaseImagePropertyID 	BaseImgPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf
);

cdCAPI CDEnumImageItemPropertyReset (
	cdHImageItem			hImgItem,
	cdHEnum*				phEnum
);

cdCAPI CDEnumImageItemPropertyNext (
	cdHEnum					hEnum,
	cdImagePropertyStruct*	pImgPropStruct
);

cdCAPI CDGetImageItemPropertyCount (
	cdHEnum					hEnum,
	cdUInt32*				pCount		
);

cdCAPI CDEnumImageItemPropertyRelease (
	cdHEnum					hEnum
);

cdCAPI CDGetImageItemProperty(
	cdHImageItem			hImgItem,
	cdImagePropertyID 		ImgPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf
);

cdCAPI CDSetImageItemProperty(
	cdHImageItem			hImgItem,
	cdImagePropertyID 		ImgPropID,
	cdUInt32*				pBufSize,
	cdVoid*					pBuf
);

cdCAPI CDFlashImageItemProperty(
	cdHImageItem			hImgItem
);

cdCAPI CDGetThumbnailValidArea(
	cdHImageItem			hImgItem,
	cdRect*					pFrame
);


/*-----------------------------------------------------------------------
   Remote Release Control Functions  
------------------------------------------------------------------------*/

cdCAPI CDEnterReleaseControl(
	cdHSource						hSource,
	cdReleaseEventCallbackFunction* pFunc,
	cdContext						Context
);

cdCAPI CDExitReleaseControl(
	cdHSource						hSource
);

cdCAPI CDGetReleaseControlFaculty(
	cdHSource						hSource,
	cdReleaseControlFaculty *	pFaculty
);

cdCAPI CDSelectReleaseDataKind( 
	cdHSource						hSource,
	cdRelDataKind					Kind
);

cdCAPI CDRelease(
	cdHSource						hSource,
	cdBoolean						bSync,
	cdProgressCallbackFunction*		pCallbackFunc,
	cdContext						Context,
	cdProgressOption				ProgressOption,
	cdUInt32*						pNumData
);

cdCAPI CDGetReleasedData(
	cdHSource						hSource,
	cdProgressCallbackFunction*		pCallbackFunc,
	cdContext						Context,
	cdProgressOption				ProgressOption,
	cdReleaseImageInfo*				pInfo,
	cdStgMedium*					pStgMedium
);

cdCAPI CDStartViewfinder(
	cdHSource						hSource,
	cdUInt32						Format,
	cdViewfinderCallbackFunction*	pFunc,
	cdContext						Context
);

cdCAPI CDTermViewfinder(
	cdHSource						hSource
);

cdCAPI CDSelectViewfinderCameraOutput( 
	cdHSource						hSource,
	cdRelViewfinderOutput			cameraOutput
);

cdCAPI CDActViewfinderAutoFunctions(
	cdHSource						hSource,
	cdUInt32  						ActivateFlag
);

cdCAPI CDGetMaximumZoomPos( 
	cdHSource						hSource,
	cdUInt32*						pMaxZoomPos,
	cdUInt32*						pMaxOpticalZoomPos
);

cdCAPI CDGetZoomPos(
	cdHSource						hSource,  
	cdUInt32* 						pZoomPos
);

cdCAPI CDGetDZoomMagnification (
	cdHSource						hSource,  
	cdURational*					pDZoomMag
);

cdCAPI CDSetZoomPos(
	cdHSource						hSource,  
	cdUInt32 						ZoomPos
);

cdCAPI CDAFLock(	
	cdHSource						hSource,
	cdBoolean						bLock
);

cdCAPI CDSetImageFormatAttribute(
	cdHSource						hSource,
	cdCompQuality					Quality,
	cdImageSize						Size
);

cdCAPI CDGetImageFormatAttribute(
	cdHSource						hSource,
	cdCompQuality*					pQuality,
	cdImageSize*					pSize
);

cdCAPI CDEnumImageFormatAttributeReset(
	cdHSource						hSource,
	cdHEnum*						phEnum
);

cdCAPI CDEnumImageFormatAttributeNext(
	cdHEnum							hEnum,
	cdCompQuality *					pQuality,
	cdImageSize*					pSize
);

cdCAPI CDEnumImageFormatAttributeRelease(
	cdHEnum							hEnum
);

cdCAPI CDGetImageFormatAttributeCount(
	cdHEnum							hEnum,
	cdUInt32*						pCount
);	

cdCAPI CDSetDriveMode(
	cdHSource						hSource,
	cdDriveMode						DriveMode
);

cdCAPI CDGetDriveMode(
	cdHSource						hSource,
	cdDriveMode*					pDriveMode
);

cdCAPI CDEnumDriveModeReset(
	cdHSource						hSource,
	cdHEnum*						phEnum
);
cdCAPI CDEnumDriveModeNext(
	cdHEnum							hEnum,
	cdDriveMode*					pDriveMode
);
cdCAPI CDEnumDriveModeRelease(
	cdHEnum							hEnum
);
cdCAPI CDGetDriveModeCount(
	cdHEnum							hEnum,
	cdUInt32*						pCount
);

cdCAPI CDSetShootingMode(	
	cdHSource					hSource,
	cdShootingMode				ShootingMode
);

cdCAPI CDGetShootingMode(
	cdHSource					hSource,
	cdShootingMode*				pShootingMode
);

cdCAPI CDEnumShootingModeReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumShootingModeNext(
	cdHEnum						hEnum,
	cdShootingMode *			pShootingMode
);
cdCAPI CDEnumShootingModeRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetShootingModeCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDSetAvValue (
	cdHSource					hSource,
	cdRemoteSetAv				Av
);

cdCAPI CDGetAvValue (
	cdHSource					hSource,
	cdRemoteSetAv*				pAv
);

cdCAPI CDSetTvValue (
	cdHSource					hSource,
	cdRemoteSetTv				Tv
);

cdCAPI CDGetTvValue (
	cdHSource					hSource,
	cdRemoteSetTv*				pTv
);

cdCAPI CDEnumAvValueReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumAvValueNext(
	cdHEnum						hEnum,
	cdRemoteSetAv *				pAv
);
cdCAPI CDEnumAvValueRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetAvValueCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDEnumTvValueReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumTvValueNext(
	cdHEnum						hEnum,
	cdRemoteSetTv *				pTv
);
cdCAPI CDEnumTvValueRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetTvValueCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDSetExposureComp(
	cdHSource					hSource,
	cdCompensation				ExpoComp
);

cdCAPI CDGetExposureComp(
	cdHSource					hSource,
	cdCompensation*				pExpoComp
);

cdCAPI CDEnumExposureCompReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumExposureCompNext(
	cdHEnum						hEnum,
	cdCompensation *			pComp
);
cdCAPI CDEnumExposureCompRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetExposureCompCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDSetWBSetting(
	cdHSource					hSource,
	cdWBLightSrc				WhiteBalance
);

cdCAPI CDGetWBSetting(
	cdHSource					hSource,
	cdWBLightSrc*				pWhiteBalance
);

cdCAPI CDEnumWBSettingReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumWBSettingNext(
	cdHEnum						hEnum,
	cdWBLightSrc *				pWBLightSrc
);
cdCAPI CDEnumWBSettingRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetWBSettingCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDSetAFDistanceSetting (
	cdHSource					hSource,
	cdAFDistance				AFDistance
);

cdCAPI CDGetAFDistanceSetting (
	cdHSource					hSource,
	cdAFDistance*				pAFDistance
);

cdCAPI CDEnumAFDistanceSettingReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumAFDistanceSettingNext(
	cdHEnum						hEnum,
	cdAFDistance *				pAFDistance
);
cdCAPI CDEnumAFDistanceSettingRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetAFDistanceSettingCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDSetFlashSetting(
	cdHSource					hSource,
	cdFlashMode					Mode,
	cdCompensation				Compensation
);

cdCAPI CDGetFlashSetting(
	cdHSource					hSource,
	cdFlashMode*				pMode,
	cdCompensation*				pCompensation
);

cdCAPI CDEnumFlashSettingReset(
	cdHSource					hSource,
	cdHEnum*					phEnum
);
cdCAPI CDEnumFlashSettingNext(
	cdHEnum						hEnum,
	cdFlashMode *				pFlashMode
);
cdCAPI CDEnumFlashSettingRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetFlashSettingCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);

cdCAPI CDGetNumAvailableShot( 
	cdHSource					hSource,
	cdUInt32* 					pNum
);

cdCAPI CDEnumRelCamSettingReset (
	cdHSource					hSource,
	cdHEnum*					phEnum
);

cdCAPI CDEnumRelCamSettingNext (
	cdHEnum						hEnum,
	cdRelCamSettingStruct*		pRelCamSettingStruct
);

cdCAPI CDGetRelCamSettingCount (
	cdHEnum						hEnum,
	cdUInt32*					pCount		
);

cdCAPI CDEnumRelCamSettingRelease (
	cdHEnum						hEnum
);

cdCAPI CDGetRelCamSettingData(
	cdHSource					hSource,
	cdRelCamSettingID			SettingID,
	cdUInt32*					pBufSize,
	cdVoid*						pBuf
);

cdCAPI CDSetRelCamSettingData(
	cdHSource					hSource,
	cdRelCamSettingID			SettingID,
	cdUInt32					BufSize,
	cdVoid*						pBuf
);

cdCAPI CDEnumRelCamSettingDataReset(
	cdHSource					hSource,
	cdRelCamSettingID			SettingID,
	cdHEnum*					phEnum,
	cdUInt32*					pBufSize
);
cdCAPI CDEnumRelCamSettingDataNext(
	cdHEnum						hEnum,
	cdUInt32					BufSize,
	cdVoid*						pBuf
);
cdCAPI CDEnumRelCamSettingDataRelease(
	cdHEnum						hEnum
);
cdCAPI CDGetRelCamSettingDataCount(
	cdHEnum						hEnum,
	cdUInt32*					pCount
);



/*-----------------------------------------------------------------------
   Utility Functions 
------------------------------------------------------------------------*/

cdCAPI CDCreateMemStream(
	cdUInt32					StartSize, 
	cdUInt32					MinAllocSize,
	cdStream*					pStream
);

cdCAPI CDDestroyMemStream(
	cdStream*					pStream
);

cdCAPI CDGetStreamInfo(
	cdStream*					pStream, 
	cdUInt32*					pSize,
	cdVoid **					ppMem 
);

cdCAPI CDGetImagePropertyPart(
	cdHImageItem				hImgItem,
	cdStgMedium*				pStgMedium
);



#ifdef macintosh
 #if PRAGMA_STRUCT_ALIGN
  #pragma options align=reset
 #endif
#endif 

#ifdef __cplusplus
}
#endif
	
#endif
