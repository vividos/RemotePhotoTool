//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Path.hpp Path class
//
#pragma once

// includes

/// file and folder path class
class Path
{
public:
   // ctors

   /// default ctor
   Path() throw()
   {
   }

   /// ctor that takes a path
   Path(const CString& cszPath)
      :m_cszPath(cszPath)
   {
   }

   // operators

   /// returns CString
   operator const CString&() const throw() { return m_cszPath; }
   /// returns raw string pointer
   operator LPCTSTR() const throw() { return m_cszPath; }

   // getters

   /// returns directory name part of path
   CString DirectoryName() const;

   /// returns filename and extension
   CString FilenameAndExt() const;

   /// returns filename without extension
   CString FilenameOnly() const;

   /// returns path as string
   CString ToString() const { return m_cszPath; }

   /// returns if path represents a file and if it exists
   bool FileExists() const throw();

   /// returns if path represents a folder and if it exists
   bool FolderExists() const throw();

   // methods

   /// canonicalizes path by removing '..', etc.
   bool Canonicalize();

   /// combine path with given second part and return new path
   Path Combine(const CString& cszPart2);

   /// adds a backslash at the end of the path
   static void AddEndingBackslash(CString& cszPath);

   /// combine both path parts and return new path
   static Path Combine(const CString& cszPart1, const CString& cszPart2)
   {
      Path part1(cszPart1);
      return part1.Combine(cszPart2);
   }

   // public members

   /// path separator string
   static const TCHAR Separator[2];

   /// path separator character
   static const TCHAR SeparatorCh = _T('\\');

private:
   /// path
   CString m_cszPath;
};
