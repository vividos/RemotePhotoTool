//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file Lua.cpp Lua wrapper classes
//

// includes
#include "StdAfx.h"
#include "Lua.hpp"

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

using namespace Lua;

/// list of libraries that can be loaded using Lua::State::RequireLib()
static const luaL_Reg loadedlibs[] = {
   { "_G", luaopen_base },
   { LUA_LOADLIBNAME, luaopen_package },
   { LUA_COLIBNAME, luaopen_coroutine },
   { LUA_TABLIBNAME, luaopen_table },
   { LUA_IOLIBNAME, luaopen_io },
   { LUA_OSLIBNAME, luaopen_os },
   { LUA_STRLIBNAME, luaopen_string },
   { LUA_MATHLIBNAME, luaopen_math },
   { LUA_UTF8LIBNAME, luaopen_utf8 },
   { LUA_DBLIBNAME, luaopen_debug },
   { NULL, NULL }
};

//
// Lua::Exception
//

CString Lua::Exception::MessageFromState(const CString& cszMessage, lua_State* L) throw()
{
   CString cszStackTrace = ReadStackTrace(L);

   try
   {
      CString cszText;
      cszText.Format(_T("Lua error: %s, in:\n%s"), cszMessage.GetString(), cszStackTrace.GetString());
      return cszText;
   }
   catch (...)
   {
      return CString();
   }
}

CString Lua::Exception::ReadStackTrace(lua_State* L)
{
   CString cszInfo;

   for (int iStack = 0;; iStack++)
   {
      lua_Debug debug = { 0 };

      int iRet = lua_getstack(L, iStack, &debug);
      if (iRet == 0)
         break;

      try
      {
         lua_getinfo(L, "Sl", &debug);
      }
      catch (...)
      {
         // may throw exception of type  struct lua_longjmp
         ATLTRACE(_T("exception during lua_getinfo()\n"));
      }

      try
      {
         cszInfo.AppendFormat(_T("%hs:%u\n"),
            debug.short_src,
            debug.currentline);
      }
      catch (...)
      {
         ATLTRACE(_T("exception during formatting debug info()\n"));
      }
   }

   cszInfo.Trim();

   return cszInfo;
}

void Lua::Exception::ParseLuaError(LPCSTR pszaText)
{
   CString cszText(pszaText);
   int iPos1 = cszText.Find(_T(':'), 3);
   int iPos2 = cszText.Find(_T(':'), iPos1 + 1);

   if (iPos1 != -1 && iPos2 != -1)
   {
      m_cszLuaSourceFile = cszText.Left(iPos1);
      m_cszLuaErrorMessage = cszText.Mid(iPos2 + 1);
      m_cszLuaErrorMessage.Trim();

      m_uiLuaLineNumber = static_cast<UINT>(_tcstoul(cszText.Mid(iPos1 + 1, iPos2 - iPos1 - 1), NULL, 10));
   }
}

//
// Lua::StackChecker
//

StackChecker::StackChecker(lua_State* L)
   :m_L(L),
   m_iStackDepth(lua_absindex(L, -1))
{
}

StackChecker::~StackChecker()
{
   int iStackDepth = lua_absindex(m_L, -1);
   ATLASSERT(m_iStackDepth == iStackDepth);
   iStackDepth;
}

//
// Lua::Ref
//

Ref::Ref(State& state, int iStackIndex)
   :m_state(state),
   m_iStackIndex(lua_absindex(state.GetState(), iStackIndex))
{
}

Ref::~Ref() throw()
{
   if (m_iStackIndex != -1)
      m_state.RemoveRef(*this);
}

void Ref::Detach()
{
   m_iStackIndex = -1;
}

void Ref::OnRemoveStackValue(int iStackIndex)
{
   ATLASSERT(iStackIndex != m_iStackIndex); // must not be own ref that is removed

   if (m_iStackIndex >= iStackIndex)
      m_iStackIndex--; // it's below us, so reduce index
}


//
// Lua::Value
//

/// default ctor; constructs nil value
Value::Value()
   :m_enType(typeNil)
{
}

Value::Value(bool bValue)
   :m_value(bValue),
   m_enType(typeBoolean)
{
}

Value::Value(double dValue)
   :m_value(dValue),
   m_enType(typeNumber)
{
}

Value::Value(int iValue)
   :m_value(iValue),
   m_enType(typeInteger)
{
}

Value::Value(const CString& cszText)
   :m_value(CStringA(cszText)),
   m_enType(typeString)
{
}

Value::Value(LPCSTR pszText)
   :m_value(CStringA(pszText)),
   m_enType(typeString)
{
}

Value::Value(Table table)
   :m_enType(typeTable),
   m_value(table),
   m_spRef(table.GetRef())
{
}

