//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ShootingMode.hpp Canon control - Shooting mode
//
#pragma once

// includes
#include "RemoteReleaseControl.hpp"

/// shooting mode helper class
class ShootingMode
{
public:
   /// ctor
   ShootingMode(std::shared_ptr<RemoteReleaseControl> spRemoteReleaseControl)
      :m_spRemoteReleaseControl(spRemoteReleaseControl)
   {
   }

   /// returns if changing shooting mode is supported
   bool IsSupportedChanging() const throw()
   {
      return m_spRemoteReleaseControl->GetCapability(RemoteReleaseControl::capChangeShootingMode);
   }

   /// returns current shooting mode
   ImageProperty Current()
   {
      unsigned int uiShootingModePropertyId =
         m_spRemoteReleaseControl->MapImagePropertyTypeToId(T_enImagePropertyType::propShootingMode);

      return m_spRemoteReleaseControl->GetImageProperty(uiShootingModePropertyId);
   }

   /// returns mode P
   ImageProperty Program() { return m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeP); }
   /// returns mode Tv
   ImageProperty Tv() { return m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeTv); }
   /// returns mode Av
   ImageProperty Av() { return m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeAv); }
   /// returns mode M
   ImageProperty Manual() { return m_spRemoteReleaseControl->MapShootingModeToImagePropertyValue(RemoteReleaseControl::shootingModeM); }

private:
   /// release control
   std::shared_ptr<RemoteReleaseControl> m_spRemoteReleaseControl;
};
