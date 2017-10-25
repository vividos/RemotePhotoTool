//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ErrorText.cpp Canon control - Error texts
//

// includes
#include "stdafx.h"
#include "ErrorText.hpp"
#include "CDSDK/CdsdkCommon.hpp"
#include "EDSDK/EdsdkCommon.hpp"
#include "PSREC/PsrecCommon.hpp"

LPCTSTR ErrorTextFromErrorId(unsigned int uiErrorId, bool bIsPsrec)
{
   if (bIsPsrec)
   {
      // these are different to the other common codes, so treat them specially here
      switch (uiErrorId)
      {
         // Function Parameter errors
      case prINVALID_PARAMETER: return _T("Invalid parameter");
      case prINVALID_HANDLE: return _T("Invalid handle");
      case prINVALID_FN_CALL: return _T("Invalid function call");
      case prWAIT_TIMEOUT_ERROR: return _T("Wait timeout error");
      case prINSUFFICIENT_BUFFER: return _T("Insufficient buffer");
      case prEVENT_CALLBACK_EXIST: return _T("Event callback exists");
      }
   }

   switch (uiErrorId)
   {
   case cdOK: return _T("OK");

      // Miscellaneous errors
   case cdUNIMPLEMENTED: return _T("Unimplemented");
   case cdINTERNAL_ERROR: return _T("Internal Error");
   case cdMEM_ALLOC_FAILED: return _T("Mem alloc failed");
   case cdMEM_FREE_FAILED: return _T("Mem free failed");
   case cdOPERATION_CANCELLED: return _T("Operation cancelled");
   case cdINCOMPATIBLE_VERSION: return _T("Incompatible version");
   case cdNOT_SUPPORTED: return _T("Not supported");
   case cdUNEXPECTED_EXCEPTION: return _T("Unexpected exception");
   case cdPROTECTION_VIOLATION: return _T("Protection violation");
   case cdMISSING_SUBCOMPONENT: return _T("Missing subcomponent");
   case cdSELECTION_UNAVAILABLE: return _T("Selection unavailable");

      // File errors
   case cdFILE_IO_ERROR: return _T("File IO error");
   case cdFILE_TOO_MANY_OPEN: return _T("Too many files open");
   case cdFILE_NOT_FOUND: return _T("File not found");
   case cdFILE_OPEN_ERROR: return _T("File open error");
   case cdFILE_CLOSE_ERROR: return _T("File close error");
   case cdFILE_SEEK_ERROR: return _T("File seek error");
   case cdFILE_TELL_ERROR: return _T("File tell error");
   case cdFILE_READ_ERROR: return _T("File read error");
   case cdFILE_WRITE_ERROR: return _T("File write error");
   case cdFILE_PERMISSION_ERROR: return _T("File permission error");
   case cdFILE_DISK_FULL_ERROR: return _T("Disk full error");
   case cdFILE_ALREADY_EXISTS: return _T("File already exists");
   case cdFILE_FORMAT_UNRECOGNIZED: return _T("File format not recognized");
   case cdFILE_DATA_CORRUPT: return _T("File data corrupt");
   case cdFILE_NAMING_NA: return _T("File naming N/A");

      // Directory errors
   case cdDIR_NOT_FOUND: return _T("Dir not found");
   case cdDIR_IO_ERROR: return _T("Dir IO error");
   case cdDIR_ENTRY_NOT_FOUND: return _T("Dir entry not found");
   case cdDIR_ENTRY_EXISTS: return _T("Dir entry exists");
   case cdDIR_NOT_EMPTY: return _T("Dir not empty");

      // Property errors
   case cdPROPERTIES_UNAVAILABLE: return _T("Properties unavailable");
   case cdPROPERTIES_MISMATCH: return _T("Properties mismatch");
   case cdPROPERTIES_NOT_LOADED: return _T("Properties not loaded");

      // Function Parameter errors
   case cdINVALID_PARAMETER: return _T("Invalid parameter");
   case cdINVALID_HANDLE: return _T("Invalid handle");
   case cdINVALID_POINTER: return _T("Invalid pointer");
   case cdINVALID_INDEX: return _T("Invalid index");
   case cdINVALID_LENGTH: return _T("Invalid length");
   case cdINVALID_FN_POINTER: return _T("Invalid function pointer");
   case cdINVALID_SORT_FN: return _T("Invalid sort function");

      // Device errors
   case cdDEVICE_NOT_FOUND: return _T("Device not found");
   case cdDEVICE_BUSY: return _T("Device busy");
   case cdDEVICE_INVALID: return _T("Device invalid");
   case cdDEVICE_EMERGENCY: return _T("Device emergency");
   case cdDEVICE_MEMORY_FULL: return _T("Device memory full");
   case cdDEVICE_INTERNAL_ERROR: return _T("Device internal error");
   case cdDEVICE_INVALID_PARAMETER: return _T("Invalid parameter");
   case cdDEVICE_NO_DISK: return _T("Device no disk");
   case cdDEVICE_DISK_ERROR: return _T("Device disk error");
   case cdDEVICE_CF_GATE_CHANGED: return _T("Device CF gate changed");
   case cdDEVICE_DIAL_CHANGED: return _T("Device dial changed");
   case cdDEVICE_NOT_INSTALLED: return _T("Device not installed");
   case cdDEVICE_STAY_AWAKE: return _T("Device stay awake");
   case cdDEVICE_NOT_RELEASED: return _T("Device not released");

      // Stream errors
   case cdSTREAM_IO_ERROR: return _T("Stream IO error");
   case cdSTREAM_NOT_OPEN: return _T("Stream not open");
   case cdSTREAM_ALREADY_OPEN: return _T("Stream already open");
   case cdSTREAM_OPEN_ERROR: return _T("Stream open error");
   case cdSTREAM_CLOSE_ERROR: return _T("Stream close error");
   case cdSTREAM_SEEK_ERROR: return _T("Stream seek error");
   case cdSTREAM_TELL_ERROR: return _T("Stream tell error");
   case cdSTREAM_READ_ERROR: return _T("Stream read error");
   case cdSTREAM_WRITE_ERROR: return _T("Stream write error");
   case cdSTREAM_PERMISSION_ERROR: return _T("Stream permission error");
   case cdSTREAM_COULDNT_BEGIN_THREAD: return _T("Stream couldn't begin thread");
   case cdSTREAM_BAD_OPTIONS: return _T("Stream bad options");
   case cdSTREAM_END_OF_STREAM: return _T("End of stream");

      // Communications errors
   case cdCOMM_PORT_IS_IN_USE: return _T("Comm port in use");
   case cdCOMM_DISCONNECTED: return _T("Comm disconnected");
   case cdCOMM_DEVICE_INCOMPATIBLE: return _T("Comm device incompatible");
   case cdCOMM_BUFFER_FULL: return _T("Comm buffer full");
   case cdCOMM_USB_BUS_ERR: return _T("Comm USB bus error");

      // Lock/Unlock
   case cdUSB_DEVICE_LOCK_ERROR: return _T("USB device lock error");
   case cdUSB_DEVICE_UNLOCK_ERROR: return _T("USB device unlock error");

      // STI/WIA
   case cdSTI_UNKNOWN_ERROR: return _T("STI unknown error");
   case cdSTI_INTERNAL_ERROR: return _T("STI internal error");
   case cdSTI_DEVICE_CREATE_ERROR: return _T("STI device created error");
   case cdSTI_DEVICE_RELEASE_ERROR: return _T("STI device released error");
   case cdDEVICE_NOT_LAUNCHED: return _T("Device not launched");

   case cdENUM_NA: return _T("Enum N/A");
   case cdINVALID_FN_CALL: return _T("Invalid function call");
   case cdHANDLE_NOT_FOUND: return _T("Handle not found");
   case cdINVALID_ID: return _T("Invalid ID");
   case cdWAIT_TIMEOUT_ERROR: return _T("Wait timeout error");

      // PTP
   case prRESPONSE_Undefined: return _T("PTP: Undefined");
   case prRESPONSE_GeneralError: return _T("PTP: General error");
   case cdSESSION_NOT_OPEN: return _T("PTP: Session not open");
   case cdINVALID_TRANSACTIONID: return _T("PTP: Invalid transaction ID");
   case prRESPONSE_OperationNotSupported: return _T("PTP: Operation not supported");
   case prRESPONSE_ParameterNotSupported: return _T("PTP: Parameter not supported");
   case cdINCOMPLETE_TRANSFER: return _T("PTP: Incomplete transfer");
   case cdINVALID_STRAGEID: return _T("PTP: Invalid stage ID");
   case prRESPONSE_InvalidObjectHandle: return _T("PTP: Invalid object handle");
   case cdDEVICEPROP_NOT_SUPPORTED: return _T("PTP: Device property not supported");
   case cdINVALID_OBJECTFORMATCODE: return _T("PTP: Invalid object format code");
   case prRESPONSE_StoreFull: return _T("PTP: Store full");
   case prRESPONSE_ObjectWriteProtected: return _T("PTP: Object write protected");
   case prRESPONSE_StoreRead_Only: return _T("PTP: Store read-only");
   case prRESPONSE_AccessDenied: return _T("PTP: Access denied");
   case prRESPONSE_NoThumbnailPresent: return _T("PTP: No thumbnail present");
   case cdSELF_TEST_FAILED: return _T("PTP: Self test failed");
   case cdPARTIAL_DELETION: return _T("PTP: Partial deletion");
   case prRESPONSE_StoreNotAvailable: return _T("PTP: Store not available");
   case cdSPECIFICATION_BY_FORMAT_UNSUPPORTED: return _T("PTP: Specification by format unsupported");
   case cdNO_VALID_OBJECTINFO: return _T("PTP: No valid object info");
   case cdINVALID_CODE_FORMAT: return _T("PTP: Invalid code format");
   case cdUNKNOWN_VENDER_CODE: return _T("PTP: Unknown vendor code");
   case cdCAPTURE_ALREADY_TERMINATED: return _T("PTP: Capture already terminated");
   case prRESPONSE_DeviceBusy: return _T("PTP: Device busy");
   case cdINVALID_PARENTOBJECT: return _T("PTP: Invalid parent object");
   case cdINVALID_DEVICEPROP_FORMAT: return _T("PTP: Invalid device property format");
   case cdINVALID_DEVICEPROP_VALUE: return _T("PTP: Invalid device property value");
   case cdSESSION_ALREADY_OPEN: return _T("PTP: Session already open");
   case cdTRANSACTION_CANCELLED: return _T("PTP: Transaction cancelled");
   case cdSPECIFICATION_OF_DESTINATION_UNSUPPORTED: return _T("PTP: Specification of destination unsupported");
   case EDS_ERR_NOT_CAMERA_SUPPORT_SDK_VERSION: return _T("Camera not supported by SDK version");

      /// PTP vendor extensions
   case prRESPONSE_Ex_Undefined: return _T("PTP: Undefined");
   case cdUNKNOWN_COMMAND: return _T("PTP: Unknown command");
   case prRESPONSE_Ex_MemAllocFailed: return _T("PTP: Mem alloc failed");
   case prRESPONSE_Ex_InternalError: return _T("PTP: Internal error");
   case prRESPONSE_Ex_DirIOError: return _T("PTP: Dir IO error");
   case cdOPERATION_REFUSED: return _T("PTP: Operation refused");
   case cdLENS_COVER_CLOSE: return _T("PTP: Lens cover closed");
   case prRESPONSE_Ex_NoRelease: return _T("PTP: No release");
   case prRESPONSE_Ex_DeviceIsHot: return _T("PTP: Device is hot");
   case prRESPONSE_Ex_LowBattery: return _T("PTP: Low battery");
   case prRESPONSE_Ex_AlreadyExit: return _T("PTP: Already exited");
   case EDS_ERR_LOW_BATTERY: return _T("PTP: Low battery error");
   case EDS_ERR_OBJECT_NOTREADY: return _T("PTP: Image data set not ready for live view");
   case EDS_ERR_CANNOT_MAKE_OBJECT: return _T("PTP: Cannot make object");
   case EDS_ERR_MEMORYSTATUS_NOTREADY: return _T("PTP: Memory status not ready");

      // Take Picture errors
   case EDS_ERR_TAKE_PICTURE_AF_NG: return _T("Focus failed");
   case EDS_ERR_TAKE_PICTURE_RESERVED: return _T("Reserved");
   case EDS_ERR_TAKE_PICTURE_MIRROR_UP_NG: return _T("Currently configuring mirror up");
   case EDS_ERR_TAKE_PICTURE_SENSOR_CLEANING_NG: return _T("Currently cleaning sensor");
   case EDS_ERR_TAKE_PICTURE_SILENCE_NG: return _T("Currently performing silent operation");
   case EDS_ERR_TAKE_PICTURE_NO_CARD_NG: return _T("Card not installed");
   case EDS_ERR_TAKE_PICTURE_CARD_NG: return _T("Error writing to card");
   case EDS_ERR_TAKE_PICTURE_CARD_PROTECT_NG: return _T("Card write protected");
   case EDS_ERR_TAKE_PICTURE_MOVIE_CROP_NG: return _T("Movie crop");
   case EDS_ERR_TAKE_PICTURE_STROBO_CHARGE_NG: return _T("Strobo change");
   case EDS_ERR_TAKE_PICTURE_NO_LENS_NG: return _T("No lens");
   case EDS_ERR_TAKE_PICTURE_SPECIAL_MOVIE_MODE_NG: return _T("Special movie mode");

   default:
      return _T("???");
   }
}
