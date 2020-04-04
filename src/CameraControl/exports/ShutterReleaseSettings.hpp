//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ShutterReleaseSettings.hpp Canon control - Shutter release settings
//
#pragma once

// includes
#include <vector>

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
   typedef std::function<void(const ShutterReleaseSettings&)> T_fnOnFinishedTransfer;

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
   void SaveTarget(T_enSaveTarget enSaveTarget) { m_enSaveTarget = enSaveTarget; }

   /// sets handler function called when image transfer has finished
   void HandlerOnFinishedTransfer(T_fnOnFinishedTransfer fnOnFinishedTransfer)
   {
      m_fnOnFinishedTransfer = fnOnFinishedTransfer;
   }

   // get functions

   /// returns filename of saved file
   CString Filename() const { return m_cszFilename; }

   /// returns save target
   T_enSaveTarget SaveTarget() const { return m_enSaveTarget; }

   /// returns handler function called when image transfer has finished
   T_fnOnFinishedTransfer HandlerOnFinishedTransfer() const { return m_fnOnFinishedTransfer; }

private:
   /// save target
   T_enSaveTarget m_enSaveTarget;

   /// handler that is called when image transfer has finished
   T_fnOnFinishedTransfer m_fnOnFinishedTransfer;

   /// filename
   CString m_cszFilename;
};
