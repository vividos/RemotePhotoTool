//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkSourceInfoImpl.hpp CDSDK - SourceInfo impl
//
#pragma once

// includes
#include "SourceInfo.hpp"
#include "SourceDevice.hpp"
#include "CdsdkCommon.hpp"
#include "CdsdkSourceDeviceImpl.hpp"

namespace CDSDK
{

/// source info impl for CDSDK
class SourceInfoImpl: public SourceInfo
{
public:
   /// ctor
   SourceInfoImpl(RefSp spRef)
      :m_spRef(spRef)
   {
      ZeroMemory(&m_sourceInfo, sizeof(m_sourceInfo));
   }

   /// dtor
   virtual ~SourceInfoImpl() {}

   virtual CString Name() const override
   {
      return CString(m_sourceInfo.Name) + _T(" [CD-SDK]");
   }

   virtual CString DeviceId() const override
   {
      return m_sourceInfo.NameInOS;
   }

   virtual std::shared_ptr<SourceDevice> Open() override
   {
      cdHSource hSource = 0;

      LOG_TRACE(_T("about to call CDOpenSource...\n"));

      // may return cdINVALID_PARAMETER, cdMEM_ALLOC_FAILED, cdDEVICE_NOT_FOUND, cdDEVICE_NOT_INSTALLED
      cdError err = CDOpenSource(&m_sourceInfo, &hSource);
      LOG_TRACE(_T("CDOpenSource(&si = \"%hs\", hSource=%08x) returned %08x\n"),
         m_sourceInfo.Name, hSource, err);
      CheckError(_T("CDOpenSource"), err, __FILE__, __LINE__);

      return std::shared_ptr<SourceDevice>(new SourceDeviceImpl(m_spRef, hSource, Name()));
   }

   /// returns source info
   cdSourceInfo& GetSourceInfo() { return m_sourceInfo; }

private:
   /// source info
   cdSourceInfo m_sourceInfo;

   /// SDK ref
   RefSp m_spRef;
};

} // namespace EDSDK
