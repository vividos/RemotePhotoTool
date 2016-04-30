//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file WindowPlacement.hpp window placement
//
#pragma once

/// \brief window placement class
/// \details stores window coordinates and maximized state for storing and
/// restoring.
class WindowPlacement : public WINDOWPLACEMENT
{
public:
   /// ctor
   WindowPlacement()
   {
      ZeroMemory((WINDOWPLACEMENT*)this, sizeof(WINDOWPLACEMENT));
      length = sizeof(WINDOWPLACEMENT);
   }

   /// sets window placement for window
   void Set(HWND hWnd) throw()
   {
      ::SetWindowPlacement(hWnd, (WINDOWPLACEMENT*)this);
   }

   /// gets window placement from window
   void Get(HWND hWnd) const throw()
   {
      ::GetWindowPlacement(hWnd, (WINDOWPLACEMENT*)this);
   }

   /// stores window placement in registy key, with given prefix
   void Store(CRegKey& regRoot, const CString& cszPrefix) const
   {
      regRoot.SetDWORDValue(cszPrefix + _T("_Flags"), flags);
      regRoot.SetDWORDValue(cszPrefix + _T("_ShowCmd"), showCmd);
      regRoot.SetDWORDValue(cszPrefix + _T("_MinPositionX"), ptMinPosition.x);
      regRoot.SetDWORDValue(cszPrefix + _T("_MinPositionY"), ptMinPosition.y);
      regRoot.SetDWORDValue(cszPrefix + _T("_MaxPositionX"), ptMaxPosition.x);
      regRoot.SetDWORDValue(cszPrefix + _T("_MaxPositionY"), ptMaxPosition.y);
      regRoot.SetDWORDValue(cszPrefix + _T("_NormalPositionLeft"), rcNormalPosition.left);
      regRoot.SetDWORDValue(cszPrefix + _T("_NormalPositionTop"), rcNormalPosition.top);
      regRoot.SetDWORDValue(cszPrefix + _T("_NormalPositionRight"), rcNormalPosition.right);
      regRoot.SetDWORDValue(cszPrefix + _T("_NormalPositionBottom"), rcNormalPosition.bottom);
   }

   /// loads window placement from registy key, with given prefix
   void Load(CRegKey& regRoot, const CString& cszPrefix)
   {
      showCmd = 0; // default value, to decide if load was successful

      ReadIntegerValue(regRoot, cszPrefix + _T("_Flags"), flags);
      ReadIntegerValue(regRoot, cszPrefix + _T("_ShowCmd"), showCmd);
      ReadIntegerValue(regRoot, cszPrefix + _T("_MinPositionX"), ptMinPosition.x);
      ReadIntegerValue(regRoot, cszPrefix + _T("_MinPositionY"), ptMinPosition.y);
      ReadIntegerValue(regRoot, cszPrefix + _T("_MaxPositionX"), ptMaxPosition.x);
      ReadIntegerValue(regRoot, cszPrefix + _T("_MaxPositionY"), ptMaxPosition.y);
      ReadIntegerValue(regRoot, cszPrefix + _T("_NormalPositionLeft"), rcNormalPosition.left);
      ReadIntegerValue(regRoot, cszPrefix + _T("_NormalPositionTop"), rcNormalPosition.top);
      ReadIntegerValue(regRoot, cszPrefix + _T("_NormalPositionRight"), rcNormalPosition.right);
      ReadIntegerValue(regRoot, cszPrefix + _T("_NormalPositionBottom"), rcNormalPosition.bottom);
   }

private:
   /// reads integer value from registry
   template <typename T>
   void ReadIntegerValue(CRegKey& regKey, LPCTSTR pszName, T& value)
   {
      DWORD dwordValue;
      if (ERROR_SUCCESS == regKey.QueryDWORDValue(pszName, dwordValue))
         value = static_cast<T>(dwordValue);
   }
};
