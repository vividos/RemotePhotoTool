//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file TestCameraScriptProcessor.cpp Tests for CameraScriptProcessor class
//

// includes
#include "stdafx.h"
#include "CppUnitTest.h"
#include "CameraScriptProcessor.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LuaScriptingUnitTest
{
   /// tests CameraScriptProcessor class
   TEST_CLASS(TestCameraScriptProcessor)
   {
   public:
      /// tests default ctor
      TEST_METHOD(TestDefaultCtor)
      {
         CameraScriptProcessor csp;
      }

      /// tests default ctor
      TEST_METHOD(TestStartStop)
      {
         CameraScriptProcessor csp;
         csp.LoadScript(_T("C:\\Users\\Michael\\SkyDrive\\Dokumente\\RemotePhotoTool.lua"));
         csp.Run();
      }
   };
}
