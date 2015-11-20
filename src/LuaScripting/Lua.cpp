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
// Lua::Value
//

/// default ctor; constructs nil value
Value::Value()
   :m_enType(typeNil),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
}

Value::Value(bool bValue)
   :m_value(bValue),
   m_enType(typeBoolean),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
}

Value::Value(double dValue)
   :m_value(dValue),
   m_enType(typeNumber),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
}

Value::Value(int iValue)
   :m_value(iValue),
   m_enType(typeInteger),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
}

Value::Value(const CString& cszText)
   :m_value(CStringA(cszText).GetString()),
   m_enType(typeString),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
}

Value::Value(LPCSTR pszText)
   :m_value(pszText),
   m_enType(typeString),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
}

Value::Value(Table table)
   :m_enType(typeTable),
   m_value(table),
   m_iStackIndex(-1),
   m_bTemporary(false) // table cleans itself up
{
}

Value::Value(Userdata userdata)
   :m_enType(typeUserdata),
   m_value(userdata),
   m_iStackIndex(-1),
   m_bTemporary(false) // userdata cleans itself up
{
}

Value::Value(Function func)
   :m_enType(typeFunction),
   m_value(func),
   m_iStackIndex(-1),
   m_bTemporary(false) // function cleans itself up
{
}

Value::Value(Thread thread)
   :m_enType(typeThread),
   m_value(thread),
   m_iStackIndex(-1),
   m_bTemporary(false) // thread cleans itself up
{
}

Value::Value(const Value& val)
   :m_value(val.m_value),
   m_enType(val.m_enType),
   m_state(val.m_state),
   m_iStackIndex(val.m_iStackIndex),
   m_bTemporary(val.m_bTemporary)
{
   // now that the values are copied, prevent deleting the source object
   const_cast<Value&>(val).m_bTemporary = false;

   if (m_state.is_initialized())
      m_state.get().get().AddRef(*this);

   if (val.m_state.is_initialized())
      val.m_state.get().get().RemoveRef(val);
}

Value& Value::operator=(const Value& val)
{
   m_value = val.m_value;
   m_enType = val.m_enType;
   m_state = val.m_state;
   m_iStackIndex = val.m_iStackIndex;
   m_bTemporary = val.m_bTemporary;

   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Value&>(val).m_bTemporary = false;

   if (m_state.is_initialized())
      m_state.get().get().AddRef(*this);

   if (val.m_state.is_initialized())
      val.m_state.get().get().RemoveRef(val);

   return *this;
}

Value::~Value() throw()
{
   if (m_bTemporary &&
      m_iStackIndex != -1 &&
      m_state.is_initialized())
   {
      try
      {
         m_state.get().get().Remove(*this, m_iStackIndex);

         m_state.reset();
      }
      catch (...)
      {
      }
   }
}

void Value::Push(State& state) const
{
   lua_State* L = state.GetState();

   switch (m_enType)
   {
   case typeNil:
      lua_pushnil(L);
      Attach(state, -1, false);
      break;

   case typeBoolean:
      lua_pushboolean(L, boost::any_cast<bool>(m_value) ? 1 : 0);
      Attach(state, -1, false);
      break;

   case typeNumber:
      lua_pushnumber(L, boost::any_cast<double>(m_value));
      Attach(state, -1, false);
      break;

   case typeInteger:
      lua_pushinteger(L, boost::any_cast<int>(m_value));
      Attach(state, -1, false);
      break;

   case typeString:
      lua_pushstring(L, CStringA(boost::any_cast<LPCSTR>(m_value)));
      Attach(state, -1, false);
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

   default:
      ATLASSERT(false);
      break;
   }
}

void Value::Detach() const
{
   m_state.reset();
   m_iStackIndex = -1;

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
   }
}

void Value::OnRemoveStackValue(int iStackIndex)
{
   if (m_iStackIndex == -1 ||
      !m_state.is_initialized())
      return;

   if (iStackIndex == m_iStackIndex)
      Detach(); // it's our own value
   else
      if (m_iStackIndex >= iStackIndex)
         m_iStackIndex--;
}

