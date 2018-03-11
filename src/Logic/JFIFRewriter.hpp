//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file JFIFRewriter.hpp JFIF (JPEG File Interchange Format) rewriter
//
#pragma once

// includes
#include <ulib/stream/IStream.hpp>

/// \brief JFIF rewriter
/// Loads JFIF data stream (used internally by JPEG images) and calls OnBlock() when a new block
/// arrives. Derived classes can then choose to re-write that block, e.g. EXIF data.
class JFIFRewriter
{
public:
   /// ctor
   JFIFRewriter(Stream::IStream& streamIn, Stream::IStream& streamOut)
      :m_streamIn(streamIn), m_streamOut(streamOut)
   {
   }

   /// starts rewriting process
   void Start();

   /// JFIF block marker
   enum T_JFIFBlockMarker
   {
      SOI  = 0xd8,
      EOI  = 0xd9,
      APP0 = 0xe0,
      APP1 = 0xe1, ///< Exif data is store in this block
      DQT  = 0xdb,
      SOF0 = 0xc0,
      DHT  = 0xc4,
      SOS  = 0xda,
   };

protected:
   /// called when the next JFIF block is starting
   virtual void OnBlock(BYTE marker, WORD length);

protected:
   Stream::IStream& m_streamIn;  ///< input stream
   Stream::IStream& m_streamOut; ///< output stream
};
