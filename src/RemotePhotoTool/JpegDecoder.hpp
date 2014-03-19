//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file JpegDecoder.hpp JPEG decoder
//
#pragma once

#include "Exception.hpp"
#include <jpeglib.h>
#include "JpegMemorySourceManager.hpp"

/// \brief JPEG file format decoder/decompressor
/// \details uses Independent JPEG Group's jpeg library
/// \see http://www.ijg.org/
struct JpegDecoder : public boost::noncopyable
{
   /// ctor
   JpegDecoder(jpeg_source_mgr& sourceManager) throw()
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
   ~JpegDecoder() throw()
   {
      boolean b = jpeg_finish_decompress(&cinfo);
      ATLVERIFY(b == TRUE); // no suspending data source

      // release the JPEG decompression object
      jpeg_destroy_decompress(&cinfo);
   }

   /// reads image info
   void ReadHeader()
   {
      // call jpeg_read_header() to obtain image info
      int ret = jpeg_read_header(&cinfo, TRUE);
      ATLVERIFY(ret == JPEG_HEADER_OK);
   }

   /// starts decompressing
   void StartDecompress()
   {
      boolean b = jpeg_start_decompress(&cinfo);
      ATLVERIFY(b == TRUE); // no suspending data source
   }

   /// returns if there still are scanlines available
   bool HasScanlines() const throw()
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
      cinfo; ATLTRACE(FormatErrorMessage(cinfo));
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
