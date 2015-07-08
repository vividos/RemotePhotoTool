//
// ulib - a collection of useful classes
// Copyright (C) 2008,2009,2013 Michael Fink
//
/// \file ProgramOptions.cpp program options implementation
//

// includes
#include "stdafx.h"
#include "ProgramOptions.hpp"
#include "CommandLineParser.hpp"

void ProgramOptions::RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, unsigned int uiArgs,
   T_fnOptionHandler fnOptionHandler)
{
   CString cszLongOptionLower(cszLongOption);
   cszLongOptionLower.MakeLower();

   OptionInfo info(cszShortOptionChars, cszLongOptionLower, cszHelpText, uiArgs, fnOptionHandler);
   m_vecOptions.push_back(info);
}

void ProgramOptions::RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText,
   T_fnOptionHandlerSingleArg fnOptionHandler)
{
   T_fnOptionHandler fnOptionHandler2 =
      std::bind(&ProgramOptions::CallSingleArgHandler, std::placeholders::_1, fnOptionHandler);

   RegisterOption(cszShortOptionChars, cszLongOption, cszHelpText, 1, fnOptionHandler2);
}

void ProgramOptions::RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, CString& cszArgStorage)
{
   T_fnOptionHandlerSingleArg fnOptionHandler =
      std::bind(&ProgramOptions::SetStringArgStorage, std::placeholders::_1, std::ref(cszArgStorage));

   RegisterOption(cszShortOptionChars, cszLongOption, cszHelpText, fnOptionHandler);
}

void ProgramOptions::RegisterOption(const CString& cszShortOptionChars, const CString& cszLongOption, const CString& cszHelpText, bool& bOptionFlag)
{
   T_fnOptionHandlerSingleArg fnOptionHandler =
      std::bind(&ProgramOptions::SetBoolArgStorage, std::ref(bOptionFlag));

   RegisterOption(cszShortOptionChars, cszLongOption, cszHelpText, fnOptionHandler);
}

void ProgramOptions::RegisterHelpOption()
{
   T_fnOptionHandler fnOptionHandler = std::bind(&ProgramOptions::OutputHelp, this);

   RegisterOption(_T("h?"), _T("help"), _T("Shows help"), 0, fnOptionHandler);
}

bool ProgramOptions::OutputHelp()
{
   if (!m_fnOptionOutputHandler)
      return true;

   CString cszAppName = m_cszExecutable;
   int iPos = cszAppName.ReverseFind(_T('\\'));
   if (iPos != -1)
      cszAppName = cszAppName.Mid(iPos+1);

   CString cszHelpText;
   cszHelpText.Format(
      _T("Syntax: %s <params> <args>\n")
      _T("Options:\n"),
      cszAppName.GetString());

   // append all options
   CString cszTemp;
   for (size_t i=0, iMax=m_vecOptions.size(); i<iMax; i++)
   {
      OptionInfo& optInfo = m_vecOptions[i];

      // add short option chars
      for (int j=0, jMax = optInfo.m_cszShortOptionChars.GetLength(); j<jMax; j++)
      {
         cszTemp.Format(_T(" -%c"), optInfo.m_cszShortOptionChars[j]);
         cszHelpText += cszTemp;
      }

      // add long option string
      cszTemp.Format(_T(" --%s"), optInfo.m_cszLongOption.GetString());
      cszHelpText += cszTemp;
      cszHelpText += _T("\n   ");

      // help text
      cszTemp = optInfo.m_cszHelpText;
      cszTemp.Replace(_T("\n"), _T("\n   ")); // add proper indentation
      cszHelpText += cszTemp + _T("\n");
   }

   m_fnOptionOutputHandler(cszHelpText);

   m_bHandledHelp = true;

   return true;
}

void ProgramOptions::Parse(int argc, _TCHAR* argv[])
{
   CommandLineParser parser(argc, argv);
   Parse(parser);
}

void ProgramOptions::Parse(LPCTSTR pszCommandLine)
{
   CommandLineParser parser(pszCommandLine);
   Parse(parser);
}

void ProgramOptions::Parse(CommandLineParser& parser)
{
   // must contain at least one param
   bool bRet1 = parser.GetNext(m_cszExecutable);
   ATLASSERT(true == bRet1); bRet1;

   CString cszArg;
   while (parser.GetNext(cszArg))
   {
      ATLASSERT(cszArg.GetLength() > 0);

      // check for first char
      TCHAR chArg = cszArg[0];
      if (cszArg.GetLength() > 1 && (chArg == _T('/') || chArg == _T('-') ))
      {
         TCHAR chSearchOpt = 0;
         CString cszLongSearchOpt;

         // it's an option, either short or long one
         TCHAR chOptChar = cszArg[1];
         if (chArg == _T('-') && chOptChar == _T('-'))
         {
            // we have a unix-style option, long name only
            cszLongSearchOpt = cszArg.Mid(2);
            cszLongSearchOpt.MakeLower();
         }
         else
         {
            // we have either /X or -X or option, short name only
            chSearchOpt = chOptChar;
         }

         // now search for the proper option
         bool bFoundOption = false;
         for (size_t i=0, iMax=m_vecOptions.size(); i<iMax; i++)
         {
            OptionInfo& optInfo = m_vecOptions[i];

            // check long name first, then short name
            if ((!cszLongSearchOpt.IsEmpty() && cszLongSearchOpt == optInfo.m_cszLongOption) ||
                (chSearchOpt != 0 && CString(chSearchOpt).FindOneOf(optInfo.m_cszShortOptionChars) != -1))
            {
               // found long or short option
               bFoundOption = true;

               // get arguments
               std::vector<CString> vecArgs;
               CString cszParamArgs;
               for (unsigned int uiArgs=0; uiArgs<optInfo.m_uiArgs; uiArgs++)
               {
                  parser.GetNext(cszParamArgs);
                  vecArgs.push_back(cszParamArgs);
               }

               if (vecArgs.size() < optInfo.m_uiArgs)
               {
                  // too few arguments
                  if (m_fnOptionOutputHandler)
                     m_fnOptionOutputHandler(CString(_T("Too few parameters for option: ") + cszArg));
                  break;
               }

               ATLASSERT(optInfo.m_fnOptionHandler != NULL);
               bool bRet2 = optInfo.m_fnOptionHandler(vecArgs);

               if (!bRet2)
               {
                  if (m_fnOptionOutputHandler)
                     m_fnOptionOutputHandler(CString(_T("Syntax error for option: ") + cszArg));
               }

               break;
            }
         } // end for

         if (!bFoundOption)
         {
            // unknown option
            if (m_fnOptionOutputHandler)
               m_fnOptionOutputHandler(CString(_T("Unknown option: ") + cszArg));
         }
      }
      else
      {
         // no arg; it's a file
         bool bHandled = false;
         if (m_fnParameterHandler)
            bHandled = m_fnParameterHandler(cszArg);

         if (!bHandled)
         {
            // output: unhandled option
            if (m_fnOptionOutputHandler)
               m_fnOptionOutputHandler(CString(_T("Unknown parameter: ") + cszArg));
         }
      }
   }
}

void ProgramOptions::OutputConsole(const CString& cszText)
{
   _tprintf(_T("%s\n"), cszText.GetString());
}
