//
// ulib - a collection of useful classes
// Copyright (C) 2008,2009,2012,2013 Michael Fink
//
/// \file ProgramOptions.hpp program options handling
//
#pragma once

// needed includes
#include <vector>
#include <boost\function.hpp>

// forward references
class CommandLineParser;

/// \brief program options class
/// \details supports parsing program options given at command line
class ProgramOptions
{
public:
   /// option handler function; called when option is parsed
   typedef boost::function<bool (const std::vector<CString>&)> T_fnOptionHandler;

   /// option handler function for single argument
   typedef boost::function<bool (const CString&)> T_fnOptionHandlerSingleArg;

   /// parameter handler
   typedef boost::function<bool (const CString&)> T_fnParameterHandler;

   /// handler for text output
   typedef boost::function<void (const CString&)> T_fnOptionOutputHandler;

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
   static void OutputConsole(const CString& cszText);

private:
   void Parse(CommandLineParser& parser);

   static bool CallSingleArgHandler(const std::vector<CString>& vecArgs, T_fnOptionHandlerSingleArg fnHandler)
   {
      ATLASSERT(vecArgs.size() == 1);
      return fnHandler(vecArgs[0]);
   }

   static bool SetStringArgStorage(const CString& cszArg, CString& cszStorage)
   {
      cszStorage = cszArg;
      return true;
   }

   static bool SetBoolArgStorage(bool& bStorage)
   {
      bStorage = true;
      return true;
   }

private:
   struct OptionInfo
   {
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

      CString m_cszShortOptionChars;
      CString m_cszLongOption;
      CString m_cszHelpText;
      unsigned int m_uiArgs;

      T_fnOptionHandler m_fnOptionHandler;
   };

   CString m_cszExecutable;
   std::vector<OptionInfo> m_vecOptions;
   T_fnParameterHandler m_fnParameterHandler;
   T_fnOptionOutputHandler m_fnOptionOutputHandler;

   bool m_bHandledHelp;
};
