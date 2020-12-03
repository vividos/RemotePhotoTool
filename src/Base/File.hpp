//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file File.hpp File related functions
//
#pragma once

#include <vector>
#include <ctime>

/// file related functions
class File
{
public:
   /// writes all bytes in the vector to the file
   static void WriteAllBytes(LPCTSTR filename, const std::vector<unsigned char>& data);

   /// sets modified time of file
   static void SetModifiedTime(LPCTSTR filename, time_t modifiedTime);
};
