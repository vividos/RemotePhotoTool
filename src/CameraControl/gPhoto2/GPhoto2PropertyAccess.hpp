//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2PropertyAccess.hpp gPhoto2 - Property access
//
#pragma once

#include "GPhoto2Common.hpp"
#include "Variant.hpp"
#include "DeviceProperty.hpp"
#include "ImageProperty.hpp"
#include "RemoteReleaseControl.hpp"

/// camera widget type
typedef struct _CameraWidget CameraWidget;

namespace GPhoto2
{
   /// access to properties of gPhoto2 camera
   class PropertyAccess : public std::enable_shared_from_this<PropertyAccess>
   {
   public:
      /// ctor
      PropertyAccess(RefSp ref, std::shared_ptr<_GPContext> context, std::shared_ptr<_Camera> camera);

      /// refreshes all properties from the camera
      void Refresh();

      /// returns config value of camera, as text
      CString GetText(LPCSTR configValueName) const;

      /// returns list of all valid values
      std::vector<CString> GetValidValues(LPCSTR configValueName) const;

      /// returns camera operation ability; use CameraOption enum for operation
      bool GetCameraOperationAbility(unsigned int operation) const;

      /// returns if a property with given name is availble
      bool IsAvailPropertyName(LPCSTR configValueName) const;

      /// maps image property type to a property ID
      unsigned int MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const;

      /// maps shooting mode to a specific image property value
      ImageProperty MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode shootingMode) const;

      /// enumerates all device property IDs
      std::vector<unsigned int> EnumDeviceProperties() const;

      /// returns a device property for given property ID
      DeviceProperty GetDeviceProperty(unsigned int propertyId) const;

      /// enumerates all image property IDs
      std::vector<unsigned int> EnumImageProperties() const;

      /// returns an image property for given property ID
      ImageProperty GetImageProperty(unsigned int imagePropertyId) const;

      /// enumerates all image properties
      std::vector<ImageProperty> EnumImagePropertyValues(unsigned int imagePropertyId) const;

      /// sets property by given gPhoto2 name
      void SetPropertyByName(LPCTSTR propertyName, const Variant& value);

      /// sets property by property ID
      void SetPropertyById(unsigned int propertyId, const Variant& value);

      /// returns displayable text from property id and value
      CString DisplayTextFromIdAndValue(unsigned int propertyId, Variant value);

      /// returns name from given property id
      LPCTSTR NameFromId(unsigned int propertyId);

   private:
      /// looks up child widget by widget name or label
      static int LookupWidget(CameraWidget* widget, const char* key, CameraWidget** child);

      /// sets property value for given widget
      void SetPropertyByWidget(CameraWidget* widget, const Variant& value);

      /// gets a property ID from given widget
      static unsigned int GetPropertyIdFromWidget(CameraWidget* widget);

      /// gets a camera widget from a property ID
      CameraWidget* GetWidgetFromPropertyId(unsigned int propertyId) const;

      /// reads property value from widget and stores it as variant value
      static void ReadPropertyValue(CameraWidget* widget, Variant& value, int type);

      /// reads all valid values for widget and stores it in variant list
      static void ReadValidValues(std::vector<Variant>& validValuesList, CameraWidget* widget, int type);

      /// recursively adds all properties found in sub-widgets
      void RecursiveAddProperties(CameraWidget* widget);

      /// dumps widget tree to logging
      static void DumpWidgetTree(CameraWidget* widget, int indendationLevel);

   private:
      /// gPhoto2 reference
      RefSp m_ref;

      /// gPhoto2 context
      std::shared_ptr<_GPContext> m_context;

      /// camera instance
      std::shared_ptr<_Camera> m_camera;

      /// camera widget with all configurable properties
      std::shared_ptr<CameraWidget> m_widget;

      /// device properties, by property ID
      std::map<unsigned int, CameraWidget*> m_mapDeviceProperties;

      /// image properties, by property ID
      std::map<unsigned int, CameraWidget*> m_mapImageProperties;

      /// property names
      std::map<unsigned int, CString> m_mapPropertyNames;
   };

} // namespace GPhoto2
