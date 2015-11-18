//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file TestCameraScriptProcessor.cpp Tests for CameraScriptProcessor class
//

// includes
#include "stdafx.h"
#include "CppUnitTest.h"
#include "CameraScriptProcessor.hpp"
#include "LuaScheduler.hpp"
#include "Event.hpp"

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

         Assert::IsTrue(csp.GetScheduler().CurrentExecutionState() == LuaScheduler::stateIdle,
            _T("current execution state must be idle"));
      }

      /// tests running without script loaded
      TEST_METHOD(TestEmpty)
      {
         CameraScriptProcessor csp;

         // run
         csp.Run();

         // check
         Assert::IsTrue(csp.GetScheduler().CurrentExecutionState() == LuaScheduler::stateIdle,
            _T("current execution state must be idle"));
      }

      /// tests Run() and Stop() methods
      TEST_METHOD(TestRunStop)
      {
         CameraScriptProcessor csp;
         csp.LoadSourceString(_T("App = { run = function() print(\"no-op\") end; }"));
         csp.Run();
         csp.Stop();
      }

      /// tests running script multiple times
      TEST_METHOD(TestRunningMultipleTimes)
      {
         CameraScriptProcessor csp;

         Event evtWaitIdle(true, false); // manual-reset
         csp.GetScheduler().SetExecutionStateChangedHandler([&](LuaScheduler::T_enExecutionState enExecutionState)
         {
            if (enExecutionState == LuaScheduler::stateIdle)
               evtWaitIdle.Set();
         });

         for (int i = 0; i < 100; i++)
         {
            csp.LoadSourceString(_T("App = { run = function() print(\"no-op\") end; }"));

            evtWaitIdle.Reset();

            csp.Run();

            Assert::IsTrue(evtWaitIdle.Wait(), _T("waiting must not fail"));

            csp.Stop();
         }
      }
   };
}
