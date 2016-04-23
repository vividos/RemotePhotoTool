//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file WindowPlacement.hpp window placement
//
#pragma once

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
   void Store(CRegKey& regRoot, LPCTSTR pszPrefix) const
   {
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_Flags"), flags);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_ShowCmd"), showCmd);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_MinPositionX"), ptMinPosition.x);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_MinPositionY"), ptMinPosition.y);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_MaxPositionX"), ptMaxPosition.x);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_MaxPositionY"), ptMaxPosition.y);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_NormalPositionLeft"), rcNormalPosition.left);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_NormalPositionTop"), rcNormalPosition.top);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_NormalPositionRight"), rcNormalPosition.right);
      regRoot.SetDWORDValue(CString(pszPrefix) + _T("_NormalPositionBottom"), rcNormalPosition.bottom);
   }

   /// loads window placement from registy key, with given prefix
   void Load(CRegKey& regRoot, LPCTSTR pszPrefix)
   {
      showCmd = 0; // default value, to decide if load was successful

      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_Flags"), flags);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_ShowCmd"), showCmd);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_MinPositionX"), ptMinPosition.x);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_MinPositionY"), ptMinPosition.y);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_MaxPositionX"), ptMaxPosition.x);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_MaxPositionY"), ptMaxPosition.y);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_NormalPositionLeft"), rcNormalPosition.left);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_NormalPositionTop"), rcNormalPosition.top);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_NormalPositionRight"), rcNormalPosition.right);
      ReadIntegerValue(regRoot, CString(pszPrefix) + _T("_NormalPositionBottom"), rcNormalPosition.bottom);
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
