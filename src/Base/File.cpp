//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file File.cpp File related functions
//
#include "stdafx.h"
#include "File.hpp"
#include <ulib/SystemException.hpp>
#include <ulib/stream/FileStream.hpp>

void File::WriteAllBytes(LPCTSTR filename, const std::vector<unsigned char>& data)
{
   FILE* fd = nullptr;
   errno_t ret = _tfopen_s(&fd, filename, _T("wb"));
   if (ret != 0 || fd == nullptr)
      throw SystemException(_T("error opening file"), ret, __FILE__, __LINE__);

   std::shared_ptr<FILE> file{ fd, &fclose };

   fwrite(data.data(), 1, data.size(), file.get());

   file.reset();
}

/// converts a time_t value to FILETIME
/// \see https://docs.microsoft.com/en-us/windows/win32/sysinfo/converting-a-time-t-value-to-a-file-time
void UnixTimeToFileTime(time_t t, LPFILETIME pft)
{
   // Note that LONGLONG is a 64-bit value
   LONGLONG ll = Int32x32To64(t, 10000000) + 116444736000000000;
   pft->dwLowDateTime = (DWORD)ll;
   pft->dwHighDateTime = ll >> 32;
}

void File::SetModifiedTime(LPCTSTR filename, time_t modifiedTime)
{
   HANDLE fileHandle = CreateFile(filename,
      FILE_WRITE_ATTRIBUTES,
      FILE_SHARE_READ | FILE_SHARE_WRITE,
      NULL,
      OPEN_EXISTING,
      FILE_ATTRIBUTE_NORMAL,
      NULL);

   if (fileHandle == INVALID_HANDLE_VALUE)
   {
      DWORD error = GetLastError();
      throw SystemException(Win32::ErrorMessage(error).ToString() + filename, error, __FILE__, __LINE__);
   }

   FILETIME lastWriteTime = {};
   UnixTimeToFileTime(modifiedTime, &lastWriteTime);

   SetFileTime(fileHandle, nullptr, nullptr, &lastWriteTime);

   CloseHandle(fileHandle);
}
