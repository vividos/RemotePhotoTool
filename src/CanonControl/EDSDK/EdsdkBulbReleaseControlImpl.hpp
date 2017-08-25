//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file EdsdkBulbReleaseControlImpl.hpp EDSDK - BulbReleaseControl implementation
//
#pragma once

// includes
#include "BulbReleaseControl.hpp"
#include <ulib/Timer.hpp>

namespace EDSDK
{

/// bulb release control impl for EDSDK
class BulbReleaseControlImpl: public BulbReleaseControl
{
public:
   /// ctor
   BulbReleaseControlImpl(const Handle& hCamera)
      :m_hCamera(hCamera),
       m_bStopped(false)
   {
      // lock ui
      EdsError err = EdsSendStatusCommand(m_hCamera.Get(), kEdsCameraStatusCommand_UILock, 0);
      LOG_TRACE(_T("EdsSendStatusCommand(ref = %08x, UILock, 0) returned %08x\n"), m_hCamera.Get(), err);
      EDSDK::CheckError(_T("EdsSendStatusCommand"), err, __FILE__, __LINE__);

      // start bulb
      err = ::EdsSendCommand(m_hCamera.Get(), kEdsCameraCommand_BulbStart, 0);
      LOG_TRACE(_T("EdsSendCommand(ref = %08x, BulbStart, 0) returned %08x\n"), m_hCamera.Get(), err);
      EDSDK::CheckError(_T("EdsSendCommand"), err, __FILE__, __LINE__);

      m_bulbTimer.Start();
   }

   /// dtor
   virtual ~BulbReleaseControlImpl() throw()
   {
      Stop();
   }

   virtual double ElapsedTime() const throw() override
   {
      return m_bulbTimer.TotalElapsed();
   }

   virtual void Stop() throw() override
   {
      // end bulb
      EdsError err = ::EdsSendCommand(m_hCamera.Get(), kEdsCameraCommand_BulbEnd, 0);
      LOG_TRACE(_T("EdsSendCommand(ref = %08x, BulbEnd, 0) returned %08x\n"), m_hCamera.Get(), err);

      // end ui lock
      err = EdsSendStatusCommand(m_hCamera.Get(), kEdsCameraStatusCommand_UIUnLock, 0);
      LOG_TRACE(_T("EdsSendStatusCommand(ref = %08x, UIUnLock, 0) returned %08x\n"), m_hCamera.Get(), err);

      m_bStopped = true;

      m_bulbTimer.Stop();
   }

private:
   /// camera handle
   Handle m_hCamera;

   /// bulb timer
   Timer m_bulbTimer;

   /// indicates if bulb was already stopped
   bool m_bStopped;
};

} // namespace EDSDK
