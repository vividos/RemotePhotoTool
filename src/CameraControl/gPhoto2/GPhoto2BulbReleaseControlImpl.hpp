//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2BulbReleaseControlImpl.hpp gPhoto2 - Bulb release control impl
//
#pragma once

#include "GPhoto2Common.hpp"
#include "BulbReleaseControl.hpp"
#include <ulib/Timer.hpp>

namespace GPhoto2
{
   class PropertyAccess;

   /// implementation of BulbReleaseControl for gPhoto2 access
   class BulbReleaseControlImpl : public BulbReleaseControl
   {
   public:
      /// ctor
      BulbReleaseControlImpl(RefSp ref, std::shared_ptr<PropertyAccess> properties);
      /// dtor
      virtual ~BulbReleaseControlImpl();

      // BulbReleaseControl virtual functions

      /// returns elapsed time, in seconds, since bulb start
      virtual double ElapsedTime() const override;

      /// stops bulb method; can be used when the shared_ptr of BulbReleaseControl
      /// cannot be destroyed, e.g. since it is held somewhere (e.g. Lua)
      virtual void Stop() override;

   private:
      /// gPhoto2 reference
      RefSp m_ref;

      /// property manager for gPhoto connected camera
      std::shared_ptr<PropertyAccess> m_properties;

      /// bulb timer
      Timer m_bulbTimer;
   };

} // namespace GPhoto2
