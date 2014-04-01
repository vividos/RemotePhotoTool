//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ErrorText.cpp Canon control - Error texts
//

// includes
#include "stdafx.h"
#include "ErrorText.hpp"
#include "CdsdkCommon.hpp"

LPCTSTR ErrorTextFromErrorId(unsigned int uiErrorId)
{
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

      // File errors omitted
      // Directory errors omitted

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

      // Stream errors omitted

      // Communications errors
   case cdCOMM_PORT_IS_IN_USE: return _T("Comm port in use");
   case cdCOMM_DISCONNECTED: return _T("Comm disconnected");
   case cdCOMM_DEVICE_INCOMPATIBLE: return _T("Comm device incompatible");
   case cdCOMM_BUFFER_FULL: return _T("Comm buffer full");
   case cdCOMM_USB_BUS_ERR: return _T("Comm USB bus error");

      // Lock/Unlock
   case cdUSB_DEVICE_LOCK_ERROR: return _T("");
   case cdUSB_DEVICE_UNLOCK_ERROR: return _T("");

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
   case cdSESSION_NOT_OPEN: return _T("PTP: Session not open");
   case cdINVALID_TRANSACTIONID: return _T("PTP: Invalid transaction ID");
   case cdINCOMPLETE_TRANSFER: return _T("PTP: Incomplete transfer");
   case cdINVALID_STRAGEID: return _T("PTP: Invalid stage ID");
   case cdDEVICEPROP_NOT_SUPPORTED: return _T("PTP: Device property not supported");
   case cdINVALID_OBJECTFORMATCODE: return _T("PTP: Invalid object format code");
   case cdSELF_TEST_FAILED: return _T("PTP: Self test failed");
   case cdPARTIAL_DELETION: return _T("PTP: Partial deletion");
   case cdSPECIFICATION_BY_FORMAT_UNSUPPORTED: return _T("PTP: Specification by format unsupported");
   case cdNO_VALID_OBJECTINFO: return _T("PTP: No valid object info");
   case cdINVALID_CODE_FORMAT: return _T("PTP: Invalid code format");
   case cdUNKNOWN_VENDER_CODE: return _T("PTP: Unknown vendor code");
   case cdCAPTURE_ALREADY_TERMINATED: return _T("PTP: Capture already terminated");
   case cdINVALID_PARENTOBJECT: return _T("PTP: Invalid parent object");
   case cdINVALID_DEVICEPROP_FORMAT: return _T("PTP: Invalid device property format");
   case cdINVALID_DEVICEPROP_VALUE: return _T("PTP: Invalid device property value");
   case cdSESSION_ALREADY_OPEN: return _T("PTP: Session already open");
   case cdTRANSACTION_CANCELLED: return _T("PTP: Transaction cancelled");
   case cdSPECIFICATION_OF_DESTINATION_UNSUPPORTED: return _T("PTP: Specification of destination unsupported");
   case cdUNKNOWN_COMMAND: return _T("PTP: Unknown command");
   case cdOPERATION_REFUSED: return _T("PTP: Operation refused");
   case cdLENS_COVER_CLOSE: return _T("PTP: Lens cover close");
   default:
      return _T("???");
   }
}
