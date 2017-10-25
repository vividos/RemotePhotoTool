//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file GPhoto2PropertyAccess.cpp gPhoto2 - Property access
//

// includes
#include "stdafx.h"
#include "GPhoto2PropertyAccess.hpp"
#include "GPhoto2Include.hpp"
#include "CameraException.hpp"

using GPhoto2::PropertyAccess;

PropertyAccess::PropertyAccess(std::shared_ptr<_GPContext> spContext, std::shared_ptr<_Camera> spCamera)
   :m_spContext(spContext),
   m_spCamera(spCamera)
{
   // since the list of property ids in the widget list changes with every
   // call to gp_camera_get_config(), we get the list once only, and work with
   // it.
   CameraWidget* widget = nullptr;
   int ret = gp_camera_get_config(spCamera.get(), &widget, spContext.get());
   CheckError(_T("gp_camera_get_config"), ret, __FILE__, __LINE__);

   m_spWidget.reset(widget, gp_widget_free);

   // note: we only enumerate device properties once, or else the ids would change
   DumpWidgetTree(m_spWidget.get(), 0);
   RecursiveAddProperties(m_spWidget.get(), m_mapDeviceProperties);
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

   int ret = get_config_value_string(m_spWidget.get(), configValueName, text);
   CheckError(_T("get_config_value_string"), ret, __FILE__, __LINE__);

   return text;
}

bool PropertyAccess::GetCameraOperationAbility(unsigned int operation) const
{
   CameraAbilities abilities = { 0 };

   int ret = gp_camera_get_abilities(m_spCamera.get(), &abilities);
   if (ret < GP_OK)
      return false;

   return (abilities.operations & operation) != 0;
}

std::vector<unsigned int> PropertyAccess::EnumDeviceProperties() const
{
   std::vector<unsigned int> vecDeviceProperties;

   std::for_each(m_mapDeviceProperties.begin(), m_mapDeviceProperties.end(),
      [&vecDeviceProperties](std::pair<unsigned int, CameraWidget*> value)
   {
      vecDeviceProperties.push_back(value.first);
   });

   return vecDeviceProperties;
}

DeviceProperty PropertyAccess::GetDeviceProperty(unsigned int propId) const
{
   CameraWidget* child = nullptr;
   int ret = gp_widget_get_child_by_id(m_spWidget.get(), static_cast<int>(propId), &child);
   CheckError(_T("gp_widget_get_child_by_id"), ret, __FILE__, __LINE__);

   CameraWidgetType type = GP_WIDGET_WINDOW;
   ret = gp_widget_get_type(child, &type);
   CheckError(_T("gp_widget_get_type"), ret, __FILE__, __LINE__);

   int readonly = 1;
   ret = gp_widget_get_readonly(child, &readonly);
   CheckError(_T("gp_widget_get_readonly"), ret, __FILE__, __LINE__);

   Variant value;
   ReadPropertyValue(child, value, type);

   DeviceProperty dp(T_enSDKVariant::variantGphoto2, propId, value, readonly != 0,
      std::static_pointer_cast<void>(std::const_pointer_cast<PropertyAccess>(shared_from_this())));

   ReadValidValues(dp, child, type);

   return dp;
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

void PropertyAccess::ReadValidValues(DeviceProperty& dp, CameraWidget* widget, int _type)
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

         dp.ValidValues().push_back(value);
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

         dp.ValidValues().push_back(value);
      }
   }
}

CString PropertyAccess::DisplayTextFromIdAndValue(unsigned int propId, Variant value)
{
   // TODO implement
   UNUSED(propId);

   return value.ToString();
}

LPCTSTR PropertyAccess::NameFromId(unsigned int propId) throw()
{
   if (m_mapDeviceProperties.find(propId) == m_mapDeviceProperties.end())
   {
      return _T("???");
   }

   const char* label = nullptr;
   gp_widget_get_label(m_mapDeviceProperties[propId], &label);

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
