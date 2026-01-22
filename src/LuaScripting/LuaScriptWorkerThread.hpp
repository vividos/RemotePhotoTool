//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2026 Michael Fink
//
/// \file LuaScriptWorkerThread.hpp Lua script worker thread
//
#pragma once

// includes
#include <asio.hpp>
#include <thread>
#include <ulib/thread/Thread.hpp>
#include "CameraException.hpp"
#include "Logging.hpp"
#include "Lua.hpp"

/// Lua script worker thread
class LuaScriptWorkerThread
{
public:
   /// ctor
   LuaScriptWorkerThread()
      :m_ioService(1),
      m_strand(m_ioService),
      m_upDefaultWork(new asio::io_service::work(m_ioService)),
      m_thread(std::bind(&LuaScriptWorkerThread::Run, this))
   {
   }

   /// dtor
   ~LuaScriptWorkerThread()
   {
      Stop();
   }

   /// function type to output debug strings
   typedef std::function<void(const CString&)> T_fnOutputDebugString;

   /// sets output debug string handler
   void SetOutputDebugStringHandler(T_fnOutputDebugString fnOutputDebugString)
   {
      m_fnOutputDebugString = fnOutputDebugString;
   }

   /// stops worker thread
   void Stop()
   {
      try
      {
         m_upDefaultWork.reset();

         if (m_thread.joinable())
            m_thread.join();
      }
      catch (...)
      {
      }
   }

   /// returns strand
   asio::io_service::strand& GetStrand() { return m_strand; }

private:
   /// thread function
   void Run()
   {
      Thread::SetName(_T("LuaScriptWorkerThread"));

      for (;;)
      {
         try
         {
            m_ioService.run();
            return;
         }
         catch (const CameraException& ex)
         {
            CString cszText;
            cszText.Format(_T("%s(%u): CameraException occured in Lua script execution: %s\n"),
               ex.SourceFile().GetString(),
               ex.SourceLine(),
               ex.Message().GetString());

            LOG_TRACE(cszText);

            if (m_fnOutputDebugString != nullptr)
               m_fnOutputDebugString(cszText);
         }
         catch (const Lua::Exception& ex)
         {
            CString cszText;
            cszText.Format(_T("%s(%u): Lua exception occured: %s\n"),
               ex.LuaSourceFile().GetString(),
               ex.LuaLineNumber(),
               ex.LuaErrorMessage().GetString());

            LOG_TRACE(cszText);

            if (m_fnOutputDebugString != nullptr)
               m_fnOutputDebugString(cszText);
         }
         catch (const Exception& ex)
         {
            CString cszText;
            cszText.Format(_T("%s(%u): Exception occured in Lua script execution: %s\n"),
               ex.SourceFile().GetString(),
               ex.SourceLine(),
               ex.Message().GetString());

            LOG_TRACE(cszText);

            if (m_fnOutputDebugString != nullptr)
               m_fnOutputDebugString(cszText);
         }
         catch (const std::exception& ex)
         {
            CString cszText;
            cszText.Format(_T("std::exception occured in Lua script execution: %hs\n"),
               ex.what());

            LOG_TRACE(cszText);

            if (m_fnOutputDebugString != nullptr)
               m_fnOutputDebugString(cszText);
         }
         catch (...)
         {
            CString cszText(_T("unknown exception occured in Lua script execution.\n"));

            LOG_TRACE(cszText);

            if (m_fnOutputDebugString != nullptr)
               m_fnOutputDebugString(cszText);
         }
      }
   }

private:
   /// io service
   asio::io_service m_ioService;

   /// single strand to be used for Lua calls
   asio::io_service::strand m_strand;

   /// default work
   std::unique_ptr<asio::io_service::work> m_upDefaultWork;

   /// background thread
   std::thread m_thread;

   /// output debug string handler
   T_fnOutputDebugString m_fnOutputDebugString;
};