Value::Value(Userdata userdata)
   :m_enType(typeUserdata),
   m_value(userdata),
   m_spRef(userdata.GetRef())
{
}

Value::Value(Function func)
   :m_enType(typeFunction),
   m_value(func),
   m_spRef(func.GetRef())
{
}

Value::Value(Thread thread)
   :m_enType(typeThread),
   m_value(thread),
   m_spRef(thread.GetRef())
{
}

Value::Value(const Value& val)
   :m_value(val.m_value),
   m_enType(val.m_enType),
   m_spRef(val.GetRef())
{
}

Value& Value::operator=(const Value& val)
{
   m_value = val.m_value;
   m_enType = val.m_enType;
   m_spRef = val.GetRef();

   return *this;
}

Value::~Value() throw()
{
   std::weak_ptr<Ref> wpRef = m_spRef;
   m_spRef.reset();

   if (wpRef.use_count() == 1)
   {
      std::shared_ptr<Ref> spRef = wpRef.lock();
      spRef->GetState().RemoveFromStack(*spRef);
   }
}

void Value::Attach(std::shared_ptr<Ref> spRef) const
{
   m_spRef = spRef;
}

void Value::Push(State& state) const
{
   lua_State* L = state.GetState();

   switch (m_enType)
   {
   case typeNil:
      lua_pushnil(L);
      Attach(std::make_shared<Ref>(state, -1));
      break;

   case typeBoolean:
      lua_pushboolean(L, boost::any_cast<bool>(m_value) ? 1 : 0);
      Attach(std::make_shared<Ref>(state, -1));
      break;

   case typeNumber:
      lua_pushnumber(L, boost::any_cast<double>(m_value));
      Attach(std::make_shared<Ref>(state, -1));
      break;

   case typeInteger:
      lua_pushinteger(L, boost::any_cast<int>(m_value));
      Attach(std::make_shared<Ref>(state, -1));
      break;

   case typeString:
      lua_pushstring(L, boost::any_cast<CStringA>(m_value));
      Attach(std::make_shared<Ref>(state, -1));
      break;

   case typeTable:
   {
      Table table = Get<Table>();
      table.Push();
   }
   break;

   case typeFunction:
   {
      Function func = Get<Function>();
      func.Push();
   }
   break;

   case typeUserdata:
   {
      Userdata userdata = Get<Userdata>();
      userdata.Push();
   }
   break;

   case typeThread:
   {
      Thread thread = Get<Thread>();
      thread.Push();
   }
   break;

   default:
      ATLASSERT(false);
      break;
   }
}

void Value::Detach() const
{
   m_spRef->Detach();
   m_spRef.reset();

   switch (m_enType)
   {
   case typeTable:
   {
      Table table = Get<Table>();
      table.Detach();
   }
   break;

   case typeFunction:
   {
      Function func = Get<Function>();
      func.Detach();
   }
   break;

   case typeUserdata:
   {
      Userdata userdata = Get<Userdata>();
      userdata.Detach();
   }
   break;

   case typeThread:
   {
      Thread thread = Get<Thread>();
      thread.Detach();
   }
   break;
   }
}

Value Value::FromStack(State& state, int iStackIndex)
{
   lua_State* L = state.GetState();

   switch (lua_type(L, iStackIndex))
   {
   case LUA_TNONE:
   case LUA_TNIL:
   {
      Value val;
      val.Attach(std::make_shared<Ref>(state, iStackIndex));
      state.AddRef(val.GetRef());
      return val;
   }

   case LUA_TNUMBER:
   {
      Value val(lua_tonumber(L, iStackIndex));
      val.Attach(std::make_shared<Ref>(state, iStackIndex));
      state.AddRef(val.GetRef());
      return val;
   }

   case LUA_TBOOLEAN:
   {
      Value val(lua_toboolean(L, iStackIndex) != 0);
      val.Attach(std::make_shared<Ref>(state, iStackIndex));
      state.AddRef(val.GetRef());
      return val;
   }

   case LUA_TSTRING:
   {
      Value val(lua_tostring(L, iStackIndex));
      val.Attach(std::make_shared<Ref>(state, iStackIndex));
      state.AddRef(val.GetRef());
      return val;
   }

   case LUA_TTABLE:
   {
      auto spRef = std::make_shared<Ref>(state, iStackIndex);
      state.AddRef(spRef);

      return Value(Table(spRef, _T("")));
   }

   case LUA_TFUNCTION:
   {
      auto spRef = std::make_shared<Ref>(state, iStackIndex);
      state.AddRef(spRef);

      return Value(Function(spRef));
   }

   case LUA_TUSERDATA:
   {
      auto spRef = std::make_shared<Ref>(state, iStackIndex);
      state.AddRef(spRef);

      return Value(Userdata(spRef));
   }

   case LUA_TTHREAD:
   {
      auto spRef = std::make_shared<Ref>(state, iStackIndex);
      state.AddRef(spRef);

      return Value(Thread(spRef));
   }

   default:
      ATLASSERT(false); // type can't be converted into Value
      break;
   }

   return Value();
}

