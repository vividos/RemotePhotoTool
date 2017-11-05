//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file AsyncReleaseControlThread.hpp Canon control - Thread for releasing shutter asynchronously
//
#pragma once

// includes
#include <atomic>
#include <thread>
#include <ulib/config/BoostAsio.hpp>

/// thread to async execute Release() and other functions
class AsyncReleaseControlThread
{
public:
   /// ctor
   AsyncReleaseControlThread()
      :m_ioService(1),
      m_upDefaultWork(new boost::asio::io_service::work(m_ioService)),
      m_bFinished(false)
   {
      m_upThread.reset(new std::thread(std::bind(&AsyncReleaseControlThread::Run, this)));
   }

   /// dtor
   ~AsyncReleaseControlThread()
   {
      try
      {
         m_bFinished = true;
         m_upDefaultWork.reset();
         m_ioService.stop();

         m_upThread->join();
      }
      catch (...)
      {
      }
   }

   /// returns io service
   boost::asio::io_service& GetIoService() { return m_ioService; }

   /// posts function to execute in thread
   void Post(std::function<void()> fn)
   {
      m_ioService.post(fn);
   }

private:
   /// thread function
   void Run();

private:
   /// background thread
   std::unique_ptr<std::thread> m_upThread;

   /// io service
   boost::asio::io_service m_ioService;

   /// default work
   std::unique_ptr<boost::asio::io_service::work> m_upDefaultWork;

   /// indicates that thread should stop
   std::atomic<bool> m_bFinished;
};
