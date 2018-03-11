//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file JFIFRewriter.cpp JFIF (JPEG File Interchange Format) rewriter
//
#include "stdafx.h"
#include "JFIFRewriter.hpp"
#include <ulib/stream/EndianAwareFilter.hpp>
#include <ulib/Exception.hpp>
#include <algorithm>

const BYTE c_startByte = 0xff;

void JFIFRewriter::Start()
{
   Stream::EndianAwareFilter endianFilterIn(m_streamIn);
   Stream::EndianAwareFilter endianFilterOut(m_streamOut);

   bool isStartOfStream = false;

   // read until EOF
   BYTE marker = SOI;
   while (!m_streamIn.AtEndOfStream() || marker != EOI)
   {
      BYTE startByte = m_streamIn.ReadByte();

      if (startByte != c_startByte)
         throw Exception(_T("found invalid jfif start byte"), __FILE__, __LINE__);

      marker = m_streamIn.ReadByte();

      WORD length = 0;
      switch(marker)
      {
      case SOI:
         ATLTRACE(_T("JFIF Marker: SOI\n"));
         break;

      case EOI:
         break;

      case APP0:
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: APP0, length 0x%04x\n"), length);
         break;

      case APP1:
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: APP1, length 0x%04x\n"), length);
         break;

      case DQT: // definition quantize tables
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: DQT, length 0x%04x\n"), length);
         break;

      case SOF0: // SOF 0: baseline DCT
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: SOF 0, length 0x%04x\n"), length);
         break;

      case DHT: // DHT: definition huffman tables
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: DHT, length 0x%04x\n"), length);
         break;

      case SOS:
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: SOS, length 0x%04x\n"), length);
         isStartOfStream = true;
         break;

      default:
         length = endianFilterIn.Read16BE();
         ATLTRACE(_T("JFIF Marker: 0x%02x, length 0x%04x\n"), marker, length);
         break;
      }

      if (marker == SOI || marker == EOI)
      {
         m_streamOut.WriteByte(startByte);
         m_streamOut.WriteByte(marker);
      }

      if (marker != SOI && marker != EOI)
      {
         if (isStartOfStream)
         {
            m_streamOut.WriteByte(startByte);
            m_streamOut.WriteByte(marker);

            endianFilterOut.Write16BE(length);

            // start of stream; read until 0xff 0xda arrives
            BYTE buffer[1024];
            DWORD dwRead = 0, dwWritten = 0;
            while (!m_streamIn.AtEndOfStream())
            {
               m_streamIn.Read(buffer, sizeof(buffer), dwRead);
               if (dwRead > 0)
               {
                  m_streamOut.Write(buffer, dwRead, dwWritten);
                  ATLASSERT(dwRead == dwWritten);
               }
            }

            marker = EOI;
         }
         else
         {
            if (length > 2)
               OnBlock(marker, length-2);
            else
            {
               m_streamOut.WriteByte(startByte);
               m_streamOut.WriteByte(marker);
               endianFilterOut.Write16BE(length);
            }
         }
      }
   }
}

void JFIFRewriter::OnBlock(BYTE marker, WORD length)
{
   // write block header
   m_streamOut.WriteByte(c_startByte);
   m_streamOut.WriteByte(marker);

   Stream::EndianAwareFilter endianFilterOut(m_streamOut);
   endianFilterOut.Write16BE(length+2);

   // copy over bytes
   BYTE buffer[1024];
   DWORD dwRead = 0, dwWritten = 0;
   DWORD dlength = length;

   while (!m_streamIn.AtEndOfStream() && dlength > 0)
   {
      DWORD dwSize = std::min(static_cast<DWORD>(sizeof(buffer)), dlength);

      m_streamIn.Read(buffer, dwSize, dwRead);

      dlength -= dwRead;

      if (dwRead > 0)
      {
         m_streamOut.Write(buffer, dwRead, dwWritten);
         ATLASSERT(dwRead == dwWritten);
      }
   }
}
