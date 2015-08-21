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

         state.LoadSourceString(_T("function test() return 42, 42==6*7, \"123abc\", nil; end"));

         std::vector<Lua::Value> vecRetvals = state.CallFunction(_T("test"), 4);
         Assert::AreEqual<size_t>(4, vecRetvals.size(), _T("must have returned 4 return values"));

         Assert::IsTrue(Lua::Value::typeNumber == vecRetvals[0].GetType(), _T("type must be number"));
         Assert::IsTrue(Lua::Value::typeBoolean == vecRetvals[1].GetType(), _T("type must be boolean"));
         Assert::IsTrue(Lua::Value::typeString == vecRetvals[2].GetType(), _T("type must be string"));
         Assert::IsTrue(Lua::Value::typeNil == vecRetvals[3].GetType(), _T("type must be nil"));
      }

      TEST_METHOD(TestAddTable)
      {
         Lua::State state;

         {
            Lua::Table table = state.AddTable(_T("abc"));
         }

         state.LoadSourceString(_T("function test() return abc; end"));

         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1);
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::IsTrue(Lua::Value::typeTable == vecRetval[0].GetType(), _T("type must be table"));
      }

      TEST_METHOD(TestGetTable)
      {
         Lua::State state;

         {
            Lua::Table table1 = state.AddTable(_T("abc"));
         }

         Lua::Table table2 = state.GetTable(_T("abc"));
      }

      TEST_METHOD(TestTableAddValue)
      {
         Lua::State state;

         {
            Lua::Table table1 = state.AddTable(_T("abc"));
            table1.AddValue(_T("def"), Lua::Value(42.0));
         }

         {
            state.LoadSourceString(_T("function test() return abc.def; end"));

            std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1);
            Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

            const Lua::Value& val = vecRetval[0];
            Assert::IsTrue(Lua::Value::typeNumber == vecRetval[0].GetType(), _T("type must be number"));
            Assert::AreEqual(val.Get<double>(), 42.0, 1e-6, _T("value must be 42"));
         }
      }

      TEST_METHOD(TestTableAddFunction)
      {
         Lua::State state;

         // define a function to call back
         Lua::T_fnCFunction fn = [&] (const std::vector<Lua::Value>& vecParams) -> std::vector<Lua::Value>
         {
            if (vecParams.size() != 1)
               throw std::runtime_error("must pass exactly 1 value");

            if (Lua::Value::typeNumber != vecParams[0].GetType())
               throw std::runtime_error("must pass a number value");

            double d = vecParams[0].Get<double>();

            std::vector<Lua::Value> vecRetValues;

            vecRetValues.push_back(Lua::Value(d));
            vecRetValues.push_back(Lua::Value(d+d));
            vecRetValues.push_back(Lua::Value(d*d));

            return vecRetValues;
         };

         {
            Lua::Table table1 = state.AddTable(_T("abc"));
            table1.AddFunction("run", fn);
         }

         {
            state.LoadSourceString(_T("function test() return abc.run(42.0); end"));

            std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 3);

            Assert::AreEqual<size_t>(3, vecRetval.size(), _T("must have returned 3 return value"));

            const Lua::Value& val0 = vecRetval[0];
            Assert::IsTrue(Lua::Value::typeNumber == val0.GetType(), _T("type must be number"));
            Assert::AreEqual(val0.Get<double>(), 42.0, 1e-6, _T("value must be 42"));

            Assert::AreEqual(vecRetval[1].Get<double>(), 42.0+42.0, 1e-6, _T("value must be 42+42"));

            Assert::AreEqual(vecRetval[2].Get<double>(), 42.0*42.0, 1e-6, _T("value must be 42*42"));
         }
      }

      TEST_METHOD(TestTableCallFunction)
      {
         Lua::State state;

         // setup
         state.LoadSourceString(_T("abc = { def = function(self, val) return val, val+val, val*val end }"));

         // run
         Lua::Table table = state.GetTable(_T("abc"));

         std::vector<Lua::Value> vecParam;
         vecParam.push_back(Lua::Value(42.0));

         std::vector<Lua::Value> vecRetval = table.CallFunction(_T("def"), 3, vecParam);

         // check
         Assert::AreEqual<size_t>(3, vecRetval.size(), _T("must have returned 3 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42"));
         Assert::AreEqual(vecRetval[1].Get<double>(), 42.0+42.0, 1e-6, _T("value must be 42+42"));
         Assert::AreEqual(vecRetval[2].Get<double>(), 42.0*42.0, 1e-6, _T("value must be 42*42"));
      }

      /// tests method State::AddUserdata(), and value isn't stored anywhere
      TEST_METHOD(TestStateAddUserdataTemporary)
      {
         // setup
         Lua::State state;

         // run
         {
            Lua::Userdata userdata = state.AddUserdata(4);
         }

         // check
         // must not crash
      }

      /// tests method State::AddUserdata() and copy ctor of Userdata
      TEST_METHOD(TestStateUserdataCopyCtor)
      {
         // setup
         Lua::State state;

         // run
         {
            Lua::Userdata userdata = state.AddUserdata(4);
            Lua::Userdata userdata2 = userdata; // copy ctor

            Lua::Userdata userdata3 = userdata;
            userdata3 = userdata; // assign operator
         }

         // check
         // must not crash
      }

      /// tests method State::AddUserdata() and stores it as global value
      TEST_METHOD(TestStateAddUserdataStoreGlobal)
      {
         // setup
         Lua::State state;

         const unsigned char ucValue = 42;

         // run
         {
            Lua::Userdata userdata = state.AddUserdata(4);

            userdata.Data<unsigned char>()[0] = ucValue;

            state.AddValue(_T("user"), Lua::Value(userdata));
         }

         state.LoadSourceString(_T("function test() return user; end"));

         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1);

         // check
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::IsTrue(Lua::Value::typeUserdata == vecRetval[0].GetType(), _T("type must be userdata"));

         Lua::Userdata userdata = vecRetval[0].Get<Lua::Userdata>();

         Assert::IsTrue(userdata.Data<unsigned char>()[0] == ucValue, _T("stored value must match"));
      }

      /// tests method State::AddUserdata() and stores it as value in a table
      TEST_METHOD(TestStateAddUserdataStoreInTable)
      {
         // setup
         Lua::State state;

         const unsigned char ucValue = 42;

         // run
         {
            Lua::Userdata userdata = state.AddUserdata(4);

            userdata.Data<unsigned char>()[0] = ucValue;

            state.AddValue(_T("user"), Lua::Value(userdata));
         }

         {
            Lua::Value value = state.GetValue(_T("user"));
            Lua::Userdata userdata = value.Get<Lua::Userdata>();

            state.AddValue(_T("user2"), Lua::Value(userdata));
         }

         state.LoadSourceString(_T("function test() return user,user2; end"));

         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 2);

         // check
         Assert::AreEqual<size_t>(2, vecRetval.size(), _T("must have returned 2 return values"));

         Assert::IsTrue(Lua::Value::typeUserdata == vecRetval[0].GetType(), _T("type must be userdata"));
         Assert::IsTrue(Lua::Value::typeUserdata == vecRetval[1].GetType(), _T("type must be userdata"));

         Lua::Userdata userdata1 = vecRetval[0].Get<Lua::Userdata>();
         Lua::Userdata userdata2 = vecRetval[1].Get<Lua::Userdata>();

         Assert::IsTrue(userdata1.Data() == userdata2.Data(), _T("pointer of two userdatas must be equal"));
      }

      /// tests method State::AddUserdata() and stores it as value in a table
      TEST_METHOD(TestStateUserdataCopy)
      {
         // setup
         Lua::State state;

         const unsigned char ucValue = 42;

         // run
         {
            Lua::Table table = state.AddTable(_T("abc"));

            Lua::Userdata userdata = state.AddUserdata(4);

            userdata.Data<unsigned char>()[0] = ucValue;

            table.AddValue(_T("user"), Lua::Value(userdata));
         }

         // check
         // must not crash
      }

      TEST_METHOD(TestStateAddFunction)
      {
         Lua::State state;

         // setup

         // define a function to call back
         Lua::T_fnCFunction fn = [&] (const std::vector<Lua::Value>&) -> std::vector<Lua::Value>
         {
            std::vector<Lua::Value> vecRetValues;
            vecRetValues.push_back(Lua::Value(42.0));

            return vecRetValues;
         };

         state.AddFunction(_T("run"), fn);

         // run
         state.LoadSourceString(_T("function test() return run(); end"));

         // check
         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1);

         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42"));
      }

      TEST_METHOD(TestStateAddFunctionWithParam)
      {
         Lua::State state;

         // setup

         // define a function to call back
         Lua::T_fnCFunction fn = [&] (const std::vector<Lua::Value>& vecParams) -> std::vector<Lua::Value>
         {
            if (vecParams.size() != 1)
               throw std::runtime_error("must pass exactly 1 value");

            if (Lua::Value::typeNumber != vecParams[0].GetType())
               throw std::runtime_error("must pass a number value");

            std::vector<Lua::Value> vecRetValues;
            vecRetValues.push_back(vecParams[0]);

            return vecRetValues;
         };

         state.AddFunction(_T("run"), fn);

         // run
         state.LoadSourceString(_T("function test(val) return run(val); end"));

         std::vector<Lua::Value> vecParam;
         vecParam.push_back(Lua::Value(42.0));

         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1, vecParam);

         // check
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42"));
      }

      TEST_METHOD(TestStateAddValue)
      {
         Lua::State state;

         // setup
         state.AddValue(_T("the_value"), Lua::Value(42.0));

         // run
         state.LoadSourceString(_T("function test() return the_value; end"));

         // check
         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1);

         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42"));
      }

      TEST_METHOD(TestFunctionPushTable)
      {
         Lua::State state;

         // setup

         // define a function to call back
         Lua::T_fnCFunction fn = [&] (const std::vector<Lua::Value>& vecParams) -> std::vector<Lua::Value>
         {
            if (vecParams.size() != 1)
               throw std::runtime_error("must pass exactly 1 value");

            if (Lua::Value::typeNumber != vecParams[0].GetType())
               throw std::runtime_error("must pass a number value");

            Lua::Table table = state.AddTable(_T(""));
            table.AddValue(_T("def"), vecParams[0]);

            std::vector<Lua::Value> vecRetValues;
            vecRetValues.push_back(Lua::Value(table));

            return vecRetValues;
         };

         state.AddFunction(_T("run"), fn);

         // run
         state.LoadSourceString(_T("function test(val) local abc = run(val); return abc.def; end"));

         std::vector<Lua::Value> vecParam;
         vecParam.push_back(Lua::Value(42.0));

         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1, vecParam);

         // check
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42"));
      }

      TEST_METHOD(TestFunctionParamInClosure)
      {
         Lua::State state;

         // setup

         // define a function to call back
         Lua::T_fnCFunction fn = [&] (const std::vector<Lua::Value>& vecParams) -> std::vector<Lua::Value>
         {
            if (vecParams.size() != 1)
               throw std::runtime_error("must pass exactly 1 value");

            if (Lua::Value::typeFunction != vecParams[0].GetType())
               throw std::runtime_error("must pass a function value");

            // call function
            Lua::Function func = vecParams[0].Get<Lua::Function>();

            std::vector<Lua::Value> vecCallParams;
            vecCallParams.push_back(Lua::Value(42.0));
            vecCallParams.push_back(Lua::Value(64.0));

            return func.Call(1, vecCallParams); // tail call
         };

         state.AddFunction(_T("run"), fn);

         // run
         state.LoadSourceString(_T("function calc(a, b) return a+b; end"));
         state.LoadSourceString(_T("function test() return run(calc); end"));

         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("test"), 1);

         // check
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0+64.0, 1e-6, _T("value must be 42+64"));
      }

      TEST_METHOD(TestFunctionPushOnStack)
      {
         Lua::State state;

         // setup

         // define a function to call back
         Lua::T_fnCFunction fnAsyncWait = [&](const std::vector<Lua::Value>& vecParams) -> std::vector<Lua::Value>
         {
            if (vecParams.size() != 1)
               throw std::runtime_error("must pass exactly 1 value");

            if (Lua::Value::typeFunction != vecParams[0].GetType())
               throw std::runtime_error("must pass a function value in param 0");

            Lua::Function func = vecParams[0].Get<Lua::Function>();

            // add function as value
            state.AddValue(_T("_the_handler"), vecParams[0]);

            return std::vector<Lua::Value>();
         };

         Lua::T_fnCFunction fnHandler = [&](const std::vector<Lua::Value>& vecParams)-> std::vector<Lua::Value>
         {
            if (vecParams.size() != 1)
               throw std::runtime_error("must pass exactly 1 value");

            if (Lua::Value::typeNumber != vecParams[0].GetType())
               throw std::runtime_error("must pass a number value in param 0");

            std::vector<Lua::Value> vecRetValues;
            vecRetValues.push_back(Lua::Value(42.0));

            return vecRetValues;
         };

         state.AddFunction(_T("async_wait"), fnAsyncWait);
         state.AddFunction(_T("handler"), fnHandler);

         state.LoadSourceString(_T("function wait_completed() return _the_handler(64.0); end"));
         state.LoadSourceString(_T("function test() async_wait(handler); end"));

         // run
         state.CallFunction(_T("test"));
         std::vector<Lua::Value> vecRetval = state.CallFunction(_T("wait_completed"), 1);

         // check
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42.0"));
      }

      TEST_METHOD(TestFunctionPushOnTable)
      {
         Lua::State state;

         // setup

         // define a function to call back
         Lua::T_fnCFunction fnAsyncWait = [&](const std::vector<Lua::Value>& vecParams) -> std::vector<Lua::Value>
         {
            if (vecParams.size() != 2)
               throw std::runtime_error("must pass exactly 2 values");

            if (Lua::Value::typeTable != vecParams[0].GetType())
               throw std::runtime_error("must pass a table value in param 0");

            if (Lua::Value::typeFunction != vecParams[1].GetType())
               throw std::runtime_error("must pass a function value in param 1");

            Lua::Table self = vecParams[0].Get<Lua::Table>();
            Lua::Function func = vecParams[1].Get<Lua::Function>();

            // add function as value
            self.AddValue(_T("_the_handler"), vecParams[1]);

            return std::vector<Lua::Value>();
         };

         Lua::T_fnCFunction fnHandler = [&](const std::vector<Lua::Value>& vecParams)-> std::vector<Lua::Value>
         {
            if (vecParams.size() != 2)
               throw std::runtime_error("must pass exactly 2 values");

            if (Lua::Value::typeTable != vecParams[0].GetType())
               throw std::runtime_error("must pass a table value in param 0");

            if (Lua::Value::typeNumber != vecParams[1].GetType())
               throw std::runtime_error("must pass a number value in param 1");

            std::vector<Lua::Value> vecRetValues;
            vecRetValues.push_back(Lua::Value(42.0));

            return vecRetValues;
         };

         state.LoadSourceString(_T("App = { ")
            _T("wait_completed = function() return App:_the_handler(64.0); end, ")
            _T("test = function() App:async_wait(App.handler); end ")
            _T("}"));

         Lua::Table table = state.GetTable(_T("App"));
         table.AddFunction("async_wait", fnAsyncWait);
         table.AddFunction("handler", fnHandler);

         // run
         table.CallFunction(_T("test"));
         std::vector<Lua::Value> vecRetval = table.CallFunction(_T("wait_completed"), 1);

         // check
         Assert::AreEqual<size_t>(1, vecRetval.size(), _T("must have returned 1 return value"));

         Assert::AreEqual(vecRetval[0].Get<double>(), 42.0, 1e-6, _T("value must be 42.0"));
      }
   };
}
