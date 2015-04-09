//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Lua.cpp Lua wrapper classes
//

// includes
#include "StdAfx.h"
#include "Lua.hpp"

#include <lua.h>
#include <lauxlib.h>

using namespace Lua;

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
      m_cszLuaErrorMessage = cszText.Mid(iPos2+1);
      m_cszLuaErrorMessage.Trim();

      m_uiLuaLineNumber = static_cast<UINT>(_tcstoul(cszText.Mid(iPos1+1, iPos2-iPos1-1), NULL, 10));
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

Value::Value(Table table)
:m_enType(typeTable),
 m_value(table)
{
}

Value::Value(Function func)
:m_enType(typeFunction),
 m_value(func)
{
}


void Value::Push(State& state) const
{
   lua_State* L = state.GetState();

   switch (m_enType)
   {
   case typeNil:
      lua_pushnil(L);
      break;

   case typeBoolean:
      lua_pushboolean(L, boost::any_cast<bool>(m_value) ? 1 : 0);
      break;

   case typeNumber:
      lua_pushnumber(L, boost::any_cast<double>(m_value));
      break;

   case typeInteger:
      lua_pushinteger(L, boost::any_cast<int>(m_value));
      break;

   case typeString:
      lua_pushstring(L, CStringA(boost::any_cast<CString>(m_value)));
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
   default:
      ATLASSERT(false);
      break;
   }
}

