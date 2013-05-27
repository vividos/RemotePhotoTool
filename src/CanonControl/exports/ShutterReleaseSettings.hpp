//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file ShutterReleaseSettings.hpp Canon control - Shutter release settings
//
#pragma once

// includes
#include <vector>
#include <boost/function.hpp>

/// shutter release settings
class ShutterReleaseSettings
{
public:
   /// possible save targets
   /// \note transfer handlers are not called when only saving to camera
   enum T_enSaveTarget
   {
      saveToCamera = 1, ///< on shutter release, image is saved to camera storage only
      saveToHost = 2,   ///< image is transferred to host only
      saveToBoth = saveToCamera | saveToHost, ///< image is saved to camera storage and transferred to host
   };

   /// handler function that is called at end of transfer
   typedef boost::function<void(const ShutterReleaseSettings&)> T_fnOnFinishedTransfer;

   /// ctor; creates release settings
   ShutterReleaseSettings(T_enSaveTarget enSaveTarget = saveToCamera,
      T_fnOnFinishedTransfer fnOnFinishedTransfer = T_fnOnFinishedTransfer())
      :m_enSaveTarget(enSaveTarget),
       m_fnOnFinishedTransfer(fnOnFinishedTransfer)
   {
   }

   // set functions

   /// sets output filename
   void Filename(const CString& cszFilename) { m_cszFilename = cszFilename; }

   /// sets save target
   void SaveTarget(T_enSaveTarget enSaveTarget) throw() { m_enSaveTarget = enSaveTarget; }

   // get functions

   /// returns filename of saved file
   CString Filename() const throw() { return m_cszFilename; }

   /// returns save target
   T_enSaveTarget SaveTarget() const throw() { return m_enSaveTarget; }

   /// returns handler function when 
   T_fnOnFinishedTransfer HandlerOnFinishedTransfer() const throw() { return m_fnOnFinishedTransfer; }

private:
   /// save target
   T_enSaveTarget m_enSaveTarget;

   /// handler that is called when image transfer has finished
   T_fnOnFinishedTransfer m_fnOnFinishedTransfer;

   /// filename
   CString m_cszFilename;
};
