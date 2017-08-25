//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file TestSystemBindings.cpp Tests for Lua System bindings
//

// includes
#include "stdafx.h"
#include "CppUnitTest.h"
#include "CameraScriptProcessor.hpp"
#include "LuaScheduler.hpp"
#include <ulib/thread/Event.hpp>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LuaScriptingUnitTest
{
   /// tests Lua System bindings
   TEST_CLASS(TestSystemBindings)
   {
   public:
      /// tests waiting for event, without event being signaled
      TEST_METHOD(TestWaitEventWithoutSignal)
      {
         // set up
         CameraScriptProcessor csp;

         ManualResetEvent evtIsStarted(false);
         ManualResetEvent evtIsIdleAgain(false);

         csp.GetScheduler().SetExecutionStateChangedHandler([&](LuaScheduler::T_enExecutionState enExecutionState)
         {
            if (enExecutionState == LuaScheduler::stateRunning)
               evtIsStarted.Set();

            if (enExecutionState == LuaScheduler::stateIdle && evtIsStarted.Wait(0) == true)
               evtIsIdleAgain.Set();
         });

         csp.LoadSourceString(
            _T("testResult = nil;")
            _T("App = {")
            _T("  run = function(self)")
            _T("     local myEvent  = Sys:createEvent();")
            _T("     testResult = myEvent:wait(0.1);")
            _T("  end; }"));

         // run
         csp.Run();

         evtIsStarted.Wait();
         evtIsIdleAgain.Wait();

         // check
         Lua::State& state = csp.GetScheduler().GetState();

         Lua::Value testResult = state.GetValue(_T("testResult"));
         Assert::IsTrue(testResult.GetType() == Lua::Value::typeBoolean,
            _T("testResult must be of type boolean"));

         Assert::IsTrue(testResult.Get<bool>() == false,
            _T("result must be false (no event signaled during waiting)"));

         // cleanup
         csp.GetScheduler().SetExecutionStateChangedHandler(nullptr);
      }
   };
}