//
// FuncData
//

/// C++ function data (stored as user data in State)
struct FuncData
{
   /// ctor
   FuncData(T_fnCFunction fn)
      :m_fn(fn)
   {
   }

   /// pushes C closure
   static void PushCClosure(State& state, T_fnCFunction fn);

   /// returns ref to self from upvalue
   static FuncData& GetSelf(lua_State* L);

   /// function call
   static int OnFunctionCall(lua_State* L);

   /// garbage collect call
   static int OnFunctionGarbageCollect(lua_State* L);

   /// bound C++ function
   T_fnCFunction m_fn;
};

void FuncData::PushCClosure(State& state, T_fnCFunction fn)
{
   lua_State* L = state.GetState();

   // add userdata
   void* pUserdata = lua_newuserdata(L, sizeof(FuncData*));
   FuncData** pFunction = reinterpret_cast<FuncData**>(pUserdata);
   *pFunction = new FuncData(fn);

   ATLASSERT(lua_isuserdata(L, -1) != 0);

   // set up metatable
   lua_newtable(L);

   lua_pushcfunction(L, &FuncData::OnFunctionGarbageCollect);
   lua_setfield(L, -2, "__gc");

   lua_setmetatable(L, -2);

   /// push the C closure
   lua_pushcclosure(L, &FuncData::OnFunctionCall, 1);
}

FuncData& FuncData::GetSelf(lua_State* L)
{
   void* p = lua_touserdata(L, lua_upvalueindex(1));

   FuncData** ppFuncData = reinterpret_cast<FuncData**>(p);
   if (ppFuncData == nullptr)
      throw Lua::Exception(_T("function call without allocated FuncData"), L, __FILE__, __LINE__);

   FuncData* pFuncData = *ppFuncData;
   if (pFuncData == nullptr)
      throw Lua::Exception(_T("function call without set FuncData"), L, __FILE__, __LINE__);

   return *pFuncData;
}

int FuncData::OnFunctionCall(lua_State* L)
{
   FuncData& data = GetSelf(L);

   if (data.m_fn == nullptr)
      return 0; // no function bound

   // collect params
   int iStackDepth = lua_absindex(L, -1);

   State paramState(L, false);

   std::vector<Value> vecParams;
   for (int i = 1; i <= iStackDepth; i++)
      vecParams.push_back(Value::FromStack(paramState, i));

   State::TraceUpvalues(L);

   // call bound function
   std::vector<Value> vecRetVals;
   try
   {
      vecRetVals = data.m_fn(paramState, vecParams);
   }
   catch (...)
   {
      // in case of exceptions, detach all values from parameter state
      paramState.DetachAll();
      throw;
   }

   // put results on stack
   std::for_each(vecRetVals.begin(), vecRetVals.end(),
      [&](const Value& value) { value.Push(paramState); value.Detach(); });

   return vecRetVals.size();
}

int FuncData::OnFunctionGarbageCollect(lua_State* L)
{
   void* p = lua_touserdata(L, -1);

   FuncData** ppFuncData = reinterpret_cast<FuncData**>(p);
   if (ppFuncData == nullptr)
      throw Lua::Exception(_T("garbage collecting c closure without allocated FuncData"), L, __FILE__, __LINE__);

   delete *ppFuncData;
   *ppFuncData = nullptr;

   return 0;
}

//
// Lua::Function
//

Function::Function(std::shared_ptr<Ref> spRef)
   :m_spRef(spRef)
{
}

Function::Function(const Function& func)
   :m_spRef(func.GetRef())
{
}

Function& Function::operator=(const Function& func)
{
   m_spRef = func.GetRef();

   return *this;
}

Function::~Function() throw()
{
   ATLASSERT(m_spRef == nullptr || m_spRef->GetStackIndex() == -1 ||
      lua_isfunction(m_spRef->GetState().GetState(), m_spRef->GetStackIndex()) == true);

   std::weak_ptr<Ref> wpRef = m_spRef;
   m_spRef.reset();

   if (wpRef.use_count() == 1)
   {
      std::shared_ptr<Ref> spRef = wpRef.lock();
      spRef->GetState().RemoveFromStack(*spRef);
   }
}

