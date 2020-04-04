//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file GPhoto2Common.hpp gPhoto2 - Common functions
//
#pragma once

// includes
#include "Instance.hpp"
#include "CameraException.hpp"
#include "SdkReferenceBase.hpp"

// forward references
struct _GPContext;
struct _Camera;

/// gPhoto2 interface classes
namespace GPhoto2
{
/// checks for gPhoto2 error and throws exception
void CheckError(const CString& cszFunction, int err, LPCSTR pszFile, UINT uiLine);

/// gPhoto2 reference
class Ref: public SdkReferenceBase, public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref();

   /// adds gPhoto2 version text
   virtual void AddVersionText(CString& versionText) const override;

   /// enumerates cameras
   virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceDevicesList) const override;

   /// returns if AsyncWaitForCamera() is possible for this SDK
   virtual bool IsAsyncWaitPossible() const override { return false; }

private:
   /// gPhoto2 context
   std::shared_ptr<_GPContext> m_spContext;
};

/// smart pointer to gPhoto2 reference
typedef std::shared_ptr<Ref> RefSp;

}
