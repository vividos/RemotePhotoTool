//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2023 Michael Fink
//
/// \file DropFilesHelper.hpp helper class for DROPFILES data
//
#pragma once

/// \brief helper class that produces a STGMEDIUM that contains CF_HDROP data
/// using the DROPFILES structure and a filename buffer. The code was adapted
/// from this article:
/// \see https://www.codeproject.com/articles/14482/wtl-for-mfc-programmers-part-x-implementing-a-drag
class DropFilesHelper
{
public:
   /// adds filename to the filename buffer
   void AddFilename(const CString& filename)
   {
      const BYTE* start = reinterpret_cast<const BYTE*>(filename.GetString());
      const BYTE* end = reinterpret_cast<const BYTE*>(filename.GetString() + filename.GetLength() + 1);

      m_filenameBuffer.insert(
         m_filenameBuffer.end(),
         start,
         end);
   }

   /// <summary>
   /// Closes the filename buffer, allocates a DROPFILES structure and sets
   /// the FORMATETC and STGMEDIUM structures usable for an
   /// IDataObject::SetData() call. When the data is not used, the HGLOBAL in
   /// stg.hGlobal must be freed manually
   /// </summary>
   /// <param name="fetc">FORMATETC struct that is filled with the format</param>
   /// <param name="stg">STGMEDIUM struct containing the global buffer</param>
   /// <returns></returns>
   bool GetDropFilesData(FORMATETC& fetc, STGMEDIUM& stg)
   {
      fetc = { CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL };
      stg = { TYMED_HGLOBAL };

      // Add one more TCHAR for the final null char.
      m_filenameBuffer.push_back(0);
#ifdef _UNICODE
      m_filenameBuffer.push_back(0);
#endif

      // Alloc a buffer to hold the DROPFILES data.
      HGLOBAL hgl = GlobalAlloc(GMEM_MOVEABLE | GMEM_ZEROINIT,
         sizeof(DROPFILES) + m_filenameBuffer.size());

      if (NULL == hgl)
         return false;

      DROPFILES* pDrop = (DROPFILES*)GlobalLock(hgl);

      if (NULL == pDrop)
      {
         GlobalFree(hgl);
         return false;
      }

      pDrop->pFiles = sizeof(DROPFILES);

#ifdef _UNICODE
      pDrop->fWide = 1;
#endif

      // Copy the filenames into the buffer.
      void* filenameStorage = (void*)(pDrop + 1);
      memcpy(filenameStorage, m_filenameBuffer.data(), m_filenameBuffer.size());

      GlobalUnlock(hgl);
      stg.hGlobal = hgl;

      return true;
   }

private:
   /// buffer containing all filenames, 0-delimited
   std::vector<BYTE> m_filenameBuffer;
};
