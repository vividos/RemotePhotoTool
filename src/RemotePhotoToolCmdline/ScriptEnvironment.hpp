#pragma once

#include "Instance.hpp"
#include "..\LuaScripting\Lua.hpp"

class ScriptEnvironment
{
public:
   ScriptEnvironment();
   void Init();

   void Run(const CString& cszFilename);

private:
   Instance m_instance;
   Lua::State m_luaState;
};
