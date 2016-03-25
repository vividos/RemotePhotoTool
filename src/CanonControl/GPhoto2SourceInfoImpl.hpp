//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2SourceInfoImpl.hpp gPhoto2 - SourceInfo impl
//
#pragma once

// includes
#include "GPhoto2Common.hpp"
#include "SourceInfo.hpp"

namespace GPhoto2
{

/// implementation of SourceInfo for gPhoto2 access
class SourceInfoImpl : public SourceInfo
{
public:
   /// ctor
   SourceInfoImpl(
      std::shared_ptr<_GPContext> spContext,
      const CString& name,
      const CString& port)
      :m_spContext(spContext),
      m_name(name),
      m_port(port)
   {
   }

   // SourceInfo virtual functions

   virtual CString Name() const override { return m_name + _T(" (gPhoto2)"); }

   virtual CString DeviceId() const override { return m_port; }

   virtual std::shared_ptr<SourceDevice> Open() override;

private:
   /// gPhoto2 context
   std::shared_ptr<_GPContext> m_spContext;

   /// camera name
   CString m_name;

   /// camera port
   CString m_port;
};

}