std::vector<Value> Function::Call(int iResults, const std::vector<Value>& vecParam)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   lua_pushvalue(L, m_spRef->GetStackIndex());

   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(state); value.Detach(); });

   lua_call(L, vecParam.size(), iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i = -iResults; i <= -1; i++)
      vecResults.push_back(Value::FromStack(state, i));

   return vecResults;
}

void Function::Push()
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   lua_pushvalue(L, m_spRef->GetStackIndex());
}

void Function::Detach() const
{
   m_spRef->Detach();
   m_spRef.reset();
}

//
// Lua::Table
//

Table::Table(State& state, const CString& cszName)
   :m_cszName(cszName)
{
   lua_State* L = state.GetState();

   lua_newtable(L);

   if (!cszName.IsEmpty())
   {
      // no temporary (empty name) table? then set and get again
      lua_setglobal(L, CStringA(m_cszName));

      lua_getglobal(L, CStringA(m_cszName).GetString());
   }

   m_spRef = std::make_shared<Ref>(state, -1);

   state.AddRef(m_spRef);
}

Table::Table(std::shared_ptr<Ref> spRef, const CString& cszName)
   :m_spRef(spRef),
   m_cszName(cszName)
{
}

Table::Table(const Table& table)
   : m_spRef(table.GetRef()),
   m_cszName(table.m_cszName)
{
}

Table& Table::operator=(const Table& table)
{
   m_spRef = table.GetRef();

   return *this;
}

Table::~Table() throw()
{
   ATLASSERT(m_spRef == nullptr || m_spRef->GetStackIndex() == -1 ||
      lua_istable(m_spRef->GetState().GetState(), m_spRef->GetStackIndex()) == true);

   std::weak_ptr<Ref> wpRef = m_spRef;
   m_spRef.reset();

   if (wpRef.use_count() == 1)
   {
      std::shared_ptr<Ref> spRef = wpRef.lock();
      spRef->GetState().RemoveFromStack(*spRef);
   }
}

Table& Table::AddValue(const CString& key, const Value& value)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   ATLASSERT(lua_istable(L, m_spRef->GetStackIndex()) != 0);

   value.Push(state);

   lua_setfield(L, m_spRef->GetStackIndex(), CStringA(key));

   value.Detach();

   return *this;
}

Table& Table::AddValue(int key, const Value& value)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   ATLASSERT(lua_istable(L, m_spRef->GetStackIndex()) != 0);

   value.Push(state);

   lua_seti(L, m_spRef->GetStackIndex(), lua_Integer(key));

   value.Detach();

   return *this;
}

Table& Table::AddFunction(LPCSTR pszaName, T_fnCFunction fn)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   ATLASSERT(lua_istable(L, m_spRef->GetStackIndex()) != 0);

   FuncData::PushCClosure(state, fn);

   lua_setfield(L, m_spRef->GetStackIndex(), pszaName);

   return *this;
}

Value Table::GetValue(const CString& key)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   lua_pushstring(L, CStringA(key));

   lua_gettable(L, m_spRef->GetStackIndex());

   return Value::FromStack(state, -1);
}

std::vector<Value> Table::CallFunction(const CString& cszName,
   int iResults,
   const std::vector<Value>& vecParam)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   lua_getfield(L, m_spRef->GetStackIndex(), CStringA(cszName).GetString());

   if (!lua_isfunction(L, -1))
   {
      lua_pop(L, 1);
      throw Lua::Exception(_T("function not found: ") + cszName, L, __FILE__, __LINE__);
   }

   // add table as first argument
   lua_pushvalue(L, m_spRef->GetStackIndex());

   // then params
   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(state); value.Detach();  });

   try
   {
      lua_call(L, vecParam.size() + 1, iResults);
   }
   catch (const Lua::Exception&)
   {
      // since the exception is re-thrown, detach all params from stack
      state.DetachAll();
      throw;
   }

   // collect return values
   std::vector<Value> vecResults;
   for (int i = -iResults; i <= -1; i++)
      vecResults.push_back(Value::FromStack(state, i));

   return vecResults;
}

void Table::Push()
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   lua_pushvalue(L, m_spRef->GetStackIndex());
}

void Table::Detach() const
{
   m_spRef->Detach();
   m_spRef.reset();
}

//
// Lua::Userdata
//

Userdata::Userdata(State& state, size_t uiSize)
   :m_pUserdata(nullptr),
   m_uiSize(uiSize)
{
   lua_State* L = state.GetState();

   m_pUserdata = lua_newuserdata(L, uiSize);

   m_spRef = std::make_shared<Ref>(state, -1);

   state.AddRef(m_spRef);
}

