//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file CanonEOSShutterCount.cpp Shows shutter count for connected Canon EOS camera
//

// includes
#include "stdafx.h"
#include "EDSDK/EdsdkShutterCounterReader.hpp"

/// main function
int _tmain(int, LPCTSTR[])
{
   try
   {
      _ftprintf(stderr, _T("CanonEOSShutterCount - Shows shutter count for connected Canon EOS camera\n\n"));

      EDSDK::ShutterCounterReader reader;
      unsigned int shutterCounter = 0;
      if (reader.Read("", shutterCounter))
      {
         _ftprintf(stdout, _T("Shutter counter for connected camera is: %i\n"),
            shutterCounter);

         return 0;
      }
      else
      {
         _ftprintf(stderr, _T("Couldn't read shutter counter.\n"));

         return -2;
      }
   }
   catch (...)
   {
      // exception while running
      _ftprintf(stderr, _T("Exception while running CanonEOSShutterCount\n"));
      return -1;
   }
}
