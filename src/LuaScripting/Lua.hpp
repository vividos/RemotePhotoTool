//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file Lua.hpp Lua wrapper classes
//
#pragma once

// includes
#include <functional>
#include <type_traits>
#include <memory>
#include <vector>
#include <boost/any.hpp>
#include <boost/optional.hpp>
#include "Exception.hpp"

// forward references
struct lua_State;
struct FuncData;

// WinBase.h defines Yield()...
#undef Yield

/// Lua C++ wrapper classes
namespace Lua
{
class Value;
class Table;
class Function;
class Userdata;
class State;
class Thread;

/// Lua exception class
class Exception: public ::Exception
{
   /// base class type
   typedef ::Exception Base;

public:
   /// ctor; with message
   Exception(const CString& cszMessage, lua_State* L, LPCSTR pszaFile, UINT uiLine)
      :Base(MessageFromState(cszMessage, L), pszaFile, uiLine),
       m_cszLuaErrorMessage(cszMessage),
       m_cszLuaSourceFile(pszaFile),
       m_uiLuaLineNumber(uiLine)
   {
   }

   /// ctor; with message and Lua error string
   Exception(const CString& cszMessage, LPCSTR pszaLuaError, lua_State* L)
      :Base(MessageFromState(cszMessage, L), "", 0),
       m_uiLuaLineNumber(0)
   {
      ParseLuaError(pszaLuaError);
   }

   /// returns Lua error message
   const CString& LuaErrorMessage() const throw() { return m_cszLuaErrorMessage; }

   /// returns Lua source file name where error occured
   const CString& LuaSourceFile() const throw() { return m_cszLuaSourceFile; }

   /// returns line number where error occured
   UINT LuaLineNumber() const throw() { return m_uiLuaLineNumber; }

private:
   /// formats message from Lua state
   static CString MessageFromState(const CString& cszMessage, lua_State* L) throw();

   /// reads stack trace from Lua state
   static CString ReadStackTrace(lua_State* L) throw();

   /// parses Lua message
   void ParseLuaError(LPCSTR pszaText);

private:
   CString m_cszLuaErrorMessage; ///< Lua error message
   CString m_cszLuaSourceFile;   ///< Lua source file name
   UINT m_uiLuaLineNumber;       ///< source file line number
};

/// checks stack for consistency in dtor (same number of values on stack)
class StackChecker
{
public:
   /// ctor; retrieves current stack depth
   StackChecker(lua_State* L);

   /// dtor; checks stack depth
   ~StackChecker();

private:
   /// Lua state
   lua_State* m_L;

   /// stack depth at ctor
   int m_iStackDepth;
};

/// \brief Reference to a value on the stack
/// \details This is used by the various classes to keep track of the values
/// stored on the stack of a Lua state. When the value isn't referenced
/// anymore, the corresponding value is removed from the stack.
class Ref
{
public:
   /// ctor
   Ref(State& state, int iStackIndex);
   /// dtor
   ~Ref() throw();

   /// returns state
   State& GetState() throw() { return m_state; }

   /// returns stack index of value
   int GetStackIndex() const throw() { return m_iStackIndex; }

private:
   friend Value;
   friend Table;
   friend Function;
   friend Userdata;
   friend State;
   friend Thread;

   /// detaches reference from stack, e.g. because the value was removed from stack by Lua itself
   void Detach();

   /// called when value at given stack index is removed
   void OnRemoveStackValue(int iStackIndex);

private:
   /// the state where the value is stored on
   State& m_state;

   /// absolute stack index
   int m_iStackIndex;
};

/// \brief Lua value
/// Stores a Lua value, e.g. for passing arguments to a function call (see
/// State::CallFunction(), Table::CallFunction(), etc.) or as function return
/// values, e.g. in a C++ closure (see the T_fnCFunction type and where it is
/// used.
class Value
{
public:
   /// Lua value type
   enum T_enType
   {
      typeNil,
      typeBoolean,
      typeNumber,
      typeInteger,
      typeString,
      typeFunction,
      typeTable,
      typeUserdata,
      typeThread,
   };

