//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file JpegMemorySourceManager.hpp JPEG memory source manager
//
#pragma once

// includes
#include <jpeglib.h>
#include <vector>

/// source manager to read JPEG file from memory
class JpegMemorySourceManager: public jpeg_source_mgr
{
public:
   /// ctor
   JpegMemorySourceManager(const std::vector<BYTE>& vecJpegData)
      :m_vecJpegData(vecJpegData)
   {
      this->init_source = &InitSource;
      this->fill_input_buffer = &FillInputBuffer;
      this->skip_input_data = &SkipInputData;
      this->resync_to_restart = &jpeg_resync_to_restart;
      this->term_source = &TermSource;

      this->next_input_byte = &m_vecJpegData[0];
      this->bytes_in_buffer = 0;
   }

   /// inits source
   static void InitSource(j_decompress_ptr cinfo)
   {
      cinfo->src->bytes_in_buffer = 0;
   }

   /// fills input buffer
   static boolean FillInputBuffer(j_decompress_ptr cinfo)
   {
      JpegMemorySourceManager* pT = reinterpret_cast<JpegMemorySourceManager*>(cinfo->src);

      cinfo->src->next_input_byte = &pT->m_vecJpegData[0];
      cinfo->src->bytes_in_buffer = pT->m_vecJpegData.size();

      return TRUE;
   }

   /// skips input data
   static void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
   {
      if (num_bytes <= 0)
         return;

      if (static_cast<long>(cinfo->src->bytes_in_buffer) >= num_bytes)
      {
         // in-buffer skip
         cinfo->src->bytes_in_buffer -= num_bytes;
         cinfo->src->next_input_byte += num_bytes;
      }
      else
      {
         cinfo->src->bytes_in_buffer = 0;
      }
   }

   /// terminates source
   static void TermSource(j_decompress_ptr /*cinfo*/)
   {
   }

private:
   /// JPEG data
   const std::vector<BYTE>& m_vecJpegData;
};
