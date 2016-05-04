//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PsrecCameraEventData.hpp PS-ReC - Camera event data
//
#pragma once

// includes
#include "PsrecVarDataParser.hpp"

namespace PSREC
{
   /// camera event data
   struct CameraEventData : public VarDataParser
   {
   public:
      /// ctor; parses event data
      CameraEventData(prVoid* pEventData)
      {
         BYTE* pbData = reinterpret_cast<BYTE*>(pEventData);

         std::vector<BYTE>& vecBuffer = GetBuffer();
         vecBuffer.assign(pbData, pbData + 12);// at least 12 bytes

                                               // read length of event data
         prUInt32 uiLength = ReadUint32();

         vecBuffer.assign(pbData, pbData + uiLength);

         unsigned int uiNumParams = (uiLength - 12) / 4;
         Parse(uiNumParams);
      }

      /// parses event data
      void Parse(unsigned int uiNumParam)
      {
         // note that buffer pointer is at pos 4 already
         prUInt16 uiContainerType = ReadUint16();
         m_uiEventCode = ReadUint16();
         prUInt32 uiTransactionId = ReadUint32();

         uiContainerType; uiTransactionId; // unused

                                           // read all parameters
         for (unsigned int ui = 0; ui < uiNumParam; ui++)
            m_vecParams.push_back(ReadUint32());
      }

      /// event code
      prUInt16 m_uiEventCode;

      /// event parameters (optional)
      std::vector<prUInt32> m_vecParams;
   };

} // namespace PSREC
