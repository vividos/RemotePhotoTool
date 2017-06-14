//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkSourceDeviceImpl.hpp CDSDK - SourceDevice impl
//
#pragma once

// includes
#include "SourceDevice.hpp"
#include "CdsdkCommon.hpp"
#include "CdsdkRemoteReleaseControlImpl.hpp"
#include "CdsdkViewfinderImpl.hpp"
#include "CdsdkDevicePropertyAccess.hpp"

namespace CDSDK
{

/// source device impl for CDSDK
class SourceDeviceImpl:
   public SourceDevice,
   public std::enable_shared_from_this<SourceDeviceImpl>
{
public:
   /// ctor
   SourceDeviceImpl(RefSp spRef, cdHSource hSource, const CString& cszModelName)
      :m_spRef(spRef),
       m_hSource(hSource),
       m_cszModelName(cszModelName)
   {
      // It seems that for some cameras (e.g. my PowerShot G2) it is necessary to enumerate device
      // properties once before reading release control faculty value.
      EnumDeviceProperties();
      ReadReleaseControlFaculty();
   }

   /// dtor
   virtual ~SourceDeviceImpl() throw()
   {
      cdError err = CDCloseSource(m_hSource);
      LOG_TRACE(_T("CDCloseSource(%08x) returned %08x\n"), m_hSource, err);
   }

   virtual bool GetDeviceCapability(SourceDevice::T_enDeviceCapability enDeviceCapability) const override
   {
      switch (enDeviceCapability)
      {
      case SourceDevice::capRemoteReleaseControl:
         return (m_faculty & cdRELEASE_CONTROL_CAP_SUPPORT) != 0;

      case SourceDevice::capRemoteViewfinder:
         return (m_faculty & cdRELEASE_CONTROL_CAP_VIEWFINDER) != 0;

      case SourceDevice::capCameraFileSystem:
         return false; // not implemented yet

      default:
         ATLASSERT(false);
         break;
      }

      return false;
   }

   virtual CString ModelName() const override
   {
      return m_cszModelName;
   }

   virtual CString SerialNumber() const override
   {
      // there's no serial number property on CDSDK
      return _T("N/A");
   }

   virtual std::vector<unsigned int> EnumDeviceProperties() const override
   {
      // start enumerating
      cdHEnum hEnum = 0;

      // may return cdINVALID_PARAMETER, cdINVALID_HANDLE
      cdError err = CDEnumDevicePropertyReset(m_hSource, 0, &hEnum);
      if (err != cdOK) LOG_TRACE(_T("CDEnumDevicePropertyReset(%08x) returned %08x\n"), m_hSource, err);
      CheckError(_T("CDEnumDeviceReset"), err, __FILE__, __LINE__);

      // get count of properties
      cdUInt32 count = 0;

      // returns cdINVALID_HANDLE, cdINVALID_PARAMETER
      err = CDGetDevicePropertyCount(hEnum, &count);
      if (err != cdOK) LOG_TRACE(_T("CDGetDevicePropertyCount(%08x, &count = %u) returned %08x\n"), hEnum, count, err);
      CheckError(_T("CDGetDevicePropertyCount"), err, __FILE__, __LINE__);

      std::vector<unsigned int> vecData;
      for (cdUInt32 i=0; i<count+1; i++)
      {
         cdDevicePropertyStruct deviceProperty;

         // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdENUM_NA
         err = CDEnumDevicePropertyNext(hEnum, &deviceProperty);
         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break;

         if (err != cdOK) LOG_TRACE(_T("CDEnumDevicePropertyNext(%08x, &propId = %08x \"%s\") returned %08x\n"),
            hEnum, deviceProperty.DevPropID,
            DevicePropertyAccess::NameFromId(deviceProperty.DevPropID),
            err);

         CheckError(_T("CDEnumDevicePropertyNext"), err, __FILE__, __LINE__);

         LOG_TRACE(_T("Available device property: \"%s\" (%08x)\n"),
            DevicePropertyAccess::NameFromId(deviceProperty.DevPropID),
            deviceProperty.DevPropID);

         vecData.push_back(deviceProperty.DevPropID);
      }

      // end enumerating
      // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
      err = CDEnumDevicePropertyRelease(hEnum);
      if (err != cdOK) LOG_TRACE(_T("CDEnumDevicePropertyRelease(%08x) returned %08x\n"), hEnum, err);
      CheckError(_T("CDEnumDevicePropertyRelease"), err, __FILE__, __LINE__);

      return vecData;
   }

   virtual DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const override
   {
      DevicePropertyAccess access(GetSource());
      Variant value = access.Get(uiPropertyId);

      bool bReadWrite =
         uiPropertyId == cdDEVICE_PROP_TIME ||
         uiPropertyId == cdDEVICE_PROP_OWNER_NAME;

      return DeviceProperty(variantCdsdk, uiPropertyId, value, !bReadWrite);
   }

   virtual std::shared_ptr<CameraFileSystem> GetFileSystem() override
   {
      if (!GetDeviceCapability(capCameraFileSystem))
      {
         throw CameraException(_T("CDSDK::SourceDevice::GetFileSystem"), _T("Not supported"),
            cdERROR_CDSDK_COMPONENTID | cdNOT_SUPPORTED, __FILE__, __LINE__);
      }

      return std::shared_ptr<CameraFileSystem>();
   }

   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override
   {
      if (!GetDeviceCapability(capRemoteReleaseControl))
      {
         throw CameraException(_T("CDSDK::SourceDevice::EnterReleaseControl"), _T("Not supported"),
            cdERROR_CDSDK_COMPONENTID | cdNOT_SUPPORTED, __FILE__, __LINE__);
      }

      std::shared_ptr<SourceDeviceImpl> spSourceDevice = shared_from_this();

      return std::shared_ptr<RemoteReleaseControl>(new RemoteReleaseControlImpl(spSourceDevice));
   }

   /// reads release control faculty value
   void ReadReleaseControlFaculty()
   {
      m_faculty = 0;
      try
      {
         // Getting the release control faculty doesn't seem to work some times, before CDEnterReleaseControl() is
         // called. So we try CDGetReleaseControlFaculty first, then we try to get the device property.
         cdError err = CDGetReleaseControlFaculty(GetSource(), &m_faculty);
         LOG_TRACE(_T("CDGetReleaseControlFaculty(%08x, &faculty = %08x) returned %08x\n"), GetSource(), m_faculty, err);

         if (err != cdOK)
         {
            DevicePropertyAccess access(GetSource());
            m_faculty = access.Get(cdDEVICE_PROP_RELEASE_CONTROL_CAP).Get<unsigned int>();
         }
      }
      catch (const CameraException& ex)
      {
         LOG_TRACE(_T("CameraException occured in ReadReleaseControlFaculty(): %s\n"), ex.Message().GetString());
         m_faculty = 0;
      }
      catch (...)
      {
         LOG_TRACE(_T("exception occured in ReadReleaseControlFaculty()\n"));
         m_faculty = 0;
      }
   }

   /// returns source
   cdHSource GetSource() const throw() { return m_hSource; }

private:
   /// source device handle
   cdHSource m_hSource;

   /// SDK ref
   RefSp m_spRef;

   /// model name
   CString m_cszModelName;

   /// release control faculty
   cdReleaseControlFaculty m_faculty;
};

inline cdHSource RemoteReleaseControlImpl::GetSource() const throw()
{
   return m_spSourceDevice->GetSource();
}

inline cdHSource ViewfinderImpl::GetSource() const throw()
{
   return m_spSourceDevice->GetSource();
}

} // namespace CDSDK
