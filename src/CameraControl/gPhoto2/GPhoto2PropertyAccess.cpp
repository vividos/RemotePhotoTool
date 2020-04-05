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

PropertyAccess::PropertyAccess(std::shared_ptr<_GPContext> context, std::shared_ptr<_Camera> camera)
   :m_context(context),
   m_camera(camera)
{
   // since the list of property ids in the widget list changes with every
   // call to gp_camera_get_config(), we get the list once only, and work with
   // it.
   CameraWidget* widget = nullptr;
   int ret = gp_camera_get_config(camera.get(), &widget, context.get());
   CheckError(_T("gp_camera_get_config"), ret, __FILE__, __LINE__);

   m_widget.reset(widget, gp_widget_free);

   // note: we only enumerate device properties once, or else the ids would change
   DumpWidgetTree(m_widget.get(), 0);
   RecursiveAddProperties(m_widget.get(), m_mapDeviceProperties);
}

/// looks up child widget by widget name or label
static int lookup_widget(CameraWidget* widget, const char* key, CameraWidget** child)
{
   int ret = gp_widget_get_child_by_name(widget, key, child);

   if (ret < GP_OK)
      ret = gp_widget_get_child_by_label(widget, key, child);

   return ret;
}

/// Gets a string configuration value. This can be:
///  - A Text widget
///  - The current selection of a Radio Button choice
///  - The current selection of a Menu choice
static int get_config_value_string(CameraWidget* widget, const char* key, CString& text)
{
   CameraWidget* child = nullptr;
   int ret = lookup_widget(widget, key, &child);
   if (ret < GP_OK)
   {
      LOG_TRACE(_T("gPhoto2: lookup widget failed: %d\n"), ret);
      return ret;
   }

   // This type check is optional, if you know what type the label
   // has already. If you are not sure, better check.
   CameraWidgetType type;
   ret = gp_widget_get_type(child, &type);
   if (ret < GP_OK)
   {
      LOG_TRACE(_T("gPhoto2: widget get type failed: %d\n"), ret);
      return ret;
   }

   switch (type)
   {
   case GP_WIDGET_MENU:
   case GP_WIDGET_RADIO:
   case GP_WIDGET_TEXT:
      break;

   default:
      LOG_TRACE(_T("gPhoto2: widget has bad type %d\n"), type);
      return GP_ERROR_BAD_PARAMETERS;
   }

   // This is the actual query call. Note that we just
   // a pointer reference to the string, not a copy...
   char* val = nullptr;
   ret = gp_widget_get_value(child, &val);
   if (ret < GP_OK)
   {
      LOG_TRACE(_T("gPhoto2: could not query widget value: %d\n"), ret);
      return ret;
   }

   text = val;

   return ret;
}

CString PropertyAccess::GetText(LPCSTR configValueName) const
{
   CString text;

   int ret = get_config_value_string(m_widget.get(), configValueName, text);
   CheckError(_T("get_config_value_string"), ret, __FILE__, __LINE__);

   return text;
}

bool PropertyAccess::GetCameraOperationAbility(unsigned int operation) const
{
   CameraAbilities abilities = { 0 };

   int ret = gp_camera_get_abilities(m_camera.get(), &abilities);
   if (ret < GP_OK)
      return false;

   return (abilities.operations & operation) != 0;
}

unsigned int PropertyAccess::MapImagePropertyTypeToId(T_enImagePropertyType imagePropertyType) const
{
   LPCSTR propertyName = nullptr;
   switch (imagePropertyType)
   {
   case propShootingMode: propertyName = "shootingmode"; break;
   case propDriveMode:
   default:
      break;
   }

   if (propertyName != nullptr)
   {
      CameraWidget* child = nullptr;
      int ret = lookup_widget(m_widget.get(), CStringA(propertyName), &child);
      CheckError(_T("lookup_widget"), ret, __FILE__, __LINE__);

      int propertyId = -1;
      ret = gp_widget_get_id(child, &propertyId);
      CheckError(_T("gp_widget_get_id"), ret, __FILE__, __LINE__);

      return static_cast<unsigned int>(propertyId);
   }

   return static_cast<unsigned int>(-1);
}