   /// default ctor; constructs nil value
   Value();

   /// ctor for boolean value
   explicit Value(bool bValue);

   /// ctor for number value
   explicit Value(double dValue);

   /// ctor for integer value
   explicit Value(int iValue);

   /// ctor for string value
   explicit Value(const CString& cszText);

   /// ctor for ANSI string value
   explicit Value(LPCSTR pszText);

   /// ctor; used to wrap Table object
   explicit Value(class Table table);

   /// ctor; used to wrap Userdata object
   explicit Value(class Userdata userdata);

   /// ctor; used to wrap Function object
   explicit Value(class Function func);

   /// ctor; used to wrap Thread object
   explicit Value(class Thread thread);

   /// copy ctor
   Value(const Value& val);

   /// assignment operator
   Value& operator=(const Value& val);

   /// dtor
   ~Value() throw();

   /// returns type
   T_enType GetType() const throw() { return m_enType; }

   /// returns value
   template <typename T>
   T Get() const
   {
      // list of allowed types; if this asserts, you're requesting the wrong type
      static_assert(
         std::is_same<T, bool>::value ||
         std::is_same<T, double>::value ||
         std::is_same<T, int>::value ||
         std::is_same<T, CString>::value ||
         std::is_same<T, LPCTSTR>::value ||
         std::is_same<T, Function>::value ||
         std::is_same<T, Table>::value ||
         std::is_same<T, Userdata>::value ||
         std::is_same<T, Thread>::value, "not an allowed type for Get<T>()");

      return boost::any_cast<T>(m_value);
   }

   /// returns value; specialisation for int
   template <>
   int Get() const
   {
      return static_cast<int>(boost::any_cast<double>(m_value));
   }

   /// returns string value
   template <>
   CString Get() const
   {
      return CString(boost::any_cast<CStringA>(m_value));
   }

   /// returns ref object
   std::shared_ptr<Ref> GetRef() const { return m_spRef; }

private:
   friend FuncData;
   friend Function;
   friend Table;
   friend State;
   friend Thread;

   /// attaches value to state, using reference
   void Attach(std::shared_ptr<Ref> spRef) const;

   /// pushes value to stack
   void Push(State& state) const;

   /// detaches the object from stack
   void Detach() const;

   /// constructs Value object from value at stack index
   static Value FromStack(State& state, int iStackIndex);

private:
   /// value
   boost::any m_value;

   /// type
   T_enType m_enType;

   /// state stack reference
   mutable std::shared_ptr<Ref> m_spRef;
};

/// \brief C++ function that can be registered as closure
/// \details When a function bound to this type is called, the input params are
/// deserialized from the stack and put into the in parameter. When the function returns,
/// the returned Lua::Value array is serialized again and put on the stack.
/// Essentially you can bind any function with any extra parameters to this function type.
/// Internally the object is stored in a Lua userdata type with __call and __gc
/// metafunctions that call or clean up the function. As soon as the userdata object is
/// garbage collected, all bound variables (such as shared_from_this()) are destroyed.
typedef std::function<std::vector<Value>(State& state, const std::vector<Value>&)> T_fnCFunction;

/// \brief Lua function
/// \details Allows calling a Lua function; this object mostly results from a Lua::Value
/// object, e.g. when a value on a stack is a Lua function, and Lua::Value::FromStack() is
/// called. Also every C++ closure function call gets this object wrapped in a Lua::Value,
/// when the calling function passed a function as argument.
class Function
{
public:
   /// copy ctor
   Function(const Function& func);

   /// assignment operator
   Function& operator=(const Function& func);

   /// dtor
   ~Function() throw();

   /// calls function
   std::vector<Value> Call(int iResults = 0,
      const std::vector<Value>& vecParam = std::vector<Value>());

   /// returns ref object
   std::shared_ptr<Ref> GetRef() const { return m_spRef; }

private:
   friend Value;

   /// ctor; creates function object from value on stack
   explicit Function(std::shared_ptr<Ref> spRef);

   /// pushes function onto stack
   void Push();

