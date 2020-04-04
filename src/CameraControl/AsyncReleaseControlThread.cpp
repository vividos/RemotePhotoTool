//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AsyncReleaseControlThread.cpp Canon control - Thread for releasing shutter asynchronously
//

// includes
#include "stdafx.h"
#include "AsyncReleaseControlThread.hpp"
#include <ulib/thread/Thread.hpp>

void AsyncReleaseControlThread::Run()
{
   Thread::SetName(_T("AsyncReleaseControlThread"));

   // run message queue
   while (!m_bFinished)
   {
      // process asio handlers, if any
      boost::system::error_code ec;

      m_ioService.poll_one(ec);

      // skip current time slice
      Sleep(1);
   }
}
