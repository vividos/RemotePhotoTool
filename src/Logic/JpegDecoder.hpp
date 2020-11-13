//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file JpegDecoder.hpp JPEG decoder
//
#pragma once

#include <ulib/Exception.hpp>
#include <jpeglib.h>
#include "JpegMemorySourceManager.hpp"

/// \brief JPEG file format decoder/decompressor
/// \details uses Independent JPEG Group's jpeg library
/// \see http://www.ijg.org/
struct JpegDecoder
{
   /// ctor
   JpegDecoder(jpeg_source_mgr& sourceManager)
      :m_sourceManager(sourceManager)
   {
      cinfo.err = jpeg_std_error(&m_errorManager);
      cinfo.err->error_exit = &OnError;
      cinfo.err->output_message = &OnOutput;

      // allocate and initialize a JPEG decompression object
      jpeg_create_decompress(&cinfo);

      // specify the source of the compressed data
      cinfo.src = &m_sourceManager;
   }

   /// dtor
   ~JpegDecoder()
   {
      try
      {
         // no suspending data source
         ATLVERIFY(TRUE == jpeg_finish_decompress(&cinfo));

         // release the JPEG decompression object
         jpeg_destroy_decompress(&cinfo);
      }
      catch (...)
      {
      }
   }

   /// reads image info
   void ReadHeader()
   {
      // call jpeg_read_header() to obtain image info
      int ret = jpeg_read_header(&cinfo, TRUE);
      ATLASSERT(ret == JPEG_HEADER_OK);

      if (ret != JPEG_HEADER_OK)
         throw Exception(_T("jpeg_read_header failed"), __FILE__, __LINE__);
   }

   /// starts decompressing
   void StartDecompress()
   {
      boolean b = jpeg_start_decompress(&cinfo);
      ATLASSERT(b == TRUE); // no suspending data source

      if (b == FALSE)
         throw Exception(_T("jpeg_start_decompress failed"), __FILE__, __LINE__);
   }

   /// returns if there still are scanlines available
   bool HasScanlines() const
   {
      return cinfo.output_scanline < cinfo.output_height;
   }

private:
   /// formats error message
   static CString FormatErrorMessage(j_common_ptr cinfo)
   {
      char buffer[JMSG_LENGTH_MAX];
      cinfo->err->format_message(cinfo, buffer);

      CString cszText;
      cszText.Format(_T("jpeg error: %hs"), buffer);

      return cszText;
   }

   /// outputs error message
   static void OnOutput(j_common_ptr cinfo)
   {
      UNUSED(cinfo);
      ATLTRACE(FormatErrorMessage(cinfo));
   }

   /// throws error
   static void OnError(j_common_ptr cinfo)
   {
      throw Exception(FormatErrorMessage(cinfo), __FILE__, __LINE__);
   }

public:
   /// decompress struct
   jpeg_decompress_struct cinfo;

private:
   /// source manager
   jpeg_source_mgr& m_sourceManager;

   /// error manager
   jpeg_error_mgr m_errorManager;
};
