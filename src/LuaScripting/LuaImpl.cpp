#include "StdAfx.h"
#include "Lua.hpp"

extern "C"
{
   #include <lua.h>
   #include <lauxlib.h>
}

using Lua::State;
using Lua::Value;


State::State()
{
   lua_State* ls = luaL_newstate();
   m_spState.reset(ls, &lua_close);
}

void State::LoadFile(const CString& /*cszFilename*/)
{
   // TODO impl
   //lua_Reader reader;
   //lua_load(m_spState.get(), reader, );
}

void State::LoadSourceString(const CString& /*cszLuaSource*/)
{
   // TODO impl
}

std::vector<Value> State::CallFunction(const CString& cszName, const std::vector<Value>& vecParam)
{
   CStringA cszaName(cszName);

   lua_pushstring(m_spState.get(), cszaName.GetString());
   lua_call(m_spState.get(), vecParam.size(), 0);

   // TODO impl
   return std::vector<Value>();
}

Lua::Binding State::BindFunction(const CString& /*cszName*/, Lua::T_fnCFunction /*fnFunction*/)
{
   // TODO impl
   return Lua::Binding();
}
