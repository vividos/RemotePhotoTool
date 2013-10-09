#pragma once

#include <functional>
#include <memory>
#include <vector>

// forward reference
struct lua_State;

namespace Lua
{

class Value
{
public:
};

class Binding
{
public:
};

/// c function that can be registered as binding
typedef std::function<std::vector<Value>(const std::vector<Value>&)> T_fnCFunction;

/// Lua state
class State
{
public:
   /// ctor
   State();

   /// loads Lua code from file
   void LoadFile(const CString& cszFilename);

   /// loads Lua code from source code string
   void LoadSourceString(const CString& cszLuaSource);

   /// calls a function
   std::vector<Value> CallFunction(const CString& cszName, const std::vector<Value>& vecParam);

   /// binds new function
   Binding BindFunction(const CString& cszName, T_fnCFunction fnFunction);

private:
   /// Lua state
   std::shared_ptr<lua_State> m_spState;
};

}; // namespace Lua
