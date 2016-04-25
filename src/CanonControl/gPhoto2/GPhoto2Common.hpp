//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2Common.hpp gPhoto2 - Common functions
//
#pragma once

// includes
#include "Instance.hpp"
#include "CameraException.hpp"

// forward references
struct _GPContext;
struct _Camera;

/// gPhoto2 interface classes
namespace GPhoto2
{
/// checks for gPhoto2 error and throws exception
void CheckError(const CString& cszFunction, int err, LPCSTR pszFile, UINT uiLine);

/// gPhoto2 reference
class Ref: public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref() throw();

   /// adds gPhoto2 version text
   void AddVersionText(CString& cszVersionText) const;

   /// enumerates cameras
   void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const;

private:
   /// gPhoto2 context
   std::shared_ptr<_GPContext> m_spContext;
};

/// smart pointer to gPhoto2 reference
typedef std::shared_ptr<Ref> RefSp;

}
