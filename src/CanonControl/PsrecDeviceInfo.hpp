//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecDeviceInfo.hpp PS-ReC - device info
//
#pragma once

// includes
#include "PsrecCommon.hpp"
#include "PsrecVarDataParser.hpp"
#include "PsrecPropertyAccess.hpp"
#include <set>

namespace PSREC
{

/// access to camera device info
struct DeviceInfo: public VarDataParser
{
   /// ctor; reads device info
   DeviceInfo(prHandle hCamera)
   {
      // get size of buffer
      prUInt32 uiSize = 0;
      prResponse err = PR_GetDeviceInfo(hCamera, &uiSize, nullptr);
      if ((err & prERROR_ERRORID_MASK) != prINSUFFICIENT_BUFFER)
      {
         LOG_TRACE(_T("PR_GetDeviceInfo(handle = %08x, 0, nullptr) returned %08x, buffer size %u\n"),
            hCamera, err, uiSize);
         CheckError(_T("PR_GetDeviceInfo"), err, __FILE__, __LINE__);
      }

      if (uiSize == 0)
         return; // no data available

      // read data bytes
      std::vector<BYTE>& vecBuffer = GetBuffer();
      vecBuffer.resize(uiSize);
      prVoid* pDeviceInfo = reinterpret_cast<prVoid*>(&vecBuffer[0]);

      // may return prINVALID_FN_CALL, prINVALID_HANDLE, prINVALID_PARAMETER, prMEM_ALLOC_FAILED,
      // prINSUFFICIENT_BUFFER
      err = PR_GetDeviceInfo(hCamera, &uiSize, pDeviceInfo);
      LOG_TRACE(_T("PR_GetDeviceInfo(handle = %08x, size = %u, buffer) returned %08x\n"),
         hCamera, uiSize, err);
      CheckError(_T("PR_GetDeviceInfo"), err, __FILE__, __LINE__);

      Parse();
   }

   /// parses device info
   void Parse()
   {
      m_uiStandardVersion = ReadUint16();
      m_uiVendorExtensionID = ReadUint32();
      m_uiVendorExtensionVersion = ReadUint16();
      m_cszVendorExtensionDesc = ReadString();
      m_uiFunctionMode = ReadUint16();

      ReadUint16ArrayAsSet(m_setOperationsSupported);
      ReadUint16ArrayAsSet(m_setEventsSupported);
      ReadUint16ArrayAsSet(m_setDevicePropertiesSupported);
      ReadUint16ArrayAsSet(m_setCaptureFormats);
      ReadUint16ArrayAsSet(m_setImageFormats);

      m_cszManufacturer = ReadString();
      m_cszModel = ReadString();
      m_cszDeviceVersion = ReadString();
      m_cszSerialNumber = ReadString();
   }

   prUInt16 m_uiStandardVersion;    ///< version
   prUInt32 m_uiVendorExtensionID;  ///< vendor extension id
   prUInt16 m_uiVendorExtensionVersion; ///< vendor extension version
   prUInt16 m_uiFunctionMode;       ///< function mode

   CString m_cszVendorExtensionDesc;///< vendor extension description
   CString m_cszManufacturer;       ///< manufacturer
   CString m_cszModel;              ///< model
   CString m_cszDeviceVersion;      ///< device version
   CString m_cszSerialNumber;       ///< serial number

   std::set<prUInt16> m_setOperationsSupported; ///< all supported operations
   std::set<prUInt16> m_setEventsSupported;     ///< all supported events
   std::set<prUInt16> m_setDevicePropertiesSupported; ///< all supported device properties
   std::set<prUInt16> m_setCaptureFormats;      ///< all supported capture formats
   std::set<prUInt16> m_setImageFormats;        ///< all supported image formats
};

} // namespace PSREC