Userdata::Userdata(std::shared_ptr<Ref> spRef)
   :m_spRef(spRef),
   m_pUserdata(nullptr),
   m_uiSize(0)
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   int iStackIndex = m_spRef->GetStackIndex();

   ATLASSERT(lua_isuserdata(L, iStackIndex) != 0);

   m_pUserdata = lua_touserdata(L, iStackIndex);
   m_uiSize = lua_rawlen(L, iStackIndex);
}

Userdata::Userdata(const Userdata& userdata)
   :m_spRef(userdata.GetRef()),
   m_pUserdata(userdata.m_pUserdata),
   m_uiSize(userdata.m_uiSize)
{
}

Userdata& Userdata::operator=(const Userdata& userdata)
{
   m_spRef = userdata.GetRef();
   m_pUserdata = userdata.m_pUserdata;
   m_uiSize = userdata.m_uiSize;

   return *this;
}

Userdata::~Userdata() throw()
{
   ATLASSERT(m_spRef == nullptr || m_spRef->GetStackIndex() == -1 ||
      lua_isuserdata(m_spRef->GetState().GetState(), m_spRef->GetStackIndex()) != 0);

   std::weak_ptr<Ref> wpRef = m_spRef;
   m_spRef.reset();

   if (wpRef.use_count() == 1)
   {
      std::shared_ptr<Ref> spRef = wpRef.lock();
      spRef->GetState().RemoveFromStack(*spRef);
   }
}

void Userdata::Push()
{
   State& state = m_spRef->GetState();
   lua_State* L = state.GetState();

   lua_pushvalue(L, m_spRef->GetStackIndex());
}

void Userdata::Detach() const
{
   m_spRef->Detach();
   m_spRef.reset();
}

//
// Lua::State
//

State::State()
{
   lua_State* L = luaL_newstate();
   m_spState.reset(L, &lua_close);

   lua_atpanic(L, OnLuaPanic);
}

State::State(lua_State* L, bool mainState)
   :m_spState(L, mainState ? &lua_close : &State::StateDeleterNoop)
{
   lua_atpanic(L, OnLuaPanic);
}

void State::RequireLib(const char* moduleName)
{
   lua_State* L = GetState();

   for (int i = 0; i < sizeof(loadedlibs) / sizeof(*loadedlibs); i++)
   {
      if (loadedlibs[i].func == nullptr)
         break;

      if (strcmp(moduleName, loadedlibs[i].name) == 0)
      {
         luaL_requiref(L, loadedlibs[i].name, loadedlibs[i].func, 1);
         lua_pop(L, 1);  // remove lib from stack

         return;
      }
   }

   throw Lua::Exception(_T("unknown builtin library"), L, __FILE__, __LINE__);
}

void State::LoadFile(const CString& cszFilename)
{
   lua_State* L = GetState();
   StackChecker checker(L);

   CStringA cszaFilename(cszFilename);

   int iRet = luaL_dofile(L, cszaFilename.GetString());

   if (iRet != 0)
      throw Lua::Exception(_T("error while loading file"), lua_tostring(L, -1), GetState());
}

void State::LoadSourceString(const CString& cszLuaSource)
{
   lua_State* L = GetState();
   StackChecker checker(L);

   CStringA cszaLuaSource(cszLuaSource);

   int iRet = luaL_dostring(L, cszaLuaSource.GetString());

   if (iRet != 0)
      throw Lua::Exception(_T("error while loading source string"), lua_tostring(L, -1), GetState());
}

std::vector<Value> State::CallFunction(const CString& cszName, int iResults, const std::vector<Value>& vecParam)
{
   lua_State* L = GetState();

   lua_getglobal(L, CStringA(cszName).GetString());
   if (!lua_isfunction(L, -1))
   {
      lua_pop(L, 1);
      throw Lua::Exception(_T("function not found: ") + cszName, L, __FILE__, __LINE__);
   }

   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(*this); value.Detach(); });

   lua_call(L, vecParam.size(), iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i = -iResults; i <= -1; i++)
      vecResults.push_back(Value::FromStack(*this, i));

   return vecResults;
}

Table State::AddTable(const CString& cszName)
{
   return Table(*this, cszName);
}

Userdata State::AddUserdata(size_t uiSize)
{
   return Userdata(*this, uiSize);
}

void State::AddFunction(LPCTSTR pszaName, T_fnCFunction fn)
{
   lua_State* L = GetState();

   FuncData::PushCClosure(*this, fn);

   lua_setglobal(L, CStringA(pszaName));
}

void State::AddValue(LPCTSTR pszaName, Value value)
{
   lua_State* L = GetState();

   value.Push(*this);

   lua_setglobal(L, CStringA(pszaName));

   value.Detach();
}

Value State::GetValue(const CString& cszName)
{
   lua_State* L = GetState();

   lua_getglobal(L, CStringA(cszName).GetString());

   return Value::FromStack(*this, -1);
}

