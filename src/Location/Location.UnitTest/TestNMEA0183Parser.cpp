//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file TestImageTypeScanner.cpp tests ImageTypeScanner class
//

// includes
#include "stdafx.h"
#include "NMEA0183/Parser.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

/// unit tests for project Location
namespace LocationUnitTest
{
   /// tests class NMEA0183::Parser
   TEST_CLASS(TestNMEA0183Parser)
   {
   public:
      /// tests default ctor
      TEST_METHOD(TestDefaultCtor)
      {
         NMEA0183::Parser p;
      }

      /// parses single NMEA 0183 sentences
      TEST_METHOD(TestParseSingleSentences)
      {
         // set up
         LPCTSTR sentenceList[] =
         {
            _T("$PSRFTXT,WAAS Enable"),
            _T("$GPRMC,162614,A,5230.5900,N,01322.3900,E,10.0,90.0,131006,1.2,E,A*13"),
            _T("$GNGNS,014035.00,4332.69262,S,17235.48549,E,RR,13,0.9,25.63,11.24,,*70"),
            _T("$GPGNS,014035.00,,,,,,8,,,,1.0,23*76"),
            _T("$GLGNS,014035.00,,,,,,5,,,,1.0,23*67"),
         };

         // run
         for (LPCTSTR sentence : sentenceList)
         {
            NMEA0183::Parser p;
            p.ParseLine(sentence);
         }
      }

      /// parses several seconds of data
      /// \see from https://en.wikipedia.org/wiki/NMEA_0183
      TEST_METHOD(TestParseSentences)
      {
         // set up
         LPCTSTR sentenceList[] =
         {
            _T("$GPGGA,092750.000,5321.6802,N,00630.3372,W,1,8,1.03,61.7,M,55.2,M,,*76"),
            _T("$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A"),
            _T("$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70"),
            _T("$GPGSV,3,2,11,02,39,223,19,13,28,070,17,26,23,252,,04,14,186,14*79"),
            _T("$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76"),
            _T("$GPRMC,092750.000,A,5321.6802,N,00630.3372,W,0.02,31.66,280511,,,A*43"),
            _T("$GPGGA,092751.000,5321.6802,N,00630.3371,W,1,8,1.03,61.7,M,55.3,M,,*75"),
            _T("$GPGSA,A,3,10,07,05,02,29,04,08,13,,,,,1.72,1.03,1.38*0A"),
            _T("$GPGSV,3,1,11,10,63,137,17,07,61,098,15,05,59,290,20,08,54,157,30*70"),
            _T("$GPGSV,3,2,11,02,39,223,16,13,28,070,17,26,23,252,,04,14,186,15*77"),
            _T("$GPGSV,3,3,11,29,09,301,24,16,09,020,,36,,,*76"),
            _T("$GPRMC,092751.000,A,5321.6802,N,00630.3371,W,0.06,31.66,280511,,,A*45"),
         };

         NMEA0183::Parser p;

         // run
         for (LPCTSTR sentence : sentenceList)
         {
            p.ParseLine(sentence);
         }

         // check
      }
   };
}
