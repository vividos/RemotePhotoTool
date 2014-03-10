#include "StdAfx.h"
#include "ScriptEnvironment.hpp"

ScriptEnvironment::ScriptEnvironment()
:m_instance(Instance::Get())
{
}

void ScriptEnvironment::Init()
{
   // TODO create all bindings
}

void ScriptEnvironment::Run(const CString& cszFilename)
{
   m_luaState.LoadFile(cszFilename);

   m_luaState.CallFunction(_T("Main"), 0, std::vector<Lua::Value>());
}
