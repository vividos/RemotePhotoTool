//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ErrorText.hpp Canon control - Error texts
//
#pragma once

/// \brief returns error text from error id
/// \details since all Canon SDKs use the same error ids (when masked), this function
/// returns the text for all SDKs.
LPCTSTR ErrorTextFromErrorId(unsigned int uiErrorId, bool bIsPsrec = false);
