//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file MoveScriptGenerator.cpp Generates move cmd script based on image types
//

// includes
#include "stdafx.h"
#include "ImageTypeScanner.hpp"
#include "CommandLineParser.hpp"
#include "Path.hpp"
#include "FileFinder.hpp"
#include "Exif.hpp"
#include <map>

/// main application for generator
class App
{
public:
   /// constructs a new application, using command line arguments
   App(int argc, _TCHAR* argv[])
      :m_imageFileInfos(CollectImageFileInfos(argc, argv))
   {
   }

   /// runs generator
   void Run()
   {
      SortImagesByDateTaken();

      ImageTypeScannerOptions options;
      ImageTypeScanner scanner(options);

      scanner.ScanImages(m_imageFileInfos, m_allImageTypeFileLists);

      PrintScript();
   }

private:
   /// collects image file infos from passed command line arguments
   static std::vector<ImageFileInfo> CollectImageFileInfos(int argc, _TCHAR* argv[])
   {
      std::vector<ImageFileInfo> allImages;

      CommandLineParser parser(argc, argv);

      CString filename;
      parser.GetNext(filename); // ignore first argument, the executable name

      while (parser.GetNext(filename))
      {
         if (Path(filename).FileExists())
            AddImageFile(allImages, filename);
         else
            if (Path(filename).FolderExists())
            {
               FileFinder finder(filename, _T("*.*"));

               if (finder.IsValid())
               {
                  while (finder.Next())
                     if (!finder.IsDot() && finder.IsFile())
                     AddImageFile(allImages, finder.Filename());
               }
            }
      }

      return allImages;
   }

   /// adds a new image file info to the list, for given image file
   static void AddImageFile(std::vector<ImageFileInfo>& allImages, const CString& filename)
   {
      _ftprintf(stderr, _T("Reading image infos for \"%s\"\n"), filename.GetString());

      ImageFileInfo info(filename);

      if (ReadImageInfos(info))
         allImages.push_back(info);
   }

   /// reads image infos for image with filename (already stored in info)
   /// \see http://www.sno.phy.queensu.ca/~phil/exiftool/TagNames/Canon.html
   static bool ReadImageInfos(ImageFileInfo& info)
   {
      Exif::Loader loader;
      loader.LoadFile(info.Filename());

      if (!loader.IsValid())
         return false;

      Exif::Data data = loader.GetData();

      if (!data.IsContentIfdAvail(EXIF_IFD_EXIF))
         return false; // no EXIF IFD

      Exif::Content exif = data.GetContent(EXIF_IFD_EXIF);

      bool autoBracketMode = exif.GetEntry(EXIF_TAG_EXPOSURE_MODE).GetShort() == 2;
      info.AutoBracketMode(autoBracketMode);

      Exif::Entry exposureCompEntry = exif.GetEntry(EXIF_TAG_EXPOSURE_BIAS_VALUE);
      ExifSRational exposureCompValue = exposureCompEntry.GetSRational(0);
      double exposureCompApexValue = double(exposureCompValue.numerator) / exposureCompValue.denominator;
      info.ExposureComp(exposureCompApexValue);

      Exif::Entry apertureEntry = exif.GetEntry(EXIF_TAG_APERTURE_VALUE);
      ExifRational apertureValue = apertureEntry.GetRational(0);
      double apertureApexValue = double(apertureValue.numerator) / apertureValue.denominator;
      unsigned int apertureTenth = (unsigned int)(std::pow(2.0, apertureApexValue / 2.0) * 10.0);
      info.Aperture(apertureTenth / 10.0);

      Exif::Entry shutterSpeedEntry = exif.GetEntry(EXIF_TAG_SHUTTER_SPEED_VALUE);
      ExifSRational shutterSpeedValue = shutterSpeedEntry.GetSRational(0);
      double shutterSpeedApexValue = double(shutterSpeedValue.numerator) / shutterSpeedValue.denominator;
      double shutterSpeedInSeconds = 1.0 / std::pow(2.0, shutterSpeedApexValue);
      info.ShutterSpeed(shutterSpeedInSeconds);

      Exif::Entry isoSpeedEntry = exif.GetEntry(EXIF_TAG_ISO_SPEED_RATINGS);
      info.IsoSpeed(isoSpeedEntry.GetShort());

      Exif::Entry focalLengthEntry = exif.GetEntry(EXIF_TAG_FOCAL_LENGTH);
      ExifRational focalLengthValue = focalLengthEntry.GetRational(0);
      double focalLength = double(focalLengthValue.numerator) / focalLengthValue.denominator;
      info.FocalLength(focalLength);

      // parse date/time, format: "2007:02:17 11:00:58"
      Exif::Entry dateTimeEntry = exif.GetEntry(EXIF_TAG_DATE_TIME_ORIGINAL);
      CString dateTimeText = dateTimeEntry.GetAscii();

      ATL::CTime dateTime(
         static_cast<unsigned int>(_tcstoul(dateTimeText.Left(4), NULL, 10)),
         static_cast<unsigned int>(_tcstoul(dateTimeText.Mid(5, 2), NULL, 10)),
         static_cast<unsigned int>(_tcstoul(dateTimeText.Mid(8, 2), NULL, 10)),
         static_cast<unsigned int>(_tcstoul(dateTimeText.Mid(11, 2), NULL, 10)),
         static_cast<unsigned int>(_tcstoul(dateTimeText.Mid(14, 2), NULL, 10)),
         static_cast<unsigned int>(_tcstoul(dateTimeText.Mid(17, 2), NULL, 10)));

      info.ImageDateStart(dateTime);

      // shutter speeds lower than 1 are just set to 0
      ATL::CTime dateTimeEnd = dateTime + ATL::CTimeSpan(0, 0, 0, static_cast<int>(info.ShutterSpeed()));
      info.ImageDateEnd(dateTimeEnd);

      // orientation stored in different content object
      Exif::Content ifd0 = data.GetContent(EXIF_IFD_0);

      Exif::Entry orientationEntry = ifd0.GetEntry(EXIF_TAG_ORIENTATION);
      info.Orientation(orientationEntry.IsValid() ? orientationEntry.GetShort() : 0);

      return true;
   }

