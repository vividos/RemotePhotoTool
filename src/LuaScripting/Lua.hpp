//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Lua.hpp Lua wrapper classes
//
#pragma once

// includes
#include <functional>
#include <memory>
#include <vector>
#include <boost/any.hpp>
#include "Exception.hpp"

// forward references
struct lua_State;

namespace Lua
{
class State;

/// Lua exception class
class Exception: public ::Exception
{
   /// base class type
   typedef ::Exception Base;

public:
   /// ctor
   Exception(const CString& cszMessage, lua_State* L)
      :Base(MessageFromState(cszMessage, L), "", 0)
   {
   }

private:
   /// formats message from Lua state
   static CString MessageFromState(const CString& cszMessage, lua_State* L) throw();
};

/// Lua value
class Value
{
public:
   enum T_enType
   {
      typeNil,
      typeBoolean,
      typeNumber,
      typeInteger,
      typeString,
      typeFunction,
      typeUserdata,
      typeTable,
   };

   /// default ctor; constructs nil value
   Value()
      :m_enType(typeNil)
   {
   }

   /// ctor for boolean value
   explicit Value(bool bValue)
      :m_value(bValue),
       m_enType(typeBoolean)
   {
   }

   /// ctor for number value
   explicit Value(double dValue)
      :m_value(dValue),
       m_enType(typeNumber)
   {
   }

   /// ctor for integer value
   explicit Value(int iValue)
      :m_value(iValue),
       m_enType(typeInteger)
   {
   }

   /// ctor for string value
   explicit Value(const CString& cszText)
      :m_value(cszText),
       m_enType(typeString)
   {
   }

   /// ctor for ANSI string value
   explicit Value(LPCSTR pszText)
      :m_value(pszText),
       m_enType(typeString)
   {
   }

   /// pushes value to stack
   void Push(State& state) const;

   /// returns type
   T_enType GetType() const throw() { return m_enType; }

   /// constructs Value object from stack entry
   static Value FromStack(lua_State* L, int iIndex);

protected:
   /// ctor; used to set type for derived classes
   explicit Value(T_enType enType)
      :m_enType(enType)
   {
   }

private:
   /// value
   boost::any m_value;

   /// type
   T_enType m_enType;
};

/// c function that can be registered as binding
typedef std::function<std::vector<Value>(const std::vector<Value>&)> T_fnCFunction;

/// Lua C++ callable function
/// \tparam Signature C++ function signature
template <typename Signature>
class Function: public Value
{
public:
   /// C++ function signature type
   typedef Signature T_fnSignature;

   /// ctor
   Function(T_fnSignature fn)
      :Value(Value::typeFunction),
       m_fn(fn)
   {
   }

private:
   /// function
   T_fnSignature m_fn;
};

/// Lua table
class Table: public Value
{
public:
   /// dtor
   ~Table();

   /// adds a value to the table
   Table& AddValue(const CString& key, const Value& value);

   /// adds a function to the table
   Table& AddFunction(LPCSTR pszaName, T_fnCFunction fn);

private:
   friend class State;

   /// ctor; can only be constructed by Lua::State
   Table(State& state, const CString& cszName);

private:
   /// state the table belongs to
   State& m_state;

   /// table name
   CString m_cszName;
};


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
   std::vector<Value> CallFunction(const CString& cszName,
      int iResults = 0,
      const std::vector<Value>& vecParam = std::vector<Value>());

   /// adds an empty table to the state
   Table AddTable(const CString& cszName);

   /// adds a global function to the state
   //Function AddFunction(const CString& cszName);

private:
   friend Value;
   friend Table;

   /// returns state
   lua_State* GetState() { return m_spState.get(); }

   /// panic error handler
   static int OnLuaPanic(lua_State* L);

private:
   /// Lua state
   std::shared_ptr<lua_State> m_spState;
};

}; // namespace Lua
