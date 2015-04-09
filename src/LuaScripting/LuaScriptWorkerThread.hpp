//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file LuaScriptWorkerThread.hpp Lua script worker thread
//
#pragma once

// includes
#include "Asio.hpp"
#include <thread>
#include "Thread.hpp"

/// Lua script worker thread
class LuaScriptWorkerThread
{
public:
   /// ctor
   LuaScriptWorkerThread()
      :m_ioService(1),
      m_strand(m_ioService),
      m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
      m_thread(std::bind(&LuaScriptWorkerThread::Run, this))
   {
   }

   /// dtor
   ~LuaScriptWorkerThread() throw()
   {
      try
      {
         m_upDefaultWork.reset();
         m_ioService.stop();
         m_thread.join();
      }
      catch (...)
      {
      }
   }

   /// returns strand
   boost::asio::io_service::strand& GetStrand() { return m_strand; }

private:
   /// thread function
   void Run()
   {
      Thread::SetName(_T("LuaScriptWorkerThread"));
      m_ioService.run();
   }

private:
   /// io service
   boost::asio::io_service m_ioService;

   /// single strand to be used for Lua calls
   boost::asio::io_service::strand m_strand;

   /// default work
   std::unique_ptr<boost::asio::io_service::work> m_upDefaultWork;

   /// background thread
   std::thread m_thread;
};
