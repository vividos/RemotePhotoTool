//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImagePropertyValueManager.hpp Image property value manager
//
#pragma once

// includes
#include "RemoteReleaseControl.hpp"

// forward references
class IImagePropertyControl;

/// image property value manager
class ImagePropertyValueManager : public boost::noncopyable
{
public:
   /// ctor
   ImagePropertyValueManager(RemoteReleaseControl& rrc);
   /// dtor
   ~ImagePropertyValueManager() throw();

   /// adds control to manager
   void AddControl(IImagePropertyControl& rControl)
   {
      m_vecControls.push_back(&rControl);
   }

   /// updates one property manually
   void UpdateProperty(unsigned int uiValue);

   /// updates all controls
   void UpdateControls();

private:
   /// called when property has changed
   void OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue);

private:
   /// release control
   RemoteReleaseControl& m_rrc;

   /// handler id for callback for property changes
   int m_iPropertyHandlerId;

   /// all managed controls
   std::vector<IImagePropertyControl*> m_vecControls;
};