Value Value::FromStack(State& state, int iIndex, bool bTemporary)
{
   lua_State* L = state.GetState();

   if (iIndex <= 0)
      iIndex = lua_absindex(L, iIndex);

   switch (lua_type(L, iIndex))
   {
   case LUA_TNONE:
   case LUA_TNIL:
   {
      Value val;
      val.Attach(state, iIndex, bTemporary);
      state.AddRef(val);
      return val;
   }

   case LUA_TNUMBER:
   {
      Value val(lua_tonumber(L, iIndex));
      val.Attach(state, iIndex, bTemporary);
      state.AddRef(val);
      return val;
   }

   case LUA_TBOOLEAN:
   {
      Value val(lua_toboolean(L, iIndex) != 0);
      val.Attach(state, iIndex, bTemporary);
      state.AddRef(val);
      return val;
   }

   case LUA_TSTRING:
   {
      Value val(lua_tostring(L, iIndex));
      val.Attach(state, iIndex, bTemporary);
      state.AddRef(val);
      return val;
   }

   case LUA_TTABLE:
      return Value(Table(state, iIndex, bTemporary, CString()));

   case LUA_TFUNCTION:
      return Value(Function(state, iIndex, bTemporary));

   case LUA_TUSERDATA:
      return Value(Userdata(state, iIndex, bTemporary));

   case LUA_TTHREAD:
      return Value(Thread(state, iIndex, bTemporary));

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

   ATLASSERT(lua_isuserdata(L, -1) == 1);

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
      vecParams.push_back(Value::FromStack(paramState, i, false));

   State::TraceUpvalues(L);

   // call bound function
   std::vector<Value> vecRetVals = data.m_fn(paramState, vecParams);

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

Function::Function(State& state, int iStackIndex, bool bTemporary)
   :m_state(state),
   m_bTemporary(bTemporary),
   m_iStackIndex(lua_absindex(state.GetState(), iStackIndex))
{
}

Function::Function(const Function& func)
   :m_state(func.m_state),
   m_iStackIndex(func.m_iStackIndex),
   m_bTemporary(func.m_bTemporary)
{
   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Function&>(func).m_bTemporary = false;
}

Function& Function::operator=(const Function& func)
{
   m_state = func.m_state;
   m_iStackIndex = func.m_iStackIndex;
   m_bTemporary = func.m_bTemporary;

   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Function&>(func).m_bTemporary = false;

   return *this;
}

Function::~Function() throw()
{
   if (m_bTemporary && m_iStackIndex != -1)
   {
      try
      {
         lua_State* L = m_state.GetState();

         ATLASSERT(lua_isfunction(L, m_iStackIndex) == true);
         m_state.RemoveIndex(m_iStackIndex);
      }
      catch (...)
      {
      }
   }
}

std::vector<Value> Function::Call(int iResults, const std::vector<Value>& vecParam)
{
   lua_State* L = m_state.GetState();

   lua_pushvalue(L, m_iStackIndex);

   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(m_state); value.Detach(); });

   lua_call(L, vecParam.size(), iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i = -iResults; i <= -1; i++)
      vecResults.push_back(Value::FromStack(m_state, i, true));

   return vecResults;
}

void Function::Push()
{
   lua_State* L = m_state.GetState();

   lua_pushvalue(L, m_iStackIndex);
}

void Function::Detach() const
{
   m_iStackIndex = -1;
}

//
// Lua::Table
//

Table::Table(State& state, const CString& cszName)
   :m_state(state),
   m_cszName(cszName),
   m_iStackIndex(-1),
   m_bTemporary(true)
{
   lua_State* L = m_state.GetState();

   lua_newtable(L);

   if (!cszName.IsEmpty())
   {
      // no temporary (empty name) table? then set and get again
      lua_setglobal(L, CStringA(m_cszName));

      lua_getglobal(L, CStringA(m_cszName).GetString());
   }

   m_iStackIndex = lua_absindex(L, -1);
}

Table::Table(State& state, int iStackIndex, bool bTemporary, const CString& cszName)
   :m_state(state),
   m_cszName(cszName),
   m_bTemporary(bTemporary),
   m_iStackIndex(lua_absindex(state.GetState(), iStackIndex))
{
}

Table::Table(const Table& table)
   :m_state(table.m_state),
   m_cszName(table.m_cszName),
   m_bTemporary(table.m_bTemporary),
   m_iStackIndex(table.m_iStackIndex)
{
   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Table&>(table).m_bTemporary = false;
}

