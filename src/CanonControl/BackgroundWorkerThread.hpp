//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file BackgroundWorkerThread.hpp Canon control - Background worker thread
//

// includes
#include <thread>
#include "Asio.hpp"
#include "Thread.hpp"

/// background worker thread
class BackgroundWorkerThread
{
public:
#pragma warning(disable: 4355) // 'this' : used in base member initializer list
   /// ctor
   BackgroundWorkerThread()
      :m_ioService(1),
       m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
       m_thread(std::bind(&BackgroundWorkerThread::Run, this))
   {
   }
#pragma warning(default: 4355)
   /// dtor
   ~BackgroundWorkerThread() throw()
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

   /// returns asio io service
   boost::asio::io_service& GetIoService() { return m_ioService; }

private:
   /// thread function
   void Run()
   {
      Thread::SetName(_T("BackgroundWorkerThread"));
      m_ioService.run();
   }

private:
   /// io service
   boost::asio::io_service m_ioService;

   /// default work
   std::unique_ptr<boost::asio::io_service::work> m_upDefaultWork;

   /// background thread
   std::thread m_thread;
};
