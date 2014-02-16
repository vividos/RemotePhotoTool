//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file FileFinder.hpp file finder
//
#pragma once

/// file finder
class FileFinder
{
public:
   /// ctor; starts finding files
   FileFinder(const CString& cszBaseFolder, const CString& cszFileSpec)
      :m_hFind(INVALID_HANDLE_VALUE),
       m_cszBaseFolder(cszBaseFolder)
   {
      if (m_cszBaseFolder.Right(1) != _T("\\"))
         m_cszBaseFolder += _T("\\");

      ::ZeroMemory(&m_findData, sizeof(m_findData));
      m_hFind = ::FindFirstFile(cszBaseFolder + cszFileSpec, &m_findData);
   }

   /// dtor
   ~FileFinder() throw()
   {
      if (m_hFind != INVALID_HANDLE_VALUE)
         ATLVERIFY(TRUE == ::FindClose(m_hFind));
   }

   /// indicates if any files were found
   bool IsValid() const throw() { return m_hFind != INVALID_HANDLE_VALUE; }

   /// returns if the current file entry is a dot file, "." or ".."
   bool IsDot() const throw()
   {
      ATLASSERT(IsValid() == true);

      if ((m_findData.cFileName[0] == _T('.') && m_findData.cFileName[1] == _T('\0')) ||
          (m_findData.cFileName[0] == _T('.') && m_findData.cFileName[1] == _T('.') && m_findData.cFileName[2] == _T('\0')))
         return true;
      return false;
   }

   /// returns if current file entry is a file
   bool IsFile() const throw()
   {
      return (m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
   }

   /// returns if current file entry is a folder
   bool IsFolder() const throw()
   {
      return (m_findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
   }

   /// returns complete filename of current file entry
   CString Filename() const throw()
   {
      CString cszName = m_cszBaseFolder + m_findData.cFileName;
      if (IsFolder())
         cszName += _T("\\");
      return cszName;
   }

   /// retrieves next file entry
   bool Next() throw()
   {
      ATLASSERT(IsValid() == true);
      return TRUE == ::FindNextFile(m_hFind, &m_findData);
   }

private:
   /// base folder of search
   CString m_cszBaseFolder;

   /// current find entry
   WIN32_FIND_DATA m_findData;

   /// find handle
   HANDLE m_hFind;
};
