//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2014 Michael Fink
//
/// \file PsrecVarDataParser.hpp PS-ReC - variable data types parser
//
#pragma once

namespace PSREC
{

/// parser for variable data types data stream that is returned by some PSREC calls
struct VarDataParser
{
   /// ctor
   VarDataParser() throw()
      :m_uiPos(0)
   {
   }

   /// returns buffer
   std::vector<BYTE>& GetBuffer() { return m_vecBuffer; }

   /// reads unsigned 8-bit value
   prUInt8 ReadUint8()
   {
      if (m_uiPos + 1 > m_vecBuffer.size())
         throw Exception(_T("couldn't read Uint8 from buffer"), __FILE__, __LINE__);

      return m_vecBuffer[m_uiPos++];
   }

   /// reads unsigned 16-bit value
   prUInt16 ReadUint16()
   {
      if (m_uiPos + 2 > m_vecBuffer.size())
         throw Exception(_T("couldn't read Uint16 from buffer"), __FILE__, __LINE__);

      prUInt16 usRet = prUInt16(m_vecBuffer[m_uiPos]) |
         (prUInt16(m_vecBuffer[m_uiPos+1]) << 8);
      m_uiPos += 2;
      return usRet;
   }

   /// reads unsigned 32-bit value
   prUInt32 ReadUint32()
   {
      if (m_uiPos + 4 > m_vecBuffer.size())
         throw Exception(_T("couldn't read Uint32 from buffer"), __FILE__, __LINE__);

      prUInt32 dwRet = prUInt32(m_vecBuffer[m_uiPos]) |
         (prUInt32(m_vecBuffer[m_uiPos+1]) << 8) |
         (prUInt32(m_vecBuffer[m_uiPos+2]) << 16) |
         (prUInt32(m_vecBuffer[m_uiPos+3]) << 24);

      m_uiPos += 4;

      return dwRet;
   }

   /// reads unsigned 8-bit array
   std::vector<prUInt8> ReadUint8Array()
   {
      // read len
      prUInt32 uiLen = ReadUint32();

      std::vector<prUInt8> vecData;
      for (prUInt32 ui = 0; ui<uiLen; ui++)
         vecData.push_back(ReadUint8());

      return vecData;
   }

   /// reads unsigned 16-bit array
   std::vector<prUInt16> ReadUint16Array()
   {
      // read len
      prUInt32 uiLen = ReadUint32();

      std::vector<prUInt16> vecData;
      for (prUInt8 ui = 0; ui<uiLen; ui++)
         vecData.push_back(ReadUint16());

      return vecData;
   }

   /// reads unsigned 32-bit array
   std::vector<unsigned int> ReadUint32Array()
   {
      // read len
      prUInt32 uiLen = ReadUint32();

      std::vector<unsigned int> vecData;
      for (prUInt32 ui = 0; ui<uiLen; ui++)
         vecData.push_back(ReadUint32());

      return vecData;
   }

   /// reads string
   CString ReadString()
   {
      if (m_uiPos + 2 > m_vecBuffer.size())
         throw Exception(_T("couldn't read String from buffer"), __FILE__, __LINE__);

      BYTE bLen = m_vecBuffer[m_uiPos];

      if (m_uiPos + 1 + bLen > m_vecBuffer.size())
         throw Exception(_T("couldn't read String from buffer"), __FILE__, __LINE__);

      LPCWSTR pszBuffer = reinterpret_cast<LPCWSTR>(&m_vecBuffer[m_uiPos+1]);

      m_uiPos += 1 + bLen * sizeof(WCHAR);

      if (bLen == 0)
         return CString();

      if (pszBuffer[bLen-1] == 0)
         return CString(pszBuffer); // string as terminating null
      else
         return CString(pszBuffer, bLen);
   }

   /// reads unsigned 8-bit value as set
   void ReadUint16ArrayAsSet(std::set<prUInt16>& setValues)
   {
      prUInt32 dwLen = ReadUint32();
      for (prUInt32 dw=0; dw<dwLen; dw++)
         setValues.insert(ReadUint16());
   }

private:
   /// buffer
   std::vector<BYTE> m_vecBuffer;

   /// current pos in buffer
   size_t m_uiPos;
};

} // namespace PSREC