Table State::GetTable(const CString& cszName)
{
   lua_State* L = GetState();

   lua_getglobal(L, CStringA(cszName).GetString());
   if (!lua_istable(L, -1))
   {
      lua_pop(L, 1); // remove global
      throw Lua::Exception(_T("table not found: ") + cszName, L, __FILE__, __LINE__);
   }

   auto spRef = std::make_shared<Ref>(*this, -1);
   AddRef(spRef);

   return Table(spRef, cszName);
}

void State::CollectGarbage()
{
   lua_State* L = GetState();

   lua_gc(L, LUA_GCCOLLECT, 0);
}

void State::StateDeleterNoop(lua_State*)
{
}

int State::OnLuaPanic(lua_State* L)
{
   //State::TraceStack(L);

   CString cszErrorMessage = lua_tostring(L, -1);
   ATLTRACE(_T("Lua Panic: %s\n"), cszErrorMessage.GetString());

   throw Lua::Exception(cszErrorMessage, L, __FILE__, __LINE__);
}

void State::TraceStack(lua_State* L)
{
   StackChecker checker(L);

   int iStackDepth = lua_gettop(L);

   ATLTRACE(_T("Tracing stack, %i entries:\n"), iStackDepth);

   for (int iIndex = 1; iIndex <= iStackDepth; iIndex++)
      TraceValue(L, iIndex, iStackDepth, false);

   ATLTRACE(_T("End tracing stack.\n"));
}

void State::TraceUpvalues(lua_State* L)
{
   StackChecker checker(L);

   ATLTRACE(_T("Tracing upvalues:\n"));

   for (int iUpvalueIndex = 1; iUpvalueIndex < 256; iUpvalueIndex++)
   {
      int iIndex = lua_upvalueindex(iUpvalueIndex);

      if (lua_type(L, iIndex) == LUA_TNONE)
         continue;

      TraceValue(L, iIndex, 0, true);
   }

   ATLTRACE(_T("End tracing upvalues.\n"));
}

/// enumerates all keys of a table and returns it as space delimited string
CString EnumTableIndices(lua_State* L, int iIndex)
{
   StackChecker checker(L);

   CString cszText;

   if (iIndex < 0)
      iIndex = lua_absindex(L, iIndex);

   lua_pushnil(L);
   while (lua_next(L, iIndex) != 0)
   {
      if (lua_type(L, -2) == LUA_TSTRING)
         cszText.AppendFormat(_T(" \"%hs\""), lua_tostring(L, -2));
      else if (lua_type(L, -2) == LUA_TNUMBER)
         cszText.AppendFormat(_T(" \"%f\""), lua_tonumber(L, -2));
      else
         cszText.AppendFormat(_T(" (type %i)"), lua_type(L, -2));

      lua_pop(L, 1);
   }

   return cszText;
}

void State::TraceValue(lua_State* L, int iIndex, int iStackDepth, bool bIsUpvalue)
{
   iStackDepth;

   CString cszType(lua_typename(L, lua_type(L, iIndex)));

   CString cszContent(_T("???"));
   switch (lua_type(L, iIndex))
   {
   case LUA_TNONE: cszContent = _T("[none]"); break;
   case LUA_TNIL:  cszContent = _T("nil"); break;
   case LUA_TNUMBER: cszContent.Format(_T("%f"), lua_tonumber(L, iIndex)); break;
   case LUA_TBOOLEAN: cszContent = lua_toboolean(L, iIndex) != 0 ? _T("true") : _T("false"); break;
   case LUA_TSTRING: cszContent.Format(_T("[%hs]"), lua_tostring(L, iIndex)); break;
   case LUA_TTABLE:
   {
      cszContent.Format(_T("[table%s]"), EnumTableIndices(L, iIndex));
      break;
   }

   case LUA_TFUNCTION:
   {
      cszContent.Format(
         lua_iscfunction(L, iIndex) ? _T("(C) 0x%p") : _T("(Lua)"),
         lua_tocfunction(L, iIndex));
      break;
   }

   case LUA_TLIGHTUSERDATA:
   {
      cszContent.Format(_T("0x%p"),
         lua_touserdata(L, iIndex));
      break;
   }

   case LUA_TUSERDATA:
   {
      cszContent.Format(_T("0x%p, size=%u"),
         lua_touserdata(L, iIndex),
         lua_rawlen(L, iIndex));
      break;
   }

   case LUA_TTHREAD:
      cszContent = _T("[thread]");
      break;

   default:
      ATLASSERT(false);
      break;
   }

   CString cszMetatableInfo;
   int iHasMetatable = lua_getmetatable(L, iIndex);
   if (iHasMetatable != 0)
   {
      cszMetatableInfo = _T(" +[metatable");
      cszMetatableInfo += EnumTableIndices(L, -1);
      cszMetatableInfo += _T("]");
      lua_pop(L, 1);
   }

   CString cszText;

   if (bIsUpvalue)
      cszText.Format(_T("[%i] "), lua_upvalueindex(iIndex));
   else
      cszText.Format(_T("[%i] [%i] "),
         iIndex,
         iIndex - iStackDepth - 1);

   cszText.AppendFormat(_T("%s %s%s\n"),
      cszType.GetString(),
      cszContent.GetString(),
      cszMetatableInfo);

   ATLTRACE(cszText);
}