   /// sorts image by date taken
   void SortImagesByDateTaken()
   {
      std::stable_sort(m_imageFileInfos.begin(), m_imageFileInfos.end());
   }

   /// prints move command script by iterating through all result entries
   void PrintScript()
   {
      std::map<T_enImageType, unsigned int> mapImageTypeToCurrentCount = {
         { T_enImageType::imageTypeNormal, 1 },
         { T_enImageType::imageTypeHDR, 1 },
         { T_enImageType::imageTypePano, 1 },
         { T_enImageType::imageTypeHDRPano, 1 } };

      std::for_each(m_allImageTypeFileLists.begin(), m_allImageTypeFileLists.end(),
         [&](const ImageTypeFilesList& filesList)
         {
            if (filesList.ImageFileInfoList().empty())
               return;

            CString baseFolder = Path(filesList.ImageFileInfoList()[0].Filename()).DirectoryName();

            // image type must be in map
            ATLASSERT(mapImageTypeToCurrentCount.find(filesList.ImageType()) != mapImageTypeToCurrentCount.end());

            unsigned int currentCount = mapImageTypeToCurrentCount[filesList.ImageType()];
            switch (filesList.ImageType())
            {
            case T_enImageType::imageTypeNormal: break;
            case T_enImageType::imageTypeHDR: baseFolder += _T("hdr"); break;
            case T_enImageType::imageTypePano: baseFolder.AppendFormat(_T("pano%i"), currentCount); break;
            case T_enImageType::imageTypeHDRPano: baseFolder.AppendFormat(_T("hdr-pano%i"), currentCount); break;
            default:
               ATLASSERT(false);
               break;
            };

            if (filesList.ImageType() != T_enImageType::imageTypeNormal &&
               filesList.ImageType() != T_enImageType::imageTypeHDR)
            {
               mapImageTypeToCurrentCount[filesList.ImageType()]++;
            }

            if (filesList.ImageType() == T_enImageType::imageTypeNormal)
               return; // don't move normal images

            PrintMoveCommands(filesList, baseFolder);
         });
   }

   /// prints move commands for single entry consisting of multiple files
   void PrintMoveCommands(const ImageTypeFilesList& filesList, const CString& baseFolder)
   {
      _ftprintf(stdout, _T("mkdir \"%s\" 2> nul\n"), baseFolder.GetString());

      std::for_each(filesList.ImageFileInfoList().begin(), filesList.ImageFileInfoList().end(),
         [&](const ImageFileInfo& info) {

         CString destFilename =
            Path::Combine(
               baseFolder,
               Path(info.Filename()).FilenameAndExt());

         _ftprintf(stdout, _T("move \"%s\" \"%s\"\n"),
            info.Filename().GetString(),
            destFilename.GetString());
      });
   }

private:
   /// list of image file infos to examine
   std::vector<ImageFileInfo> m_imageFileInfos;

   /// results of image scanning
   std::vector<ImageTypeFilesList> m_allImageTypeFileLists;
};

/// main function
int _tmain(int argc, _TCHAR* argv[])
{
   try
   {
      _ftprintf(stderr, _T("MoveScriptGenerator - Generates move cmd script based on image types\n\n"));

      App app(argc, argv);
      app.Run();

      return 0;
   }
   catch (...)
   {
      // exception while running
      _ftprintf(stderr, _T("Exception while running MoveScriptGenerator\n"));
      return -1;
   }
}
