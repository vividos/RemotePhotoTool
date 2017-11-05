//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file ImagePropertyValueManager.cpp Image property value manager
//

// includes
#include "stdafx.h"
#include "ImagePropertyValueManager.hpp"
#include "IImagePropertyControl.hpp"

ImagePropertyValueManager::ImagePropertyValueManager(RemoteReleaseControl& rrc)
:m_rrc(rrc),
 m_iPropertyHandlerId(rrc.AddPropertyEventHandler(std::bind(&ImagePropertyValueManager::OnUpdatedProperty, this, std::placeholders::_1, std::placeholders::_2)))
{
}

ImagePropertyValueManager::~ImagePropertyValueManager()
{
   try
   {
      m_rrc.RemovePropertyEventHandler(m_iPropertyHandlerId);
   }
   catch (...)
   {
   }
}

void ImagePropertyValueManager::UpdateProperty(unsigned int uiValue)
{
   OnUpdatedProperty(RemoteReleaseControl::propEventPropertyDescChanged, uiValue);
   OnUpdatedProperty(RemoteReleaseControl::propEventPropertyChanged, uiValue);
}

void ImagePropertyValueManager::UpdateControls()
{
   std::for_each(m_vecControls.begin(), m_vecControls.end(), [&](IImagePropertyControl* pControl)
   {
      pControl->UpdateValuesList();
      pControl->UpdateValue();
   });
}

void ImagePropertyValueManager::OnUpdatedProperty(RemoteReleaseControl::T_enPropertyEvent enPropertyEvent, unsigned int uiValue)
{
   // my first lambda expression
   std::for_each(m_vecControls.begin(), m_vecControls.end(), [&](IImagePropertyControl* pControl)
   {
      // when 0 was passed, update all properties; when not, update only exact property
      unsigned int uiControlPropertyId = pControl->GetPropertyId();
      if (uiValue != 0 &&
          uiControlPropertyId != 0 &&
          uiControlPropertyId != uiValue)
         return;

      if (enPropertyEvent == RemoteReleaseControl::propEventPropertyChanged)
         pControl->UpdateValue();
      else
         pControl->UpdateValuesList();
   });
}