LPCTSTR State::GetVersion() throw()
{
   return _T(LUA_RELEASE);
}

void State::AddRef(std::shared_ptr<Ref> spRef)
{
   m_vecRefsOnStack.push_back(spRef);
}

void State::RemoveRef(Ref& ref)
{
   RemoveFromStack(ref);
   RemoveIndex(ref.GetStackIndex());
}

bool State::RemoveFromStack(const Ref& ref)
{
   auto iter = std::find_if(m_vecRefsOnStack.begin(), m_vecRefsOnStack.end(),
      [&](std::shared_ptr<Ref>& spRef)
   {
      return spRef.get() == &ref;
   });

   bool bFound = iter != m_vecRefsOnStack.end();
   if (bFound)
      m_vecRefsOnStack.erase(iter);

   return bFound;
}

void State::RemoveIndex(int iStackIndex)
{
   // adjust all stack indices of referenced values
   std::for_each(m_vecRefsOnStack.begin(), m_vecRefsOnStack.end(),
      [&](std::shared_ptr<Ref>& spRef)
   {
      spRef->OnRemoveStackValue(iStackIndex);
   });

   lua_State* L = GetState();

   State::TraceStack(L);

   lua_remove(L, iStackIndex);
}

void State::DetachAll()
{
   std::for_each(m_vecRefsOnStack.begin(), m_vecRefsOnStack.end(),
      [&](std::shared_ptr<Ref>& spRef)
   {
      spRef->Detach();
   });
}

//
// Lua::Thread
//

static_assert(Thread::statusOK == LUA_OK, "value of statusOk must equal LUA_OK");
static_assert(Thread::statusYield == LUA_YIELD, "value of statusOk must equal LUA_YIELD");

Thread::Thread(Lua::State& state)
   :m_state(state),
   m_threadState(lua_newthread(state.GetState()), false) // non-mainState
{
}

Thread::Thread(std::shared_ptr<Ref> spRef)
   :m_spRef(spRef),
   m_state(spRef->GetState()),
   m_threadState(lua_tothread(spRef->GetState().GetState(), spRef->GetStackIndex()), false) // non-mainState
{
}

Value Thread::GetValue(const CString& cszName)
{
   lua_State* L = m_threadState.GetState();

   lua_getglobal(L, CStringA(cszName).GetString());

   return Value::FromStack(m_threadState, -1);
}

Table Thread::GetTable(const CString& cszName)
{
   lua_State* L = m_threadState.GetState();

   lua_getglobal(L, CStringA(cszName).GetString());
   if (!lua_istable(L, -1))
   {
      lua_pop(L, 1); // remove global
      throw Lua::Exception(_T("table not found: ") + cszName, L, __FILE__, __LINE__);
   }

   auto spRef = std::make_shared<Ref>(m_threadState, -1);
   m_threadState.AddRef(spRef);

   return Table(spRef, cszName);
}

Thread::T_enThreadStatus Thread::Status() const throw()
{
   lua_State* L = const_cast<Lua::State&>(m_threadState).GetState();

   return static_cast<T_enThreadStatus>(lua_status(L));
}

std::pair<Thread::T_enThreadStatus, std::vector<Lua::Value>> Thread::Start(Lua::Function& func, const std::vector<Lua::Value>& vecParam)
{
   ATLASSERT(Status() == statusOK);

   if (Status() != statusOK)
      throw Lua::Exception(_T("Lua thread attempted to start has yielded"), m_threadState.GetState(), __FILE__, __LINE__);

   Lua::Value funcValue(func);

   funcValue.Push(m_threadState);

   // detach in case the thread yields; then the stack is empty
   funcValue.Detach();

   return InternalResume(vecParam);
}

std::pair<Thread::T_enThreadStatus, std::vector<Lua::Value>> Thread::Resume(const std::vector<Lua::Value>& vecParam)
{
   ATLASSERT(Status() == statusYield);

   if (Status() != statusYield)
      throw Lua::Exception(_T("Lua thread attempted to resume hasn't yielded"), m_threadState.GetState(), __FILE__, __LINE__);

   return InternalResume(vecParam);
}

