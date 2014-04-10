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
   }

   /// dtor
   virtual ~SourceDeviceImpl() throw()
   {
      cdError err = CDCloseSource(m_hSource);
      LOG_TRACE(_T("CDCloseSource(%08x) returned %08x\n"), m_hSource, err);
   }

   virtual bool GetDeviceCapability(SourceDevice::T_enDeviceCapability enDeviceCapability) const override
   {
      cdReleaseControlFaculty faculty = 0;
      try
      {
         cdError err = CDGetReleaseControlFaculty(GetSource(), &faculty);
         LOG_TRACE(_T("CDGetReleaseControlFaculty(%08x, &faculty = %08x) returned %08x\n"), GetSource(), faculty, err);
         CheckError(_T("CDGetReleaseControlFaculty"), err, __FILE__, __LINE__);
      }
      catch (const CameraException& ex)
      {
         LOG_TRACE(_T("CameraException occured in GetDeviceCapability(): %s\n"), ex.Message().GetString());
      }
      catch (...)
      {
         LOG_TRACE(_T("exception occured in GetDeviceCapability()\n"));
      }

      switch (enDeviceCapability)
      {
      case SourceDevice::capRemoteReleaseControl:
         return (faculty & cdRELEASE_CONTROL_CAP_SUPPORT) != 0;

      case SourceDevice::capRemoteViewfinder:
         return (faculty & cdRELEASE_CONTROL_CAP_VIEWFINDER) != 0;

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
            DevicePropertyAccess::NameFromId(deviceProperty.DevPropID).GetString(),
            err);

         CheckError(_T("CDEnumDevicePropertyNext"), err, __FILE__, __LINE__);

         LOG_TRACE(_T("Available device property: \"%s\" (%08x)\n"),
            DevicePropertyAccess::NameFromId(deviceProperty.DevPropID).GetString(),
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

   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override
   {
      if (!GetDeviceCapability(capRemoteReleaseControl))
      {
         throw CameraException(_T("EnterReleaseControl"), _T("Not supported"),
            cdERROR_CDSDK_COMPONENTID | cdNOT_SUPPORTED, __FILE__, __LINE__);
      }

      std::shared_ptr<SourceDeviceImpl> spSourceDevice = shared_from_this();

      return std::shared_ptr<RemoteReleaseControl>(new RemoteReleaseControlImpl(spSourceDevice));
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