Table& Table::operator=(const Table& table)
{
   m_state = table.m_state;
   m_bTemporary = table.m_bTemporary;
   m_iStackIndex = table.m_iStackIndex;

   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Table&>(table).m_bTemporary = false;

   return *this;
}

Table::~Table() throw()
{
   if (m_bTemporary && m_iStackIndex != -1)
   {
      try
      {
         lua_State* L = m_state.GetState();

         ATLASSERT(lua_istable(L, m_iStackIndex) == true);
         m_state.RemoveIndex(m_iStackIndex);
      }
      catch (...)
      {
      }
   }
}

Table& Table::AddValue(const CString& key, const Value& value)
{
   lua_State* L = m_state.GetState();

   ATLASSERT(lua_istable(L, m_iStackIndex) != 0);

   value.Push(m_state);

   lua_setfield(L, m_iStackIndex, CStringA(key));

   value.Detach();

   return *this;
}

Table& Table::AddValue(int key, const Value& value)
{
   lua_State* L = m_state.GetState();

   ATLASSERT(lua_istable(L, m_iStackIndex) != 0);

   value.Push(m_state);

   lua_seti(L, m_iStackIndex, lua_Integer(key));

   value.Detach();

   return *this;
}

Table& Table::AddFunction(LPCSTR pszaName, T_fnCFunction fn)
{
   lua_State* L = m_state.GetState();

   ATLASSERT(lua_istable(L, m_iStackIndex) == true);

   FuncData::PushCClosure(m_state, fn);

   lua_setfield(L, m_iStackIndex, pszaName);

   return *this;
}

Value Table::GetValue(const CString& key)
{
   lua_State* L = m_state.GetState();

   lua_pushstring(L, CStringA(key));

   lua_gettable(L, m_iStackIndex);

   return Value::FromStack(m_state, -1, true);
}

std::vector<Value> Table::CallFunction(const CString& cszName,
   int iResults,
   const std::vector<Value>& vecParam)
{
   lua_State* L = m_state.GetState();

   lua_getfield(L, m_iStackIndex, CStringA(cszName).GetString());
   if (!lua_isfunction(L, -1))
   {
      lua_pop(L, 1);
      throw Lua::Exception(_T("function not found: ") + cszName, L, __FILE__, __LINE__);
   }

   // add table as first argument
   lua_pushvalue(L, m_iStackIndex);

   // then params
   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(m_state); value.Detach();  });

   try
   {
      lua_call(L, vecParam.size() + 1, iResults);
   }
   catch (const Lua::Exception&)
   {
      // since the exception is re-thrown, detach all params from stack
      m_state.DetachAll();
      throw;
   }

   // collect return values
   std::vector<Value> vecResults;
   for (int i = -iResults; i <= -1; i++)
      vecResults.push_back(Value::FromStack(m_state, i, true));

   return vecResults;
}

void Table::Push()
{
   lua_State* L = m_state.GetState();

   lua_pushvalue(L, m_iStackIndex);

   if (m_bTemporary)
      m_bTemporary = false;
}

void Table::Detach() const
{
   m_iStackIndex = -1;
}

//
// Lua::Userdata
//

Userdata::Userdata(State& state, size_t uiSize)
   :m_state(state),
   m_pUserdata(nullptr),
   m_uiSize(uiSize),
   m_bTemporary(true),
   m_iStackIndex(-1)
{
   lua_State* L = m_state.GetState();

   m_pUserdata = lua_newuserdata(L, uiSize);

   m_iStackIndex = lua_absindex(L, -1);
}

Userdata::Userdata(State& state, int iStackIndex, bool bTemporary)
   :m_state(state),
   m_pUserdata(nullptr),
   m_uiSize(0),
   m_bTemporary(bTemporary),
   m_iStackIndex(lua_absindex(state.GetState(), iStackIndex))
{
   lua_State* L = m_state.GetState();

   ATLASSERT(lua_isuserdata(L, m_iStackIndex) != 0);

   m_pUserdata = lua_touserdata(L, m_iStackIndex);
   m_uiSize = lua_rawlen(L, m_iStackIndex);
}

