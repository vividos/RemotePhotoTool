//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2SourceInfoImpl.hpp gPhoto2 - SourceInfo impl
//
#pragma once

#include "GPhoto2Common.hpp"
#include "SourceInfo.hpp"

struct _Camera;

namespace GPhoto2
{
   /// implementation of SourceInfo for gPhoto2 access
   class SourceInfoImpl : public SourceInfo
   {
   public:
      /// ctor
      SourceInfoImpl(
         RefSp ref,
         const CString& name,
         const CString& port)
         :m_ref(ref),
         m_name(name),
         m_port(port)
      {
      }

      // SourceInfo virtual functions

      virtual CString Name() const override { return m_name + _T(" [libgphoto2]"); }

      virtual CString DeviceId() const override { return m_port; }

      virtual std::shared_ptr<SourceDevice> Open() override;

   private:
      /// initializes camera from this source info object
      void InitCamera(std::shared_ptr<_Camera> camera);

   private:
      /// gPhoto2 reference
      RefSp m_ref;

      /// camera name
      CString m_name;

      /// camera port
      CString m_port;
   };

} // namespace GPhoto2
