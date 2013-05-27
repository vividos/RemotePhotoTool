//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file PsrecCommon.hpp PS-ReC common header
//
#pragma once

// includes
#include "prAPI.h"
#include "prType.h"
#include "SourceInfo.hpp"
#include "CameraException.hpp"

/// Powershot Remote Capture SDK (PS-ReC) interface
namespace PSREC
{

/// checks for error and throws CameraException when necessary
inline void CheckError(const CString& cszFunction, prResponse err, LPCSTR pszFile, UINT uiLine)
{
   if (err != prOK)
      throw CameraException(cszFunction,
         false,
         err & prERROR_COMPONENTID_MASK,
         err & prERROR_ERRORID_MASK,
         pszFile, uiLine);
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
