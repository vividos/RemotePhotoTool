//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file RemotePhotoToolCmdline\stdafx.cpp Precompiled header support
//

// includes
#include "stdafx.h"
#include "CmdlineApp.hpp"

#if 0
void CheckUnknownDeviceProperties(std::shared_ptr<SourceDevice> spSourceDevice)
{
   for (unsigned int propId = 0; propId < 0x1000; propId++)
   {
      try
      {
         DeviceProperty dp = spSourceDevice->GetDeviceProperty(propId);

         _tprintf(_T("Id [%04x] Name [%s] Value [%s]\n"),
            propId,
            dp.Name(),
            dp.AsString()
            );
      }
      catch(...)
      {
         continue;
      }
   }
}

void CheckUnknownImageProperties(std::shared_ptr<RemoteReleaseControl> spReleaseControl)
{
//   std::shared_ptr<Viewfinder> spViewfinder = spReleaseControl->StartViewfinder();

   for (unsigned int propId = 0; propId < 0x1000; propId++)
   {
      try
      {
         ImageProperty ip = spReleaseControl->GetImageProperty(propId);

         _tprintf(_T("Id [%04x] Name [%s] Value [%s]\n"),
            propId,
            ip.Name(),
            ip.AsString()
            );
      }
      catch(...)
      {
         continue;
      }
   }
}

void ListRemoteCapabilities(std::shared_ptr<RemoteReleaseControl> spReleaseControl)
{
   _tprintf(_T("List remote capture capabilites\n"));

   struct SRemoteCapList
   {
      RemoteReleaseControl::T_enRemoteCapability enCapability;
      LPCTSTR pszCapabilityName;
   };

   SRemoteCapList s_capList[] =
   {
      { RemoteReleaseControl::capChangeShootingParameter, _T("can change shooting parameter at all") },
      { RemoteReleaseControl::capChangeShootingMode,      _T("ability to change shooting modes") },
      { RemoteReleaseControl::capZoomControl,             _T("can control zoom") },
      { RemoteReleaseControl::capViewfinder,              _T("can fetch live view image") },
      { RemoteReleaseControl::capReleaseWhileViewfinder,  _T("can capture during live view") },
      { RemoteReleaseControl::capAFLock,                  _T("supports AF lock/unlock") },
      { RemoteReleaseControl::capBulbMode,                _T("supports bulb mode") },
   };

   for (unsigned int i=0; i<sizeof(s_capList)/sizeof(*s_capList); i++)
   {
      bool bSupported = spReleaseControl->GetCapability(s_capList[i].enCapability);

      _tprintf(_T("%s: %s\n"), s_capList[i].pszCapabilityName, bSupported ? _T("yes"): _T("no"));
   }

   _tprintf(_T("\n"));
}
#endif

/// command line app main function
int _tmain(int argc, _TCHAR* argv[])
{
   CmdlineApp app;
   app.Run(argc, argv);

   return 0;
}