   /// detaches function from stack
   void Detach() const;

private:
   /// state stack reference
   mutable std::shared_ptr<Ref> m_spRef;
};

/// \brief Lua table
/// \details The Table class can be either in a "constructing" mode, where you are
/// constructing a new table, or in a "using" mode, where you are using the table.
/// You get a table in the "constructing" mode by calling State::AddTable().
/// You get a table in the "using" mode by calling State::GetTable(). You can put
/// the table in "using" mode by calling Push(). This is also called when wrapping
/// a table object in a Lua::Value object.
class Table
{
public:
   /// copy ctor
   Table(const Table& table);

   /// assignment operator
   Table& operator=(const Table& table);

   /// dtor
   ~Table() throw();

   /// adds a value to the table, using a string as key
   Table& AddValue(const CString& key, const Value& value);

   /// adds a value to the table, using a numeric key
   Table& AddValue(int key, const Value& value);

   /// adds a function to the table
   Table& AddFunction(LPCSTR pszaName, T_fnCFunction fn);

   /// returns value from table
   Value GetValue(const CString& key);

   /// calls function stored in table
   std::vector<Value> CallFunction(const CString& cszName,
      int iResults = 0,
      const std::vector<Value>& vecParam = std::vector<Value>());

   /// returns ref object
   std::shared_ptr<Ref> GetRef() const { return m_spRef; }

private:
   friend State;
   friend Thread;
   friend Value;

   // note: these ctors can only be called by Lua::State

   /// ctor; constructs new table object on stack
   explicit Table(State& state, const CString& cszName);

   /// ctor; creates table object from value on stack
   explicit Table(std::shared_ptr<Ref> spRef, const CString& cszName);

   /// pushes table onto stack
   void Push();

   /// detaches table from stack
   void Detach() const;

private:
   /// table name
   CString m_cszName;

   /// state stack reference
   mutable std::shared_ptr<Ref> m_spRef;
};

/// \brief Lua userdata
/// \details represents a memory block that is managed by Lua. When creating
/// a userdata, the size of the memory block has to be specified.
class Userdata
{
public:
   /// copy ctor
   Userdata(const Userdata& userdata);

   /// assignment operator
   Userdata& operator=(const Userdata& userdata);

   /// dtor
   ~Userdata() throw();

   /// returns size of userdata memory block
   size_t Size() const throw() { return m_uiSize; }

   /// returns userdata memory block
   void* Data() const throw() { return m_pUserdata;  }

   /// returns userdata memory block, as typed pointer
   template <typename T>
   T* Data() const throw() { return reinterpret_cast<T*>(m_pUserdata); }

   /// returns ref object
   std::shared_ptr<Ref> GetRef() const { return m_spRef; }

private:
   friend State;
   friend Value;

   // note: these ctors can only be called by Lua::State

   /// ctor; constructs new userdata object on stack, with given size
   explicit Userdata(State& state, size_t uiSize);

   /// ctor; creates userdata object from value on stack
   explicit Userdata(std::shared_ptr<Ref> spRef);

   /// pushes userdata onto stack
   void Push();

   /// detaches userdata from stack
   void Detach() const;

private:
   /// raw memory block allocated by ctor
   void* m_pUserdata;

   /// size of raw memory block
   size_t m_uiSize;

   /// state stack reference
   mutable std::shared_ptr<Ref> m_spRef;
};

/// \brief Lua state
/// \details Helper functions to load Lua code, to call functions, to add tables and C++
/// closure functions and to access tables. Also some debug tracing functions for use
/// while coding.
class State
{
public:
   /// ctor
   State();

   /// loads a built-in library into state
   void RequireLib(const char* moduleName);

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

   /// adds an unnamed userdata, with a memory block of given size
   Userdata AddUserdata(size_t uiSize);

   /// adds a global function to the state
   void AddFunction(LPCTSTR pszaName, T_fnCFunction fn);

   /// adds a global value to the state
   void AddValue(LPCTSTR pszaName, Value value);

   /// returns an existing global value
   Value GetValue(const CString& cszName);

   /// returns an existing table
   Table GetTable(const CString& cszName);

   /// starts Lua garbage collector
   void CollectGarbage();

