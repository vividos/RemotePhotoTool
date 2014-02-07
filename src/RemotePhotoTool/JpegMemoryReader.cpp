//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file JpegMemoryReader.cpp JPEG memory reader
//

// includes
#include "StdAfx.h"
#include "JpegMemoryReader.hpp"

#pragma comment(lib, "jpeg-8d.lib")

void JpegMemoryReader::Read()
{
   m_decoder.ReadHeader();

   // set parameters for decompression
   if (m_decoder.cinfo.num_components != 3)
      m_decoder.cinfo.out_color_space = JCS_RGB; // convert grayscale to RGB

   m_decoder.StartDecompress();

   // calculate padding
   int row_stride = m_decoder.cinfo.output_width * m_decoder.cinfo.output_components;
   unsigned int uiPadding = (row_stride & 3) == 0 ? 0 : 4 - (row_stride & 3);

   m_imageInfo = JpegImageInfo(m_decoder.cinfo.output_width, m_decoder.cinfo.output_height, uiPadding);
   ATLASSERT(m_decoder.cinfo.output_components == 3);

   std::vector<JSAMPLE> vecScanline(m_decoder.cinfo.output_width * m_decoder.cinfo.output_components);
   JSAMPROW pScanline = &vecScanline[0];

   // while (scan lines remain to be read)
   unsigned int uiScanline = 0;
   while (m_decoder.HasScanlines())
   {
      JDIMENSION dim = jpeg_read_scanlines(&m_decoder.cinfo, &pScanline, 1);
      if (dim != 1)
         break;

       OnReadScanline(pScanline, row_stride);

       uiScanline++;
   }
}

void JpegMemoryReader::OnReadScanline(BYTE* pbData, UINT uiLength)
{
   // convert from BGR to RGB
   for (UINT ui = 0; ui<uiLength; ui += 3)
      std::swap(pbData[ui], pbData[ui+2]);

   m_vecBitmapData.insert(m_vecBitmapData.end(), pbData, pbData+uiLength);

   // add padding bytes, if any
   unsigned int uiPadding = m_imageInfo.Padding();
   if (uiPadding > 0)
   {
      for (unsigned int i=0; i<uiPadding; i++)
         m_vecBitmapData.push_back(0);
   }
}