void Thread::Yield(State& localParamState, const std::vector<Value>& vecRetValues, T_fnYieldCallback fnYieldCallback)
{
   lua_State* L = m_threadState.GetState();

   if (!lua_isyieldable(L))
      throw Lua::Exception(_T("Lua thread isn't yieldable"), L, __FILE__, __LINE__);

   std::for_each(vecRetValues.begin(), vecRetValues.end(),
      [&](const Value& value) { value.Push(m_threadState); value.Detach(); });

   m_state.DetachAll();
   m_threadState.DetachAll();
   localParamState.DetachAll();

   lua_KContext context = 0;
   lua_KFunction func = nullptr;

   if (fnYieldCallback != nullptr)
   {
      context = reinterpret_cast<lua_KContext>(new T_fnYieldCallback(fnYieldCallback));
      func = &Thread::InternalYield;
   }

   // bug? lua_yieldk only returns n-1 values to lua_resume, so add one
   lua_yieldk(L, vecRetValues.size() + 1, context, func);

   throw Lua::Exception(_T("Yield() continued after calling lua_yieldk"), L, __FILE__, __LINE__);
}

std::pair<Thread::T_enThreadStatus, std::vector<Lua::Value>> Thread::InternalResume(const std::vector<Lua::Value>& vecParam)
{
   // L is the thread stack here
   lua_State* L = m_threadState.GetState();

   // determine last stack value we don't use; when starting new thread, don'T remove pushed function
   int iIndexLastValue = lua_gettop(L);
   if (Status() != statusYield)
      iIndexLastValue--;

   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(m_threadState); value.Detach(); });

   // since lua_resume removes everything from stack, detach all values still referenced on the stack
   m_threadState.DetachAll();

   // remove all values below our parameters (and pushed function, when we start a new thread)
   for (int iIndex = iIndexLastValue; iIndex >= 1; iIndex--)
      lua_remove(m_threadState.GetState(), iIndex);

   int iRet = lua_resume(L, m_state.GetState(), vecParam.size());

   if (iRet == LUA_ERRRUN || // a runtime error.
      iRet == LUA_ERRMEM || // memory allocation error. For such errors, Lua does not call the message handler.
      iRet == LUA_ERRERR || // error while running the message handler.
      iRet == LUA_ERRGCMM) // error while running a __gc metamethod
   {
      State::OnLuaPanic(L);
   }

   State::TraceStack(L);

   std::pair<Thread::T_enThreadStatus, std::vector<Lua::Value>> retVal;
   retVal.first = static_cast<Thread::T_enThreadStatus>(iRet);

   std::vector<Value>& vecResults = retVal.second;

   // collect return values; since we removed all excess values on stack,
   // always start at the first element.
   int iTopIndex = lua_gettop(L);

   for (int i = 1; i <= iTopIndex; i++)
      vecResults.push_back(Value::FromStack(m_threadState, i));

   return retVal;
}

int Thread::InternalYield(lua_State* L, int status, lua_KContext context)
{
   ATLASSERT(status == LUA_YIELD); status; // Lua docs says it always passes this value

   State::TraceStack(L);

   T_fnYieldCallback* pfnCallback = reinterpret_cast<T_fnYieldCallback*>(context);
   if (pfnCallback == nullptr)
      return 0; // InternalResume() called without a continuation function

   State paramState(L, false);

   // collect params for continuation
   std::vector<Lua::Value> vecParams;

   int iTopIndex = lua_gettop(L);

   for (int iStackIndex = 1; iStackIndex <= iTopIndex; iStackIndex++)
      vecParams.push_back(Value::FromStack(paramState, iStackIndex));

   std::vector<Lua::Value> vecResults;
   try
   {
      // call continuation
      vecResults = (*pfnCallback)(paramState, vecParams);

      // put results on stack
      std::for_each(vecResults.begin(), vecResults.end(),
         [&](const Value& value) { value.Push(paramState); value.Detach(); });
   }
   catch (const Lua::Exception& ex)
   {
      delete pfnCallback;

      lua_pushstring(L, CStringA(ex.Message()));
      throw;
   }
   catch (const std::exception& ex)
   {
      delete pfnCallback;

      lua_pushstring(L, ex.what());
      throw;
   }
   catch (...)
   {
      delete pfnCallback;

      lua_pushstring(L, "unknown C++ exception thrown");
      throw;
   }

   delete pfnCallback;

   return vecResults.size();
}

void Thread::Push()
{
   lua_pushthread(m_threadState.GetState());
}

void Thread::Detach() const
{
   m_spRef->Detach();
   m_spRef.reset();
}
