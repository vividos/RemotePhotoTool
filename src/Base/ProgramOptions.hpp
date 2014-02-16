//
// ulib - a collection of useful classes
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ProgramOptions.hpp program options handling
//
#pragma once

// needed includes
#include <vector>
#include <functional>

// forward references
class CommandLineParser;

/// \brief program options class
/// \details supports parsing program options given at command line
class ProgramOptions
{
public:
   /// option handler function; called when option is parsed
   typedef std::function<bool (const std::vector<CString>&)> T_fnOptionHandler;

   /// option handler function for single argument
   typedef std::function<bool (const CString&)> T_fnOptionHandlerSingleArg;

   /// parameter handler
   typedef std::function<bool (const CString&)> T_fnParameterHandler;

   /// handler for text output
   typedef std::function<void (const CString&)> T_fnOptionOutputHandler;

   /// ctor
   ProgramOptions() throw()
      :m_bHandledHelp(false)
   {
   }

   /// returns if help option was selected, e.g. when program should exit
   bool IsSelectedHelpOption() const throw() { return m_bHandledHelp; }

   /// registers a handler for arguments without preceding options; most of the time file names
   void RegisterParameterHandler(T_fnParameterHandler fnParameterHandler) throw()
   {
      m_fnParameterHandler = fnParameterHandler;
   }

   /// registers an output handler; needed when options parser has to output something
   void RegisterOutputHandler(T_fnOptionOutputHandler fnOptionOutputHandler) throw()
   {
      m_fnOptionOutputHandler = fnOptionOutputHandler;
   }

   /// registers an option and sets option handler callback function
   void RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, unsigned int uiArgs,
      T_fnOptionHandler fnOptionHandler);

   /// registers option with handler that takes single argument
   void RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, T_fnOptionHandlerSingleArg fnOptionHandler);

   /// registers option that sets string variable; var must be available until Parse() returns
   void RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, CString& cszArgStorage);

   /// registers option that sets boolean variable to true; var must be available until Parse() returns
   void RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, bool& bOptionFlag);

   /// parses command line options, C-style
   void Parse(int argc, _TCHAR* argv[]);

   /// parses command line options, Win32-style
   void Parse(LPCTSTR pszCommandLine);

   /// registers general help option /h, /?, --help
   void RegisterHelpOption();

   /// output help (manually)
   bool OutputHelp();

   /// helper function for outputting to console; can directly be used in call to RegisterOutputHandler()
   static void OutputConsole(const CString& cszText) throw();

private:
   /// parses command line
   void Parse(CommandLineParser& parser);

   /// calls single argument handler
   static bool CallSingleArgHandler(const std::vector<CString>& vecArgs, T_fnOptionHandlerSingleArg fnHandler)
   {
      ATLASSERT(vecArgs.size() == 1);
      return fnHandler(vecArgs[0]);
   }

   /// sets string argument to storage variable
   static bool SetStringArgStorage(const CString& cszArg, CString& cszStorage)
   {
      cszStorage = cszArg;
      return true;
   }

   /// sets bool argument to storage variable
   static bool SetBoolArgStorage(bool& bStorage)
   {
      bStorage = true;
      return true;
   }

private:
   /// info about a single option
   struct OptionInfo
   {
      /// ctor
      OptionInfo(const CString& cszShortOptionChars,
         const CString& cszLongOption,
         const CString& cszHelpText,
         unsigned int uiArgs,
         T_fnOptionHandler fnOptionHandler)
         :m_cszShortOptionChars(cszShortOptionChars),
          m_cszLongOption(cszLongOption),
          m_cszHelpText(cszHelpText),
          m_uiArgs(uiArgs),
         m_fnOptionHandler(fnOptionHandler)
      {
      }

      CString m_cszShortOptionChars;   ///< collection of short option characters for this option
      CString m_cszLongOption;         ///< text of long option
      CString m_cszHelpText;           ///< help text
      unsigned int m_uiArgs;           ///< number of args for this option

      /// option handler to call
      T_fnOptionHandler m_fnOptionHandler;
   };

   /// holds the program executable (first argument of command line)
   CString m_cszExecutable;

   /// contains all options
   std::vector<OptionInfo> m_vecOptions;

   /// function pointer to parameter handler
   T_fnParameterHandler m_fnParameterHandler;

   /// function pointer to option output handler
   T_fnOptionOutputHandler m_fnOptionOutputHandler;

   /// indicates if help option was handled
   bool m_bHandledHelp;
};
