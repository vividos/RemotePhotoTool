//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file CdsdkCommon.hpp CDSDK - Common functions
//
#pragma once

// includes
#pragma warning(push)
#include "cdAPI.h"
#pragma warning(pop)
#include "SourceInfo.hpp"
#include "CameraException.hpp"
#include "SdkReferenceBase.hpp"
#include <vector>

/// Canon Digital Camera SDK interface
namespace CDSDK
{

/// checks for CDSDK error and throws exception
void CheckError(const CString& cszFunction, cdError err, LPCSTR pszFile, UINT uiLine);

/// SDK reference
class Ref: public SdkReferenceBase, public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref();

   /// adds version text
   virtual void AddVersionText(CString& cszVersionText) const override;

   /// enumerates devices
   virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const override;

   /// returns if AsyncWaitForCamera() is possible for this SDK
   virtual bool IsAsyncWaitPossible() const override { return false; }
};

/// shared ptr to SDK reference
typedef std::shared_ptr<Ref> RefSp;

} // namespace CDSDK
