//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PreviousImagesView.cpp Previous images view
//

// includes
#include "stdafx.h"
#include "resource.h"
#include "PreviousImagesView.hpp"
#include "IPhotoModeViewHost.hpp"

void PreviousImagesView::DestroyView()
{
   BOOL bRet = DestroyWindow();
   ATLASSERT(TRUE == bRet); bRet;
}
