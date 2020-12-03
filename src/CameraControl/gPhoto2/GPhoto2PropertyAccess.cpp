//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2PropertyAccess.cpp gPhoto2 - Property access
//
#include "stdafx.h"
#include "GPhoto2PropertyAccess.hpp"
#include "GPhoto2Include.hpp"
#include "CameraException.hpp"

using GPhoto2::PropertyAccess;

PropertyAccess::PropertyAccess(RefSp ref, std::shared_ptr<_GPContext> context, std::shared_ptr<_Camera> camera)
   :m_ref(ref),
   m_context(context),
   m_camera(camera)
{
   Refresh();
}

void PropertyAccess::Refresh()
{
   m_mapImageProperties.clear();
   m_mapPropertyNames.clear();

   CameraWidget* widget = nullptr;
   int ret = gp_camera_get_config(m_camera.get(), &widget, m_context.get());
   CheckError(_T("gp_camera_get_config"), ret, __FILE__, __LINE__);

   m_widget.reset(widget, gp_widget_free);

   DumpWidgetTree(m_widget.get(), 0);
   RecursiveAddProperties(m_widget.get());
}

int PropertyAccess::LookupWidget(CameraWidget* widget, const char* key, CameraWidget** child)
{
   int ret = gp_widget_get_child_by_name(widget, key, child);

   if (ret < GP_OK)
      ret = gp_widget_get_child_by_label(widget, key, child);

   return ret;
}