   /// debug-traces current stack
   static void TraceStack(lua_State* L);

   /// debug-traces current upvalues (e.g. for C++ closures)
   static void TraceUpvalues(lua_State* L);

   /// debug-traces a single value on stack
   static void TraceValue(lua_State* L, int iIndex, int iStackDepth, bool bIsUpvalue);

   /// returns Lua state
   /// \note use this with care, as you can mess up the stack or the state!
   lua_State* GetState() { return m_spState.get(); }

   /// returns the Lua version used
   static LPCTSTR GetVersion() throw();

private:
   friend Ref;
   friend Value;
   friend Table;
   friend Function;
   friend FuncData;
   friend Userdata;
   friend Thread;

   /// ctor that takes an existing state
   explicit State(lua_State* L, bool mainState);

   /// no-op deleter, used for non-mainState states
   static void StateDeleterNoop(lua_State*);

   /// panic error handler
   static int OnLuaPanic(lua_State* L);

   /// adds a reference to value on this state stack
   void AddRef(std::shared_ptr<Ref> spRef);

   /// removes reference from state stack
   void RemoveRef(Ref& ref);

   /// removes reference to value stored in this state stack
   bool RemoveFromStack(const Ref& val);

   /// removes stack value from state stack
   void RemoveIndex(int iStackIndex);

   /// detaches all references from state
   void DetachAll();

private:
   /// Lua state
   std::shared_ptr<lua_State> m_spState;

   /// list of references of values on state stack
   std::vector<std::shared_ptr<Ref>> m_vecRefsOnStack;
};

/// \brief Lua thread
/// \details manages a Lua thread, also called coroutine.
class Thread
{
public:
   /// thread status
   enum T_enThreadStatus
   {
      statusOK = 0,     ///< thread has returned or hasn't run yet
      statusYield = 1,  ///< thread has yielded
   };

   /// ctor; creates a new thread
   Thread(Lua::State& state);

   /// callback function for Yield() that is called when thread is being Resume()d
   typedef std::function<std::vector<Lua::Value>(Lua::State&, const std::vector<Lua::Value>&)> T_fnYieldCallback;

   // access to thread state values

   /// returns an existing global value
   Value GetValue(const CString& cszName);

   /// returns an existing table
   Table GetTable(const CString& cszName);

   // thread handling

   /// returns status of Lua thread; either LUA_OK or LUA_YIELD
   T_enThreadStatus Status() const throw();

   /// starts a function running in this thread, with given function and params,
   /// and returns new status and return values. Status must be LUA_OK (the thread
   /// must not have been yielded.
   std::pair<T_enThreadStatus, std::vector<Value>> Start(Function& func, const std::vector<Value>& vecParam);

   /// resumes thread when a function has yielded (status must be LUA_YIELD)
   /// and returns new status and return values.
   std::pair<T_enThreadStatus, std::vector<Value>> Resume(const std::vector<Value>& vecParam);

   /// yields this thread and passes the given values to the caller.
   __declspec(noreturn)
   void Yield(State& localParamState, const std::vector<Value>& vecRetValues, T_fnYieldCallback fnYieldCallback = T_fnYieldCallback());

   /// returns thread Lua state
   lua_State* GetThreadState() { return m_threadState.GetState();  }

   /// returns ref object
   std::shared_ptr<Ref> GetRef() const { return m_spRef; }

private:
   friend Value;

   /// ctor; creates thread object from value on stack
   explicit Thread(std::shared_ptr<Ref> spRef);

   /// pushes thread onto stack
   void Push();

   /// detaches thread from stack
   void Detach() const;

   /// resume implementation, used by Start() and Resume()
   std::pair<T_enThreadStatus, std::vector<Lua::Value>> InternalResume(const std::vector<Lua::Value>& vecParam);

   /// is called when Yield() is called
   static int InternalYield(lua_State* L, int status, ptrdiff_t context);

private:
   /// parent state
   Lua::State& m_state;

   /// thread state
   Lua::State m_threadState;

   /// state stack reference
   mutable std::shared_ptr<Ref> m_spRef;
};

}; // namespace Lua
