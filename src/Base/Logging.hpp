//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Logging.hpp Simple logging for SDK functions
//
#pragma once

/// log macro
#define LOG_TRACE LogTrace

/// configure logging
void LogConfigure(bool bEnable, const CString& cszLogfilePath);

/// logs message
void LogTrace(LPCTSTR pszFormat, ...);
