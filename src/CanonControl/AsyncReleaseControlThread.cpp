//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AsyncReleaseControlThread.cpp Canon control - Thread for releasing shutter asynchronously
//

// includes
#include "stdafx.h"
#include "AsyncReleaseControlThread.hpp"
#include "Thread.hpp"

void AsyncReleaseControlThread::Run()
{
   Thread::SetName(_T("AsyncReleaseControlThread"));

   // run message queue
   MSG msg = { 0 };
   while (!m_bFinished)
   {
      // process window messages
      if (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
      {
         BOOL bRet = ::GetMessage(&msg, nullptr, 0, 0);
         if (!bRet)
            break;   // WM_QUIT, exit message loop

         ATLTRACE(_T("HWND=%08x MSG=%04x\n"), msg.hwnd, msg.message);

         ::TranslateMessage(&msg);
         ::DispatchMessage(&msg);
      }
      else
      {
         // process asio handlers, if any
         boost::system::error_code ec;

         m_ioService.poll_one(ec);
      }

      // skip current time slice
      Sleep(1);
   }
}