CString PropertyAccess::GetText(LPCSTR configValueName) const
{
   CameraWidget* child = nullptr;
   int ret = LookupWidget(m_widget.get(), configValueName, &child);
   CheckError(_T("LookupWidget"), ret, __FILE__, __LINE__);

   // This type check is optional, if you know what type the label
   // has already. If you are not sure, better check.
   CameraWidgetType type;
   ret = gp_widget_get_type(child, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   if (type != GP_WIDGET_MENU &&
      type != GP_WIDGET_RADIO &&
      type != GP_WIDGET_TEXT)
      return _T("???");

   const char* text = nullptr;
   ret = gp_widget_get_value(child, &text);
   CheckError(_T("gp_widget_get_value"), ret, __FILE__, __LINE__);

   return CString(text);
}

std::vector<CString> PropertyAccess::GetValidValues(LPCSTR configValueName) const
{
   CameraWidget* child = nullptr;
   int ret = LookupWidget(m_widget.get(), configValueName, &child);
   CheckError(_T("LookupWidget"), ret, __FILE__, __LINE__);

   CameraWidgetType type;
   ret = gp_widget_get_type(child, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   std::vector<CString> validValuesList;

   if (type == GP_WIDGET_RADIO ||
      type == GP_WIDGET_MENU)
   {
      int choiceCount = gp_widget_count_choices(child);

      for (int index = 0; index < choiceCount; index++)
      {
         const char* choiceText = nullptr;
         ret = gp_widget_get_choice(child, index, &choiceText);
         CheckError(_T("gp_widget_get_choice"), ret, __FILE__, __LINE__);

         validValuesList.push_back(CString(choiceText));
      }
   }

   return validValuesList;
}

bool PropertyAccess::GetCameraOperationAbility(unsigned int operation) const
{
   CameraAbilities abilities = { 0 };

   int ret = gp_camera_get_abilities(m_camera.get(), &abilities);
   if (ret < GP_OK)
      return false;

   return (abilities.operations & operation) != 0;
}

bool PropertyAccess::IsAvailPropertyName(LPCSTR configValueName) const
{
   CameraWidget* child = nullptr;
   int ret = LookupWidget(m_widget.get(), configValueName, &child);

   return ret == GP_OK && child != nullptr;
}

unsigned int PropertyAccess::MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const
{
   LPCSTR propertyName = nullptr;
   switch (imagePropertyType)
   {
   case propShootingMode: propertyName = "shootingmode"; break;
   case propDriveMode:
      if (IsAvailPropertyName("drivemode")) propertyName = "drivemode";
      if (IsAvailPropertyName("Drive Mode")) propertyName = "Drive Mode";
      break;
   case propISOSpeed: propertyName = "iso"; break;
   case propMeteringMode: propertyName = "meteringmode"; break;
   case propAFMode: if (IsAvailPropertyName("Metering Mode")) propertyName = "Metering Mode"; break;
   case propAv: propertyName = "aperture"; break;
   case propTv: propertyName = "shutterspeed"; break;
   case propExposureCompensation: propertyName = "exposurecompensation"; break;
   case propFlashExposureComp: propertyName = "flashcompensation"; break;
      //case propFocalLength: break;
   case propFlashMode: propertyName = "flashmode"; break;
   case propWhiteBalance: propertyName = "whitebalance"; break;
   case propAFDistance: propertyName = "afdistance"; break;
   case propCurrentZoomPos:
      if (IsAvailPropertyName("Zoom")) propertyName = "Zoom";
      if (IsAvailPropertyName("zoom")) propertyName = "zoom";
      break;
   case propMaxZoomPos:
      // there is no zoom_max in gPhoto2, we handle it by ourselves
      propertyName = "zoom_max";
      break;
   case propAvailableShots:
      if (IsAvailPropertyName("availableshots")) propertyName = "availableshots";
      if (IsAvailPropertyName("Available Shots")) propertyName = "Available Shots";
      break;
   case propSaveTo: propertyName = "capturetarget"; break;
   case propBatteryLevel:
      if (IsAvailPropertyName("batterylevel")) propertyName = "batterylevel";
      if (IsAvailPropertyName("Battery Mode")) propertyName = "Battery Mode";
      break;
   case propImageFormat: propertyName = "imageformat"; break;
   case propUnknown:
   default:
      break;
   }

   if (propertyName != nullptr)
   {
      CameraWidget* child = nullptr;
      int ret = LookupWidget(m_widget.get(), CStringA(propertyName), &child);
      CheckError(_T("LookupWidget"), ret, __FILE__, __LINE__);

      return GetPropertyIdFromWidget(child);
   }

   return static_cast<unsigned int>(-1);
}

ImageProperty PropertyAccess::MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode shootingMode) const
{
   unsigned int propertyId = MapImagePropertyTypeToId(T_enImagePropertyType::propShootingMode);

   std::vector<ImageProperty> shootingModeList = EnumImagePropertyValues(propertyId);

   for (const ImageProperty& shootingModeValue : shootingModeList)
   {
      CString value = shootingModeValue.AsString();
      value.MakeUpper();

      if (value == "AV" && shootingMode == RemoteReleaseControl::shootingModeAv)
         return shootingModeValue;
      else if (value == "TV" && shootingMode == RemoteReleaseControl::shootingModeTv)
         return shootingModeValue;
      else if (value == "P" && shootingMode == RemoteReleaseControl::shootingModeP)
         return shootingModeValue;
      else if ((value == "M" || value == "MANUAL") && shootingMode == RemoteReleaseControl::shootingModeM)
         return shootingModeValue;
   }

   // invalid shooting mode
   ATLASSERT(false);
   throw CameraException(
      _T("MapShootingModeToImagePropertyValue"),
      _T("invalid shooting mode"),
      static_cast<unsigned int>(GP_ERROR), __FILE__, __LINE__);
}

std::vector<unsigned int> PropertyAccess::EnumDeviceProperties() const
{
   std::vector<unsigned int> devicePropertiesList;

   for (const std::pair<unsigned int, CameraWidget*>& value : m_mapDeviceProperties)
   {
      devicePropertiesList.push_back(value.first);
   }

   return devicePropertiesList;
}

DeviceProperty PropertyAccess::GetDeviceProperty(unsigned int propertyId) const
{
   CameraWidget* child = GetWidgetFromPropertyId(propertyId);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   int ret = gp_widget_get_type(child, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   int readonly = 1;
   ret = gp_widget_get_readonly(child, &readonly);
   CheckError(_T("gp_widget_get_readonly"), ret, __FILE__, __LINE__);

   Variant value;
   ReadPropertyValue(child, value, type);

   DeviceProperty deviceProperty(T_enSDKVariant::variantGphoto2, propertyId, value, readonly != 0,
      std::static_pointer_cast<void>(std::const_pointer_cast<PropertyAccess>(shared_from_this())));

   ReadValidValues(deviceProperty.ValidValues(), child, type);

   return deviceProperty;
}

std::vector<unsigned int> PropertyAccess::EnumImageProperties() const
{
   std::vector<unsigned int> imagePropertiesList;

   for (const std::pair<unsigned int, CameraWidget*>& value : m_mapImageProperties)
   {
      imagePropertiesList.push_back(value.first);
   }

   return imagePropertiesList;
}

ImageProperty PropertyAccess::GetImageProperty(unsigned int imagePropertyId) const
{
   if (m_mapImageProperties.find(imagePropertyId) == m_mapImageProperties.end())
      CheckError(_T("m_mapImageProperties"), GP_ERROR, __FILE__, __LINE__);

   CameraWidget* child = GetWidgetFromPropertyId(imagePropertyId);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   int ret = gp_widget_get_type(child, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   int readonly = 1;
   ret = gp_widget_get_readonly(child, &readonly);
   CheckError(_T("gp_widget_get_readonly"), ret, __FILE__, __LINE__);

   Variant value;
   ReadPropertyValue(child, value, type);

   ImageProperty imageProperty(T_enSDKVariant::variantGphoto2, imagePropertyId, value, readonly != 0,
      std::static_pointer_cast<void>(std::const_pointer_cast<PropertyAccess>(shared_from_this())));

   return imageProperty;
}

std::vector<ImageProperty> PropertyAccess::EnumImagePropertyValues(unsigned int imagePropertyId) const
{
   if (m_mapImageProperties.find(imagePropertyId) == m_mapImageProperties.end())
      CheckError(_T("m_mapImageProperties"), GP_ERROR, __FILE__, __LINE__);

   CameraWidget* child = GetWidgetFromPropertyId(imagePropertyId);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   int ret = gp_widget_get_type(child, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   int readonly = 1;
   ret = gp_widget_get_readonly(child, &readonly);
   CheckError(_T("gp_widget_get_readonly"), ret, __FILE__, __LINE__);

   std::vector<Variant> validValues;
   ReadValidValues(validValues, child, type);

   std::vector<ImageProperty> imagePropertyList;

   for (const Variant& value : validValues)
   {
      ImageProperty imageProperty(T_enSDKVariant::variantGphoto2, imagePropertyId, value, readonly != 0,
         std::static_pointer_cast<void>(std::const_pointer_cast<PropertyAccess>(shared_from_this())));

      imagePropertyList.push_back(imageProperty);
   }

   return imagePropertyList;
}

void PropertyAccess::SetPropertyByName(LPCTSTR propertyName, const Variant& value)
{
   CameraWidget* child = nullptr;
   int ret = LookupWidget(m_widget.get(), CStringA(propertyName), &child);
   CheckError(_T("LookupWidget"), ret, __FILE__, __LINE__);

   SetPropertyByWidget(child, value);
}

void PropertyAccess::SetPropertyById(unsigned int propertyId, const Variant& value)
{
   CameraWidget* widget = GetWidgetFromPropertyId(propertyId);

   SetPropertyByWidget(widget, value);
}

void PropertyAccess::SetPropertyByWidget(CameraWidget* widget, const Variant& value)
{
   CameraWidgetType type = GP_WIDGET_WINDOW;
   int ret = gp_widget_get_type(widget, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   switch (type)
   {
   case GP_WIDGET_MENU:
   case GP_WIDGET_TEXT:
   case GP_WIDGET_RADIO:
      ret = gp_widget_set_value(widget, CStringA(value.Get<CString>()).GetString());
      break;

   case GP_WIDGET_RANGE:
   {
      float rangeValue = value.Get<float>();
      ret = gp_widget_set_value(widget, &rangeValue);
   }
   break;

   //case GP_WIDGET_DATE:
   // pass an int
   //   break;
   case GP_WIDGET_TOGGLE:
   {
      int toggleValue =
         value.Type() == Variant::typeString ? (value.Get<CString>() == _T("on") ? 1 : 0) :
         value.Type() == Variant::typeBool ? value.Get<bool>() ? 1 : 0 :
         value.Type() == Variant::typeInt32 ? value.Get<int>() :
         false; // maybe add more types?

      ret = gp_widget_set_value(widget, &toggleValue);
   }
   break;

   default:
      ATLASSERT(false);
      ret = GP_ERROR;
      break;
   }

   CheckError(_T("gp_widget_set_value"), ret, __FILE__, __LINE__);

   bool retry;
   do
   {
      retry = false;

      ret = gp_camera_set_config(m_camera.get(), m_widget.get(), m_context.get());
      if (ret == GP_ERROR_CAMERA_BUSY)
      {
         ATLTRACE(_T("retrying setting value: ") + value.ToString());
         retry = true;
         Sleep(100);
      }
      else
         CheckError(_T("gp_camera_set_config"), ret, __FILE__, __LINE__);
   } while (retry);
}

unsigned int PropertyAccess::GetPropertyIdFromWidget(CameraWidget* widget)
{
   const char* name = nullptr;
   int ret = gp_widget_get_name(widget, &name);
   CheckError(_T("gp_widget_get_name"), ret, __FILE__, __LINE__);

   unsigned int propertyId = std::hash<std::string>()(std::string(name));

   return propertyId;
}

CameraWidget* PropertyAccess::GetWidgetFromPropertyId(unsigned int propertyId) const
{
   auto iterDevice = m_mapDeviceProperties.find(propertyId);
   if (iterDevice != m_mapDeviceProperties.end())
      return iterDevice->second;

   auto iterImage = m_mapImageProperties.find(propertyId);
   if (iterImage != m_mapImageProperties.end())
      return iterImage->second;

   CheckError(_T("GetPropertyIdFromWidget"), GP_ERROR, __FILE__, __LINE__);
   return nullptr;
}

void PropertyAccess::ReadPropertyValue(CameraWidget* widget, Variant& value, int _type)
{
   CameraWidgetType type = (CameraWidgetType)_type;

   switch (type)
   {
   case GP_WIDGET_MENU:
   case GP_WIDGET_RADIO:
   case GP_WIDGET_TEXT:
   {
      char* text = nullptr;
      int ret = gp_widget_get_value(widget, &text);
      CheckError(_T("gp_widget_get_value"), ret, __FILE__, __LINE__);

      value.Set<CString>(CString(text));
      value.SetType(Variant::typeString);
   }
   break;

   case GP_WIDGET_RANGE:
   {
      float floatValue = 0.0;
      int ret = gp_widget_get_value(widget, &floatValue);
      CheckError(_T("gp_widget_get_value"), ret, __FILE__, __LINE__);

      value.Set<float>(floatValue);
      value.SetType(Variant::typeFloat);
   }
   break;

   case GP_WIDGET_TOGGLE:
   case GP_WIDGET_DATE:
   {
      int intValue = 0;
      int ret = gp_widget_get_value(widget, &intValue);
      CheckError(_T("gp_widget_get_value"), ret, __FILE__, __LINE__);

      value.Set<int>(intValue);
      value.SetType(Variant::typeInt32);
      // TODO  use typeTime for GP_WIDGET_DATE?
   }
   break;

   default:
      // nothing to read
      value.SetType(Variant::typeInvalid);
      break;
   }
}

void PropertyAccess::ReadValidValues(std::vector<Variant>& validValuesList, CameraWidget* widget, int _type)
{
   CameraWidgetType type = (CameraWidgetType)_type;

   if (type == GP_WIDGET_RANGE)
   {
      float minValue = 0.0, maxValue = 0.0, increment = 0.0;
      int ret = gp_widget_get_range(widget, &minValue, &maxValue, &increment);
      CheckError(_T("gp_widget_get_range"), ret, __FILE__, __LINE__);

      for (float floatValue = minValue; floatValue < maxValue; floatValue += increment)
      {
         Variant value;
         value.Set<float>(floatValue);
         value.SetType(Variant::typeFloat);

         validValuesList.push_back(value);
      }
   }

   if (type == GP_WIDGET_RADIO ||
      type == GP_WIDGET_MENU)
   {
      int choiceCount = gp_widget_count_choices(widget);

      for (int index = 0; index < choiceCount; index++)
      {
         const char* choiceText = nullptr;
         int ret = gp_widget_get_choice(widget, index, &choiceText);
         CheckError(_T("gp_widget_get_choice"), ret, __FILE__, __LINE__);

         Variant value;
         value.Set<CString>(CString(choiceText));
         value.SetType(Variant::typeString);

         validValuesList.push_back(value);
      }
   }
}

CString PropertyAccess::DisplayTextFromIdAndValue(unsigned int propertyId, Variant value)
{
   UNUSED(propertyId);

   return value.ToString();
}

LPCTSTR PropertyAccess::NameFromId(unsigned int propertyId)
{
   if (m_mapPropertyNames.find(propertyId) == m_mapPropertyNames.end())
      return _T("???");

   return m_mapPropertyNames.find(propertyId)->second.GetString();
}

void PropertyAccess::RecursiveAddProperties(CameraWidget* widget)
{
   int count = gp_widget_count_children(widget);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   int ret = gp_widget_get_type(widget, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   if (type == GP_WIDGET_WINDOW || type == GP_WIDGET_SECTION)
   {
      for (int i = 0; i < count; i++)
      {
         CameraWidget* child = nullptr;
         gp_widget_get_child(widget, i, &child);

         RecursiveAddProperties(child);
      }

      return;
   }

   if (type == GP_WIDGET_TEXT ||
      type == GP_WIDGET_MENU ||
      type == GP_WIDGET_RANGE ||
      type == GP_WIDGET_RADIO ||
      type == GP_WIDGET_DATE ||
      type == GP_WIDGET_TOGGLE)
   {
      unsigned int propertyId = GetPropertyIdFromWidget(widget);

      if (m_mapDeviceProperties.find(propertyId) != m_mapDeviceProperties.end())
         return; // already a device property

      const char* name = nullptr;
      gp_widget_get_name(widget, &name);

      const char* label = nullptr;
      gp_widget_get_label(widget, &label);

      CString propertyName;
      propertyName.Format(_T("%hs \"%hs\""), label, name);
      m_mapPropertyNames[propertyId] = propertyName;

      CameraWidget* parent = nullptr;
      gp_widget_get_parent(widget, &parent);

      const char* parentName = nullptr;
      gp_widget_get_name(parent, &parentName);

      CString parentNameText{ parentName };
      bool isImageProperty =
         parentNameText == "imgsettings" ||
         parentNameText == "capturesettings" ||
         parentNameText == "other";

      if (isImageProperty)
         m_mapImageProperties.insert(std::make_pair(propertyId, widget));
      else
         m_mapDeviceProperties.insert(std::make_pair(propertyId, widget));
   }
}

void PropertyAccess::DumpWidgetTree(CameraWidget* widget, int indendationLevel)
{
   const char* name = nullptr;
   gp_widget_get_name(widget, &name);

   int count = gp_widget_count_children(widget);

   unsigned int propertyId = GetPropertyIdFromWidget(widget);

   const char* label = nullptr;
   gp_widget_get_label(widget, &label);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   gp_widget_get_type(widget, &type);

   CString indendationSpaces(_T(' '), indendationLevel * 3);

   LPCTSTR typeText =
      type == GP_WIDGET_WINDOW ? _T("window") :
      type == GP_WIDGET_SECTION ? _T("section") :
      type == GP_WIDGET_TEXT ? _T("text") :
      type == GP_WIDGET_RANGE ? _T("range") :
      type == GP_WIDGET_TOGGLE ? _T("toggle") :
      type == GP_WIDGET_RADIO ? _T("radio") :
      type == GP_WIDGET_MENU ? _T("menu") :
      type == GP_WIDGET_BUTTON ? _T("button") :
      type == GP_WIDGET_DATE ? _T("date") : _T("???");

   LOG_TRACE(_T("%swidget name=\"%hs\", id=%04x, label=%hs, type=%s, %i children\n"),
      indendationSpaces.GetString(),
      name,
      propertyId,
      label,
      typeText,
      count);

   for (int i = 0; i < count; i++)
   {
      CameraWidget* child = nullptr;
      gp_widget_get_child(widget, i, &child);

      DumpWidgetTree(child, indendationLevel + 1);
   }
}
