//
// ulib - a collection of useful classes
// Copyright (C) 2014 Michael Fink
//
/// \file Process.hpp Win32 process
//
#pragma once

/// Win32 process
class Process
{
public:
   /// ctor
   Process()
   {
      ZeroMemory(&m_startupInfo, sizeof(m_startupInfo));
      m_startupInfo.cb = sizeof(m_startupInfo);

      ZeroMemory(&m_processInfo, sizeof(m_processInfo));
   }
   /// dtor
   ~Process()
   {
      if (m_processInfo.hThread != nullptr)
         CloseHandle(m_processInfo.hThread);

      if (m_processInfo.hProcess != nullptr)
         CloseHandle(m_processInfo.hProcess);
   }

   /// sets working directory for process
   void WorkingDirectory(const CString& cszWorkingDirectory)
   {
      m_cszWorkingDirectory = cszWorkingDirectory;
      m_cszWorkingDirectory.TrimRight(_T('\\'));
   }

   /// creates process with given command line
   bool Create(const CString& cszCommandLine)
   {
      BOOL bRet = ::CreateProcess(NULL,
         const_cast<LPTSTR>(static_cast<LPCTSTR>(cszCommandLine)),
         NULL, NULL, FALSE, 0, NULL,
         m_cszWorkingDirectory.IsEmpty() ? nullptr : m_cszWorkingDirectory.GetString(),
         &m_startupInfo,
         &m_processInfo);

      return bRet != FALSE;
   }

   /// returns process handle
   HANDLE ProcessHandle() const throw() { return m_processInfo.hProcess; }

private:
   /// startup info
   STARTUPINFO m_startupInfo;

   /// process info
   PROCESS_INFORMATION m_processInfo;

   /// working directory to start in
   CString m_cszWorkingDirectory;
};
