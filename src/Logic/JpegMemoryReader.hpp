//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file JpegMemoryReader.hpp JPEG memory reader
//
#pragma once

// includes
#include <vector>
#include "JpegMemorySourceManager.hpp"
#include "JpegDecoder.hpp"

/// JPEG image info
class JpegImageInfo
{
public:
   /// ctor
   JpegImageInfo(unsigned int uiWidth, unsigned int uiHeight, unsigned int uiPadding = 0)
      :m_uiWidth(uiWidth),
       m_uiHeight(uiHeight),
       m_uiPadding(uiPadding)
   {
   }

   unsigned int Width() const { return m_uiWidth; }       ///< returns width
   unsigned int Height() const { return m_uiHeight; }     ///< returns height

   unsigned int Padding() const { return m_uiPadding; }   ///< returns padding

private:
   unsigned int m_uiWidth;    ///< width
   unsigned int m_uiHeight;   ///< height
   unsigned int m_uiPadding;  ///< padding
};

/// JPEG memory reader
class JpegMemoryReader : public boost::noncopyable
{
public:
   /// ctor
   JpegMemoryReader(const std::vector<BYTE>& vecJpegData)
      :m_sourceManager(vecJpegData),
       m_decoder(m_sourceManager),
       m_imageInfo(0, 0)
   {
   }

   /// reads JPEG image from buffer
   void Read();

   /// returns image info
   JpegImageInfo ImageInfo() const { return m_imageInfo; };

   /// returns decoded bitmap data (RGB bytes)
   std::vector<BYTE>& BitmapData() { return m_vecBitmapData; }

   /// returns decoded bitmap data (RGB bytes); const version
   const std::vector<BYTE>& BitmapData() const { return m_vecBitmapData; }

private:
   /// called when next scanline has been decoded
   virtual void OnReadScanline(BYTE* pbData, UINT uiLength);

private:
   /// source manager to read JPG file from memory
   JpegMemorySourceManager m_sourceManager;

   /// decoder
   JpegDecoder m_decoder;

   /// image info
   JpegImageInfo m_imageInfo;

   /// decoded bitmap data (RGB bytes)
   std::vector<BYTE> m_vecBitmapData;
};
