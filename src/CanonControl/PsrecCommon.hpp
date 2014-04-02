//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecCommon.hpp PS-ReC common header
//
#pragma once

// includes
#pragma warning(push)
#include "prAPI.h"
#pragma warning(pop)
#include "SourceInfo.hpp"
#include "CameraException.hpp"
#include "ErrorText.hpp"

/// Powershot Remote Capture SDK (PS-ReC) interface
namespace PSREC
{

/// checks for error and throws CameraException when necessary
inline void CheckError(const CString& cszFunction, prResponse err, LPCSTR pszFile, UINT uiLine)
{
   if (err == prOK)
      return;

   prResponse componentId = err & prERROR_COMPONENTID_MASK;
   prResponse errorId = err & prERROR_ERRORID_MASK;

   CString cszMessage;
   cszMessage.Format(_T("Error in function \"%s\": %s, %s (%08x)"),
      cszFunction.GetString(),
      componentId == prERROR_PTP_COMPONENTID ? _T("PTP") :
      componentId == prERROR_PRSDK_COMPONENTID ? _T("PRSDK") :
      componentId == prERROR_WIA_STI_COMPONENTID ? _T("WIA/STI") :
      componentId == prERROR_WINDOWS_COMPONENTID ? _T("Windows") :
      componentId == prERROR_COMIF_COMPONENTID ? _T("COM") : _T("???"),
      ErrorTextFromErrorId(errorId, true),
      err);

   throw CameraException(cszFunction, cszMessage, err, pszFile, uiLine);
}

/// SDK reference
class Ref: public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref() throw();

   /// adds verwion text
   void AddVersionText(CString& cszVersionText) const;

   /// enumerates devices
   void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const;

private:
   class Impl;

   /// implementation
   std::unique_ptr<Impl> m_upImpl;
};

/// shared pointer to SDK reference
typedef std::shared_ptr<Ref> RefSp;

} // namespace PSREC
