//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file TestLuaState.cpp Tests for Lua::State class
//

// includes
#include "stdafx.h"
#include "CppUnitTest.h"
#include "Lua.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace LuaScriptingUnitTest
{
   /// tests Lua::State class
   TEST_CLASS(TestLuaState)
   {
   public:
      /// tests LoadSourceString()
      TEST_METHOD(TestLoadSourceString)
      {
         Lua::State state;

         state.LoadSourceString(_T("-- comment test"));
         state.LoadSourceString(_T("function run() return 42; end"));

         // TODO check that run() exists instead of running the script
         state.CallFunction(_T("run"));
      }

      /// tests error handling
      TEST_METHOD(TestStateErrorHandling)
      {
         Lua::State state;
         state.LoadSourceString(_T("function run() error(); end"));

         try
         {
            state.CallFunction(_T("run"));
         }
         catch (const Lua::Exception&)
         {
            // ok
            return;
         }
         catch(...)
         {
            Assert::Fail(_T("must throw Lua::Exception and nothing else"));
         }

         Assert::Fail(_T("must throw exception"));
      }

      /// tests CallFunction(), without return parameters
      TEST_METHOD(TestCallFunctionNoRet)
      {
         Lua::State state;

         state.LoadSourceString(_T("function run() return; end"));

         // TODO check that run() exists
         std::vector<Lua::Value> vecRetvals = state.CallFunction(_T("run"));
         Assert::AreEqual<size_t>(0, vecRetvals.size(), _T("must have returned zero return values"));
      }

      /// tests CallFunction(), with single return parameter
      TEST_METHOD(TestCallFunctionRetval)
      {
         Lua::State state;

         state.LoadSourceString(_T("function run() return 42; end"));

         // TODO check that run() exists
         std::vector<Lua::Value> vecRetvals = state.CallFunction(_T("run"), 1);
         Assert::AreEqual<size_t>(1, vecRetvals.size(), _T("must have returned one return value"));

         const Lua::Value& value = vecRetvals[0];
         Assert::IsTrue(Lua::Value::typeNumber == value.GetType(), _T("type must be number"));
         // TODO check value returned
      }

      /// tests CallFunction(), with multiple return parameters
      TEST_METHOD(TestCallFunctionMultipleRetvals)
      {
         Lua::State state;

         state.LoadSourceString(_T("function run() return 42, 42==6*7, \"123abc\", nil; end"));

         std::vector<Lua::Value> vecRetvals = state.CallFunction(_T("run"), 4);
         Assert::AreEqual<size_t>(4, vecRetvals.size(), _T("must have returned 4 return values"));

         Assert::IsTrue(Lua::Value::typeNumber == vecRetvals[0].GetType(), _T("type must be number"));
         Assert::IsTrue(Lua::Value::typeBoolean == vecRetvals[1].GetType(), _T("type must be boolean"));
         Assert::IsTrue(Lua::Value::typeString == vecRetvals[2].GetType(), _T("type must be string"));
         Assert::IsTrue(Lua::Value::typeNil == vecRetvals[3].GetType(), _T("type must be nil"));
      }
   };
}