Userdata::Userdata(const Userdata& userdata)
   :m_state(userdata.m_state),
   m_pUserdata(userdata.m_pUserdata),
   m_uiSize(userdata.m_uiSize),
   m_bTemporary(userdata.m_bTemporary),
   m_iStackIndex(userdata.m_iStackIndex)
{
   lua_State* L = m_state.GetState(); L;
   ATLASSERT(lua_isuserdata(L, m_iStackIndex) != 0);

   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Userdata&>(userdata).m_bTemporary = false;
}

Userdata& Userdata::operator=(const Userdata& userdata)
{
   m_state = userdata.m_state;
   m_pUserdata = userdata.m_pUserdata;
   m_uiSize = userdata.m_uiSize;
   m_bTemporary = userdata.m_bTemporary;
   m_iStackIndex = userdata.m_iStackIndex;

   lua_State* L = m_state.GetState(); L;
   ATLASSERT(lua_isuserdata(L, m_iStackIndex) != 0);

   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Userdata&>(userdata).m_bTemporary = false;

   return *this;
}

Userdata::~Userdata() throw()
{
   if (m_bTemporary && m_iStackIndex != -1)
   {
      try
      {
         lua_State* L = m_state.GetState();

         ATLASSERT(lua_isuserdata(L, m_iStackIndex) != 0);
         m_state.RemoveIndex(m_iStackIndex);
      }
      catch (...)
      {
      }
   }
}

void Userdata::Push()
{
   lua_State* L = m_state.GetState();

   lua_pushvalue(L, m_iStackIndex);

   if (m_bTemporary)
      m_bTemporary = false;
}

void Userdata::Detach() const
{
   m_iStackIndex = -1;
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
      vecResults.push_back(Value::FromStack(*this, i, true));

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

   return Value::FromStack(*this, -1, true);
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

   return Table(*this, -1, true, cszName);
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
   int iStackDepth = lua_gettop(L);

   ATLTRACE(_T("Tracing stack, %i entries:\n"), iStackDepth);

   for (int iIndex = 1; iIndex <= iStackDepth; iIndex++)
      TraceValue(L, iIndex, iStackDepth, false);

   ATLASSERT(iStackDepth == lua_gettop(L));

   ATLTRACE(_T("End tracing stack.\n"));
}