Value Value::FromStack(State& state, int iIndex)
{
   lua_State* L = state.GetState();

   switch (lua_type(L, iIndex))
   {
   case LUA_TNONE:
   case LUA_TNIL:
      return Value();
      break;

   case LUA_TNUMBER:
      return Value(lua_tonumber(L, iIndex));

   case LUA_TBOOLEAN:
      return Value(lua_toboolean(L, iIndex) != 0);

   case LUA_TSTRING:
      return Value(lua_tostring(L, iIndex));

   case LUA_TTABLE:
      return Value(Table(state, iIndex, false));

   case LUA_TFUNCTION:
      return Value(Function(state, iIndex));

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
   FuncData(State& state, T_fnCFunction fn)
      :m_state(state),
       m_fn(fn)
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

   /// Lua state
   State& m_state;

   /// bound C++ function
   T_fnCFunction m_fn;
};

void FuncData::PushCClosure(State& state, T_fnCFunction fn)
{
   lua_State* L = state.GetState();

   // add userdata
   void* pUserdata = lua_newuserdata(L, sizeof(FuncData*));
   FuncData** pFunction = reinterpret_cast<FuncData**>(pUserdata);
   *pFunction = new FuncData(state, fn);

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

   std::vector<Value> vecParams;
   for (int i=1; i <= iStackDepth; i++)
      vecParams.push_back(Value::FromStack(data.m_state, i));

   State::TraceUpvalues(L);

   // call bound function
   std::vector<Value> vecRetVals = data.m_fn(vecParams);

   // push all params
   std::for_each(vecRetVals.begin(), vecRetVals.end(), [&](const Value& value){ value.Push(data.m_state); });

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

Function::~Function()
{
}

std::vector<Value> Function::Call(int iResults, const std::vector<Value>& vecParam)
{
   lua_State* L = m_state.GetState();

   lua_pushvalue(L, m_iStackIndex);

   std::for_each(vecParam.begin(), vecParam.end(), [&](const Value& value){ value.Push(m_state); });

   lua_call(L, vecParam.size(), iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i=-iResults; i<=-1; i++)
      vecResults.push_back(Value::FromStack(m_state, i));

   lua_pop(L, 1); // remove copied function

   return vecResults;
}

void Function::Push()
{
   lua_State* L = m_state.GetState();

   lua_pushvalue(L, m_iStackIndex);
}

Function::Function(State& state, int iStackIndex)
:m_state(state),
 m_iStackIndex(lua_absindex(state.GetState(), iStackIndex))
{
}



//
// Lua::Table
//

Table::Table(State& state, const CString& cszName)
:m_state(state),
 m_cszName(cszName),
 m_bCreating(true),
 m_bTemporary(true),
 m_iStackIndex(-1)
{
   lua_State* L = m_state.GetState();
   lua_newtable(L);

   m_iStackIndex = lua_absindex(L, -1);
}

Table::Table(State& state, int iStackIndex, bool bTemporary)
:m_state(state),
 m_bCreating(false),
 m_bTemporary(bTemporary),
 m_iStackIndex(lua_absindex(state.GetState(), iStackIndex))
{
}

Table::~Table() throw()
{
   lua_State* L = m_state.GetState();

   if (m_bCreating)
   {
      if (m_cszName.IsEmpty())
         ; // TODO what to do?
      else
      {
         try
         {
            lua_setglobal(L, CStringA(m_cszName));
         }
         catch (...)
         {
         }
      }
   }
   else
   if (m_bTemporary)
   {
      ATLASSERT(lua_istable(L, m_iStackIndex) == true);
      lua_remove(L, m_iStackIndex);
   }
}

Table::Table(const Table& table)
:m_state(table.m_state),
 m_bCreating(table.m_bCreating),
 m_bTemporary(table.m_bTemporary),
 m_iStackIndex(table.m_iStackIndex)
{
   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Table&>(table).m_bCreating = false;
   const_cast<Table&>(table).m_bTemporary = false;
}

Table& Table::operator=(const Table& table)
{
   m_state = table.m_state,
   m_bCreating = table.m_bCreating;
   m_bTemporary = table.m_bTemporary;
   m_iStackIndex = table.m_iStackIndex;

   // now that the values are copied, prevent creating or deleting the source object
   const_cast<Table&>(table).m_bCreating = false;
   const_cast<Table&>(table).m_bTemporary = false;

   return *this;
}

Table& Table::AddValue(const CString& key, const Value& value)
{
   lua_State* L = m_state.GetState();

   ATLASSERT(lua_istable(L, m_iStackIndex) == true);

   value.Push(m_state);
   lua_setfield(L, m_iStackIndex, CStringA(key));

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

void Table::Push()
{
   lua_State* L = m_state.GetState();

   if (m_bCreating)
   {
      // nothing to do; already on the stack

      m_bCreating = false;
      m_bTemporary = false;
   }
   else
   {
      if (m_bTemporary)
      {
         lua_pushvalue(L, m_iStackIndex);
         m_bTemporary = false;
      }
      else
      {
         lua_pushvalue(L, m_iStackIndex);
      }
   }
}

Value Table::GetValue(const CString& key)
{
   lua_State* L = m_state.GetState();

   lua_pushstring(L, CStringA(key));

   lua_gettable(L, -2);

   return Value::FromStack(m_state, -1);
}

std::vector<Value> Table::CallFunction(const CString& cszName,
   int iResults,
   const std::vector<Value>& vecParam)
{
   // must not call table function when currently creating table! (use GetTable() for that)
   ATLASSERT(m_bCreating == false);

   lua_State* L = m_state.GetState();

   lua_getfield(L, m_iStackIndex, CStringA(cszName).GetString());
   if (!lua_isfunction(L, -1))
      throw Lua::Exception(_T("function not found: ") + cszName, L, __FILE__, __LINE__);

   // add table as first argument
   lua_pushvalue(L, m_iStackIndex);

   // then params
   std::for_each(vecParam.begin(), vecParam.end(), [&](const Value& value){ value.Push(m_state); });

   lua_call(L, vecParam.size()+1, iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i=-iResults; i<=-1; i++)
      vecResults.push_back(Value::FromStack(m_state, i));

   return vecResults;
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

void State::LoadFile(const CString& cszFilename)
{
   CStringA cszaFilename(cszFilename);

   lua_State* L = GetState();

   int iRet = luaL_dofile(L, cszaFilename.GetString());

   if (iRet != 0)
      throw Lua::Exception(_T("error while loading file"), lua_tostring(L, -1), GetState());
}

void State::LoadSourceString(const CString& cszLuaSource)
{
   CStringA cszaLuaSource(cszLuaSource);

   lua_State* L = GetState();

   int iRet = luaL_dostring(L, cszaLuaSource.GetString());

   if (iRet != 0)
      throw Lua::Exception(_T("error while loading source string"), lua_tostring(L, -1), GetState());
}

std::vector<Value> State::CallFunction(const CString& cszName, int iResults, const std::vector<Value>& vecParam)
{
   lua_State* L = GetState();

   lua_getglobal(L, CStringA(cszName).GetString());
   if (!lua_isfunction(L, -1))
      throw Lua::Exception(_T("function not found: ") + cszName, L, __FILE__, __LINE__);

   std::for_each(vecParam.begin(), vecParam.end(), [&](const Value& value){ value.Push(*this); });

   lua_call(L, vecParam.size(), iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i=-iResults; i<=-1; i++)
      vecResults.push_back(Value::FromStack(*this, i));

   return vecResults;
}

Table State::AddTable(const CString& cszName)
{
   return Table(*this, cszName);
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
      throw Lua::Exception(_T("table not found: ") + cszName, L, __FILE__, __LINE__);

   return Table(*this, -1, true);
}

void State::CollectGarbage()
{
   lua_State* L = GetState();

   lua_gc(L, LUA_GCCOLLECT, 0);
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

   for (int iIndex=1; iIndex <= iStackDepth; iIndex++)
   {
      TraceValue(L, iIndex, iStackDepth);
   }

   ATLASSERT(iStackDepth == lua_gettop(L));

   ATLTRACE(_T("End tracing stack.\n"));
}

void State::TraceUpvalues(lua_State* L)
{
   ATLTRACE(_T("Tracing upvalues:\n"));

   for (int iUpvalueIndex = 1; iUpvalueIndex<256; iUpvalueIndex++)
   {
      int iIndex = lua_upvalueindex(iUpvalueIndex);

      if (lua_type(L, iIndex) == LUA_TNONE)
         continue;

      TraceValue(L, iIndex, 256);
   }

   ATLTRACE(_T("End tracing upvalues.\n"));
}

void State::TraceValue(lua_State* L, int iIndex, int iStackDepth)
{
   iStackDepth;

   CString cszType(lua_typename(L, lua_type(L, iIndex)));

   CString cszContent(_T("???"));
   switch (lua_type(L, iIndex))
   {
   case LUA_TNONE: cszContent = _T("none"); break;
   case LUA_TNIL:  cszContent = _T("[nil]"); break;
   case LUA_TNUMBER: cszContent.Format(_T("%f"), lua_tonumber(L, iIndex)); break;
   case LUA_TBOOLEAN: cszContent = lua_toboolean(L, iIndex) != 0 ? _T("true") : _T("false"); break;
   case LUA_TSTRING: cszContent.Format(_T("[%hs]"), lua_tostring(L, iIndex)); break;
   case LUA_TTABLE:
      {
         lua_len(L, iIndex);
         int iLen = lua_tointeger(L, -1);
         lua_pop(L, 1);

         cszContent.Format(_T("[table, len=%i]"), iLen);
      }
      break;
   case LUA_TFUNCTION: cszContent.Format(_T("&%p"), lua_tocfunction(L, iIndex)); break;
   case LUA_TLIGHTUSERDATA: cszContent.Format(_T("0x%p"), lua_touserdata(L, iIndex)); break;
   case LUA_TUSERDATA: cszContent.Format(_T("&%p"), lua_touserdata(L, iIndex)); break;
   case LUA_TTHREAD: cszContent = _T("[thread]"); break;
   default:
      ATLASSERT(false);
      break;
   }

   int iHasMetatable = lua_getmetatable(L, iIndex);
   if (iHasMetatable != 0)
   {
      lua_len(L, -1);
      //int iLenMetatable = lua_tointeger(L, -1);
      lua_pop(L, 2);
   }

   ATLTRACE(_T("[%i] [%i] %s %s%s\n"),
      iIndex,
      iIndex - iStackDepth - 1,
      cszType.GetString(),
      cszContent.GetString(),
      iHasMetatable != 0 ? _T(" +[metatable] ") : _T(""));
}
