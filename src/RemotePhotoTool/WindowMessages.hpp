//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file WindowMessages.hpp Window messages
//
#pragma once

// app wide window message

/// sent when viewfinder image is available
#define WM_VIEWFINDER_AVAIL_IMAGE WM_APP+1

/// sent when property has changed; wParam has image property id
#define WM_RELEASECONTROL_PROPERTY_CHANGED WM_APP+2

/// sent when all properties have changed
#define WM_RELEASECONTROL_ALL_PROPERTIES_CHANGED WM_APP+3

/// sent to lock or unlock action mode
#define WM_LOCK_ACTIONMODE WM_APP+4

/// sent to PreviousImagesView to update current image
#define WM_PREV_IMAGES_UPDATE WM_APP + 5
