//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2PropertyAccess.hpp gPhoto2 - Property access
//
#pragma once

// includes
#include "GPhoto2Common.hpp"
#include "Variant.hpp"
#include "DeviceProperty.hpp"

// forward references
/// camera widget type
typedef struct _CameraWidget CameraWidget;

namespace GPhoto2
{

/// access to properties of gPhoto2 camera
class PropertyAccess : public std::enable_shared_from_this<PropertyAccess>
{
public:
   /// ctor
   PropertyAccess(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera);

   /// returns config value of camera, as text
   CString GetText(LPCSTR configValueName) const;

   /// returns camera operation ability; use CameraOption enum for operation
   bool GetCameraOperationAbility(unsigned int operation) const;

   /// enumerates all property ids
   std::vector<unsigned int> EnumDeviceProperties() const;

   /// returns a property for given property id
   DeviceProperty GetDeviceProperty(unsigned int uiPropertyId) const;

   /// returns displayable text from property id and value
   CString DisplayTextFromIdAndValue(unsigned int propId, Variant value);

   /// returns name from given id
   LPCTSTR NameFromId(unsigned int propId);

private:
   /// reads property value from widget and stores it as variant value
   static void ReadPropertyValue(CameraWidget* widget, Variant& value, int type);

   /// reads all valid values for widget and stores it in device property
   static void ReadValidValues(DeviceProperty& dp, CameraWidget* widget, int type);

   /// recursively adds all properties found in sub-widgets
   void RecursiveAddProperties(CameraWidget* widget, std::map<unsigned int, CameraWidget*>& mapDeviceProperties) const;

   /// dumps widget tree to logging
   static void DumpWidgetTree(CameraWidget* widget, int indendationLevel);

private:
   /// gPhoto2 context
   std::shared_ptr<_GPContext> m_spContext;

   /// camera instance
   std::shared_ptr<_Camera> m_spCamera;

   /// camera widget with all configurable properties
   std::shared_ptr<CameraWidget> m_spWidget;

   /// device properties (cached)
   std::map<unsigned int, CameraWidget*> m_mapDeviceProperties;
};

} // namespace GPhoto2
