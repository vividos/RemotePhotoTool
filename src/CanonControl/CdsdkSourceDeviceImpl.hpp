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
   SourceDeviceImpl(RefSp spRef, cdHSource hSource)
      :m_spRef(spRef),
       m_hSource(hSource)
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
      try
      {
         switch (enDeviceCapability)
         {
         case SourceDevice::capRemoteReleaseControl:
            {
               DeviceProperty p = GetDeviceProperty(cdDEVICE_PROP_RELEASE_CONTROL_CAP);
               unsigned int uiValue = p.Value().Get<unsigned int>();

               return (uiValue & 0x40000000) != 0;
            }
            break;

         case SourceDevice::capRemoteViewfinder:
            // TODO implement
            ATLASSERT(false);
            break;

         default:
            ATLASSERT(false);
         }
      }
      catch (Exception& ex)
      {
         LOG_TRACE(_T("Exception occured in GetDeviceCapability(): %s\n"),
            ex.Message().GetString());
      }
      catch (boost::bad_any_cast&)
      {
         LOG_TRACE(_T("boost::bad_any_cast exception occured in GetDeviceCapability()\n"));
      }
      catch (std::exception& ex)
      {
         LOG_TRACE(_T("std::exception occured in GetDeviceCapability(): %Hs\n"),
            ex.what());
      }
      return false;
   }

   virtual CString ModelName() const override
   {
      try
      {
         DevicePropertyAccess p(const_cast<cdHSource&>(m_hSource));
         Variant v = p.Get(cdDEVICE_PROP_MODEL_NAME);
         return v.Get<CString>();
      }
      catch (Exception& ex)
      {
         LOG_TRACE(_T("exception in ModelName(): %s"), ex.Message().GetString());
      }

      return CString();
   }

   virtual CString SerialNumber() const override
   {
      return CString();
   }

   virtual std::vector<unsigned int> EnumDeviceProperties() const override
   {
      // start enumerating
      cdHEnum hEnum = 0;

      // may return cdINVALID_PARAMETER, cdINVALID_HANDLE
      cdError err = CDEnumDevicePropertyReset(m_hSource, 0, &hEnum);
      LOG_TRACE(_T("CDEnumDevicePropertyReset(%08x) returned %08x\n"), m_hSource, err);
      CheckError(_T("CDEnumDeviceReset"), err, __FILE__, __LINE__);

      // get count of properties
      cdUInt32 count = 0;

      // returns cdINVALID_HANDLE, cdINVALID_PARAMETER
      err = CDGetDevicePropertyCount(hEnum, &count);
      LOG_TRACE(_T("CDGetDevicePropertyCount(%08x, &count = %u) returned %08x\n"), hEnum, count, err);
      CheckError(_T("CDGetDevicePropertyCount"), err, __FILE__, __LINE__);

      std::vector<unsigned int> vecData;
      for (cdUInt32 i=0; i<count+1; i++)
      {
         cdDevicePropertyStruct deviceProperty;

         // may return cdINVALID_PARAMETER, cdINVALID_HANDLE, cdENUM_NA
         err = CDEnumDevicePropertyNext(hEnum, &deviceProperty);
         LOG_TRACE(_T("CDEnumDevicePropertyNext(%08x, &propId = %08x) returned %08x\n"), hEnum, deviceProperty.DevPropID, err);
         if ((err & cdERROR_ERRORID_MASK) == cdENUM_NA)
            break;

         CheckError(_T("CDEnumDevicePropertyNext"), err, __FILE__, __LINE__);

         vecData.push_back(deviceProperty.DevPropID);
      }

      // end enumerating
      // may return cdINVALID_HANDLE, cdINVALID_FN_CALL
      err = CDEnumDevicePropertyRelease(hEnum);
      LOG_TRACE(_T("CDEnumDevicePropertyRelease(%08x) returned %08x\n"), hEnum, err);
      CheckError(_T("CDEnumDevicePropertyRelease"), err, __FILE__, __LINE__);

      return vecData;
   }

   virtual DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const override
   {
      DevicePropertyAccess access(GetSource());
      Variant value = access.Get(uiPropertyId);

      return DeviceProperty(variantCdsdk, uiPropertyId, value, true);
   }

   virtual std::shared_ptr<RemoteReleaseControl> EnterReleaseControl() override
   {
      if (!GetDeviceCapability(capRemoteReleaseControl))
      {
         // throw a "not supported" error
         throw CameraException(_T("SourceDevice::EnterReleaseControl"), false,
            cdERROR_CDSDK_COMPONENTID, cdNOT_SUPPORTED, __FILE__, __LINE__);
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
