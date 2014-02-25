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
   CString cszInfo;

   for (int iStack = 1; ; iStack++)
   {
      lua_Debug debug = {0};

      int iRet = lua_getstack(L, iStack, &debug);
      if (iRet == 0)
         break;

      lua_getinfo(L, "S", &debug);

      // TODO format info
      //debug.source
      cszInfo += _T("\n");
   }

   //for (int i=1; i<5; i++)
   //{
   //   luaL_where(L, i);
   //   cszInfo += lua_tostring(L, -1);
   //   cszInfo += _T("\n");
   //   lua_pop(L, 1);
   //}

   cszInfo.Trim();

   CString cszText;
   cszText.Format(_T("Lua error: %s in %s"), cszMessage.GetString(), cszInfo.GetString());
   return cszText;
}

//
// Lua::Value
//

void Value::Push(State& state) const
{
   lua_State* L = state.GetState();

   boost::any m_value;
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

   case typeFunction:
   case typeUserdata:
   case typeTable:
   default:
      ATLASSERT(false);
      break;
   }
}

Value Value::FromStack(lua_State* L, int iIndex)
{
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

   default:
      ATLASSERT(false); // type can't be converted into Value
      break;
   }

   return Value();
}

//
// Lua::Table
//

Table::Table(State& state, const CString& cszName)
:Value(Value::typeTable),
 m_state(state),
 m_cszName(cszName)
{
   lua_State* L = m_state.GetState();
   lua_pushstring(L, CStringA(cszName));
   lua_createtable(L, 0, 0);
}

Table::~Table() throw()
{
}

Table& Table::AddValue(const CString& key, const Value& value)
{
   value.Push(m_state);
   lua_setfield(m_state.GetState(), -1, CStringA(key));

   return *this;
}

Table& Table::AddFunction(LPCSTR /*pszaName*/, T_fnCFunction /*fn*/)
{
   // TODO
   return *this;
}

State::State()
{
   lua_State* L = luaL_newstate();
   m_spState.reset(L, &lua_close);

   lua_atpanic(L, OnLuaPanic);
}

void State::LoadFile(const CString& cszFilename)
{
   CStringA cszaFilename(cszFilename);
   luaL_dofile(m_spState.get(), cszaFilename.GetString());
}

void State::LoadSourceString(const CString& cszLuaSource)
{
   CStringA cszaLuaSource(cszLuaSource);
   luaL_dostring(m_spState.get(), cszaLuaSource.GetString());
}

std::vector<Value> State::CallFunction(const CString& cszName, int iResults, const std::vector<Value>& vecParam)
{
   CStringA cszaName(cszName);

   std::for_each(vecParam.begin(), vecParam.end(), [&](const Value& value){ value.Push(*this); });

   lua_State* L = m_spState.get();

   lua_getglobal(L, CStringA(cszName).GetString());
   if (!lua_isfunction(L, -1))
      throw Lua::Exception(_T("function not found: ") + cszName, L);

   lua_call(L, vecParam.size(), iResults);

   // collect return values
   std::vector<Value> vecResults;
   for (int i=-iResults; i<=-1; i++)
      vecResults.push_back(Value::FromStack(L, i));

   return vecResults;
}

int State::OnLuaPanic(lua_State* L)
{
   CString cszErrorMessage = lua_tostring(L, -1);
   throw Lua::Exception(cszErrorMessage, L);
}
