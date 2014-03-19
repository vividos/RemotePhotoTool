//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file CdsdkCommon.hpp CDSDK - Common functions
//
#pragma once

// includes
#pragma warning(push)
#include "cdAPI.h"
#pragma warning(pop)
#include "SourceInfo.hpp"
#include <vector>

/// Canon Digital Camera SDK interface
namespace CDSDK
{

/// checks for CDSDK error
void CheckError(const CString& cszFunction, cdError err, LPCSTR pszFile, UINT uiLine);

/// SDK reference
class Ref: public std::enable_shared_from_this<Ref>
{
public:
   /// ctor
   Ref();
   /// dtor
   ~Ref() throw();

   /// adds version text
   void AddVersionText(CString& cszVersionText) const;

   /// enumerates devices
   void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& vecSourceDevices) const;
};

/// shared ptr to SDK reference
typedef std::shared_ptr<Ref> RefSp;

} // namespace CDSDK