ImageProperty PropertyAccess::MapShootingModeToImagePropertyValue(RemoteReleaseControl::T_enShootingMode shootingMode) const
{
   unsigned int propertyId = MapImagePropertyTypeToId(T_enImagePropertyType::propShootingMode);

   std::vector<ImageProperty> shootingModeList = EnumImagePropertyValues(propertyId);

   switch (shootingMode)
   {
   default:
      ATLASSERT(false); // invalid shooting mode
      break;
   }

   // TODO implement
   Variant value;
   value.Set<unsigned char>(42);
   value.SetType(Variant::typeUInt8);

   return ImageProperty(T_enSDKVariant::variantGphoto2, 0, value, true);
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
   CameraWidget* child = nullptr;
   int ret = gp_widget_get_child_by_id(m_widget.get(), static_cast<int>(propertyId), &child);
   CheckError(_T("gp_widget_get_child_by_id"), ret, __FILE__, __LINE__);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   ret = gp_widget_get_type(child, &type);
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
   CameraWidget* child = nullptr;
   int ret = gp_widget_get_child_by_id(m_widget.get(), static_cast<int>(imagePropertyId), &child);
   CheckError(_T("gp_widget_get_child_by_id"), ret, __FILE__, __LINE__);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   ret = gp_widget_get_type(child, &type);
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
   CameraWidget* child = nullptr;
   int ret = gp_widget_get_child_by_id(m_widget.get(), static_cast<int>(imagePropertyId), &child);
   CheckError(_T("gp_widget_get_child_by_id"), ret, __FILE__, __LINE__);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   ret = gp_widget_get_type(child, &type);
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
   int ret = lookup_widget(m_widget.get(), CStringA(propertyName), &child);
   CheckError(_T("lookup_widget"), ret, __FILE__, __LINE__);

   SetPropertyByWidget(child, value);
}

void PropertyAccess::SetPropertyById(unsigned int propertyId, const Variant& value)
{
   CameraWidget* child = nullptr;
   int ret = gp_widget_get_child_by_id(m_widget.get(), static_cast<int>(propertyId), &child);
   CheckError(_T("gp_widget_get_child_by_id"), ret, __FILE__, __LINE__);

   SetPropertyByWidget(child, value);
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
      float rangeValue = 0.0f;
      // TODO
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

   ret = gp_camera_set_config(m_camera.get(), m_widget.get(), m_context.get());
   CheckError(_T("gp_camera_set_config"), ret, __FILE__, __LINE__);
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
   // TODO implement
   UNUSED(propertyId);

   return value.ToString();
}

LPCTSTR PropertyAccess::NameFromId(unsigned int propertyId)
{
   if (m_mapDeviceProperties.find(propertyId) == m_mapDeviceProperties.end())
   {
      return _T("???");
   }

   const char* label = nullptr;
   gp_widget_get_label(m_mapDeviceProperties[propertyId], &label);

   static CString s_propName(label);

   return s_propName.GetString();
}

void PropertyAccess::RecursiveAddProperties(CameraWidget* widget, std::map<unsigned int, CameraWidget*>& mapDeviceProperties) const
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

         RecursiveAddProperties(child, mapDeviceProperties);
      }
   }
   else
      if (type == GP_WIDGET_TEXT ||
         type == GP_WIDGET_MENU ||
         type == GP_WIDGET_RADIO ||
         type == GP_WIDGET_DATE ||
         type == GP_WIDGET_TOGGLE)
      {
         int id = 0;
         ret = gp_widget_get_id(widget, &id);
         CheckError(_T("gp_widget_get_id"), ret, __FILE__, __LINE__);

         mapDeviceProperties.insert(std::make_pair(static_cast<unsigned int>(id), widget));
      }
}

void PropertyAccess::DumpWidgetTree(CameraWidget* widget, int indendationLevel)
{
   const char* name = nullptr;
   gp_widget_get_name(widget, &name);

   int count = gp_widget_count_children(widget);

   int id = 0;
   gp_widget_get_id(widget, &id);

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
      id,
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