void State::TraceUpvalues(lua_State* L)
{
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

CString EnumTableIndices(lua_State* L, int iIndex)
{
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

void State::Remove(Value& val, int iStackIndex)
{
   RemoveRef(val);
   RemoveIndex(iStackIndex);
}

void State::AddRef(Value& val)
{
   m_vecValuesOnStack.push_back(std::ref(val));
}

bool State::RemoveRef(const Value& val)
{
   auto iter = std::find_if(m_vecValuesOnStack.begin(), m_vecValuesOnStack.end(),
      [&](std::reference_wrapper<Value>& valueRef)
   {
      return &valueRef.get() == &val;
   });

   bool bFound = iter != m_vecValuesOnStack.end();
   if (bFound)
      m_vecValuesOnStack.erase(iter);

   return bFound;
}

void State::RemoveIndex(int iStackIndex)
{
   // adjust all stack indices of referenced values
   std::for_each(m_vecValuesOnStack.begin(), m_vecValuesOnStack.end(),
      [&](std::reference_wrapper<Value>& valueRef)
   {
      valueRef.get().OnRemoveStackValue(iStackIndex);
   });

   lua_State* L = GetState();

   lua_remove(L, iStackIndex);
}

void State::DetachAll()
{
   std::for_each(m_vecValuesOnStack.begin(), m_vecValuesOnStack.end(),
      [&](std::reference_wrapper<Value>& valueRef)
   {
      valueRef.get().Detach();
   });
}

//
// Lua::Thread
//

static_assert(Thread::statusOK == LUA_OK, "value of statusOk must equal LUA_OK");
static_assert(Thread::statusYield == LUA_YIELD, "value of statusOk must equal LUA_YIELD");

/// Lua doesn't pass the number of arguments used in the
/// InternalResume() -> lua_resume() call to the yield function; to notify the
/// InternalYield() function about number of parameters, this static variable is
/// used.
size_t Thread::s_iNumResumeParams = 0;

Thread::Thread(Lua::State& state)
   :m_state(state),
   m_threadState(lua_newthread(state.GetState()), false) // non-mainState
{
}

Thread::Thread(State& state, int iStackIndex, bool bTemporary)
   : m_state(state),
   m_threadState(lua_tothread(state.GetState(), iStackIndex), false) // non-mainState
{
   ATLASSERT(bTemporary == false); bTemporary; // must not create temporary thread object
}

Value Thread::GetValue(const CString& cszName)
{
   lua_State* L = m_threadState.GetState();

   lua_getglobal(L, CStringA(cszName).GetString());

   return Value::FromStack(m_threadState, -1, true);
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

   return Table(m_threadState, -1, true, cszName);
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

void Thread::Yield(const std::vector<Value>& vecRetValues, T_fnYieldCallback fnYieldCallback)
{
   if (!lua_isyieldable(m_threadState.GetState()))
      throw Lua::Exception(_T("Lua thread isn't yieldable"), m_threadState.GetState(), __FILE__, __LINE__);

   State::TraceStack(m_threadState.GetState());

   std::for_each(vecRetValues.begin(), vecRetValues.end(),
      [&](const Value& value) { value.Push(m_threadState); value.Detach(); });

   State::TraceStack(m_threadState.GetState());

   lua_KContext context = 0;
   lua_KFunction func = nullptr;

   if (fnYieldCallback != nullptr)
   {
      context = reinterpret_cast<lua_KContext>(new T_fnYieldCallback(fnYieldCallback));
      func = &Thread::InternalYield;
   }

   lua_yieldk(m_threadState.GetState(), vecRetValues.size(), context, func);

   throw Lua::Exception(_T("Yield() continued after calling lua_yieldk"), m_threadState.GetState(), __FILE__, __LINE__);
}

std::pair<Thread::T_enThreadStatus, std::vector<Lua::Value>> Thread::InternalResume(const std::vector<Lua::Value>& vecParam)
{
   // L is the thread stack here
   lua_State* L = m_threadState.GetState();

   int iBeforeResumeStackIndex = lua_gettop(L);
   if (Status() == statusYield)
      iBeforeResumeStackIndex++;

   std::for_each(vecParam.begin(), vecParam.end(),
      [&](const Value& value) { value.Push(m_threadState); value.Detach(); });

   s_iNumResumeParams = vecParam.size();

   State::TraceStack(L);

   // since lua_resume removes everything from stack, detach all values still referenced on the stack
   m_threadState.DetachAll();

   int iRet = lua_resume(L, m_state.GetState(), vecParam.size());

   if (iRet == LUA_ERRRUN || // a runtime error.
      iRet == LUA_ERRMEM || // memory allocation error. For such errors, Lua does not call the message handler.
      iRet == LUA_ERRERR || // error while running the message handler.
      iRet == LUA_ERRGCMM) // error while running a __gc metamethod
   {
      State::OnLuaPanic(L);
   }

   std::pair<Thread::T_enThreadStatus, std::vector<Lua::Value>> retVal;
   retVal.first = static_cast<Thread::T_enThreadStatus>(iRet);

   State::TraceStack(L);

   // collect return values; note that returning from lua_resume removed
   // all values from stack, and put the return values from Yield() on it.
   std::vector<Value>& vecResults = retVal.second;

   int iTopIndex = lua_gettop(L);
   for (int i = 1; i <= iTopIndex; i++)
      vecResults.push_back(Value::FromStack(m_threadState, i, true));

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

   for (int iStackIndex = iTopIndex - s_iNumResumeParams + 1; iStackIndex <= iTopIndex; iStackIndex++)
      vecParams.push_back(Value::FromStack(paramState, iStackIndex, true));

   std::vector<Lua::Value> vecResults;
   try
   {
      // call continuation
      vecResults = (*pfnCallback)(paramState, vecParams);

      // put results on stack
      std::for_each(vecResults.begin(), vecResults.end(),
         [&](const Value& value) { value.Push(paramState); });
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

//
// Value
//

void Value::Attach(State& state, int iStackIndex, bool bTemporary) const
{
   m_state = std::ref(state);

   lua_State* L = state.GetState();
   m_iStackIndex = lua_absindex(L, iStackIndex);

   const_cast<Value&>(*this).m_bTemporary = bTemporary;
}
