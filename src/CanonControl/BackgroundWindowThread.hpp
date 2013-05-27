//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file BackgroundWindowThread.hpp Canon control - Background window thread
//
#pragma once

// includes
#include "Event.hpp"
#include <thread>
#include <boost/bind.hpp>

/// this is the background window we're starting
template <typename TInstance>
class BackgroundWindowThread
{
public:
   /// ctor
   BackgroundWindowThread()
      :m_evtStarted(false, false), // auto-reset event
       m_evtStop(true, false) // manual-reset event
   {
   }
   /// dtor
   ~BackgroundWindowThread()
   {
      if (m_upThread != nullptr)
      {
         m_evtStop.Set();
         m_upThread->join();
      }
   }

   /// starts background thread
   void Start()
   {
      m_upThread.reset(new std::thread(boost::bind(&BackgroundWindowThread::Run, this)));
      m_evtStarted.Wait();
   }

private:
   /// thread method
   int Run()
   {
      // note: we don't even have to create a window, it's enough to run the message loop

      // start instance
      m_upInst.reset(new TInstance);

      m_evtStarted.Set();

      // run message queue
      for (bool bLoop = true; bLoop;)
      {
         HANDLE h = m_evtStop.Handle();
         DWORD dwRet = ::MsgWaitForMultipleObjects(1, &h, FALSE, INFINITE, QS_ALLINPUT);

         if (WAIT_OBJECT_0 == dwRet)
            bLoop = false;

         LOG_TRACE(_T("NEW MESSAGES\n"));

         // process window messages
         MSG msg = {0};
         for (;;)
         {
            if (::PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE))
            {
               BOOL bRet = ::GetMessage(&msg, nullptr, 0, 0);
               if(!bRet)
                  break;   // WM_QUIT, exit message loop

               LOG_TRACE(_T("HWND=%08x MSG=%04x\n"), msg.hwnd, msg.message);

               ::TranslateMessage(&msg);
               ::DispatchMessage(&msg);
            }
            else
               break;
         }

         LOG_TRACE(_T("MESSAGES DONE\n"));
      }

      // end instance
      m_upInst.reset();

      return 0;
   }

private:
   Event m_evtStarted;  ///< indicates if thread has started
   Event m_evtStop;     ///< indicates that thread should stop
   std::unique_ptr<std::thread> m_upThread;  ///< background thread
   std::unique_ptr<TInstance> m_upInst;      ///< instance
};
