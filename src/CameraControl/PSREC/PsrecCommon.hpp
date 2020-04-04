//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
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
#include "SdkReferenceBase.hpp"

/// Powershot Remote Capture SDK (PS-ReC) interface
namespace PSREC
{

/// checks for error and throws CameraException when necessary
void CheckError(const CString& cszFunction, prResponse err, LPCSTR pszFile, UINT uiLine);

/// SDK reference
class Ref: public SdkReferenceBase, public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref();

   /// adds verwion text
   virtual void AddVersionText(CString& versionText) const override;

   /// enumerates devices
   virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceDevicesList) const override;

   /// returns if AsyncWaitForCamera() is possible for this SDK
   virtual bool IsAsyncWaitPossible() const override { return false; }
};

/// shared pointer to SDK reference
typedef std::shared_ptr<Ref> RefSp;

} // namespace PSREC
