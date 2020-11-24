//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file ImageFileManager.cpp Image file manager
//

// includes
#include "stdafx.h"
#include "ImageFileManager.hpp"
#include <ulib/Path.hpp>
#include <ulib/FileFinder.hpp>
#include <ctime>
#include <algorithm>

ImageFileManager::ImageFileManager(AppSettings& settings)
:m_settings(settings),
 m_uiNextImageIndex(1)
{
   FindLastUsedFilename();
}

CString ImageFileManager::NextFilename(T_enImageType enImageType, bool startNewSeries)
{
   time_t nowtime = time(&nowtime);
   return NextFilename(enImageType, nowtime, startNewSeries);
}

CString ImageFileManager::NextFilename(T_enImageType enImageType, time_t time, bool bStartNewSeries)
{
   CString cszPath = m_settings.m_cszProjectsFolder;

   if (!Path::FolderExists(cszPath))
      CreateDirectory(cszPath, nullptr);

   if (m_settings.m_bCurrentDateSubfolder)
      AddDate(cszPath, time);

   if (!Path::FolderExists(cszPath))
      CreateDirectory(cszPath, nullptr);

   if (m_settings.m_bImageTypeSubfolder)
      AddImageTypePath(cszPath, enImageType, bStartNewSeries);

   if (!Path::FolderExists(cszPath))
      CreateDirectory(cszPath, nullptr);

   CString cszImageFilename;
   unsigned int uiImageNr = m_uiNextImageIndex;
   do
   {
      cszImageFilename.Format(_T("IMG_%04u.jpg"), uiImageNr);
      uiImageNr++;

   } while (Path::FileExists(Path::Combine(cszPath, cszImageFilename)));

   m_uiNextImageIndex = uiImageNr-1;

   return Path::Combine(cszPath, cszImageFilename);
}

void ImageFileManager::AddDate(CString& cszPath, time_t time)
{
   struct tm tm = {0};
   ATLVERIFY(0 == localtime_s(&tm, &time));

   CString cszDate;
   _tcsftime(cszDate.GetBuffer(256), 256, _T("%Y-%m-%d"), &tm);
   cszDate.ReleaseBuffer();

   cszPath = Path::Combine(cszPath, cszDate);
}

void ImageFileManager::AddImageTypePath(CString& cszPath, T_enImageType enImageType, bool bStartNewSeries)
{
   CString cszSubfolder;

   switch (enImageType)
   {
   case imageTypeNormal:      break;
   case imageTypeHDR:         cszSubfolder = _T("hdr%u"); break;
   case imageTypePano:        cszSubfolder = _T("pano%u"); break;
   case imageTypeHDRPano:     cszSubfolder = _T("hdr-pano%u"); break;
   case imageTypeTimelapse:   cszSubfolder = _T("timelapse%u"); break;
   case imageTypePhotostack:  cszSubfolder = _T("photostack%u"); break;
   default:
      ATLASSERT(false);
      break;
   }

   if (cszSubfolder.Find(_T("%u")) != -1)
      FormatNumberedImagePath(cszPath, cszSubfolder, bStartNewSeries);

   cszPath = Path::Combine(cszPath, cszSubfolder);
}

void ImageFileManager::FormatNumberedImagePath(const CString& cszPath, CString& cszSubfolder, bool bStartNewSeries)
{
   // search for current or next folder
   CString cszSearchPathN = Path::Combine(cszPath, cszSubfolder);
   cszSearchPathN += _T("\\");

   CString cszSearchPath;
   unsigned int uiCount = 1;
   do
   {
      cszSearchPath.Format(cszSearchPathN, uiCount++);

   } while (INVALID_FILE_ATTRIBUTES != GetFileAttributes(cszSearchPath));

   uiCount--;

   // found invalid path
   if (!bStartNewSeries)
      uiCount--; // use previous, existing folder

   CString cszTemp;
   cszTemp.Format(cszSubfolder, uiCount);

   cszSubfolder = cszTemp;
}

void ImageFileManager::FindLastUsedFilename()
{
   unsigned int uiMaxImageNr = 0;

   // find max. image number in newest subfolder; when empty, look at previous subfolder
   std::vector<CString> vecAllFolders =
      FileFinder::FindAllInPath(
         Path::Combine(m_settings.m_cszProjectsFolder, _T("")),
         _T("*.*"), true, false);

   std::sort(vecAllFolders.begin(), vecAllFolders.end());

   for (std::vector<CString>::const_reverse_iterator iter = vecAllFolders.rbegin(); iter != vecAllFolders.rend(); ++iter)
   {
      std::vector<CString> vecAllImageFiles =
         FileFinder::FindAllInPath(*iter, _T("IMG_*.*"), false, true);

      if (vecAllImageFiles.empty())
         continue; // try previous subfolder

      for (size_t i=0, iMax=vecAllImageFiles.size(); i<iMax; i++)
      {
         CString cszFilename = vecAllImageFiles[i];

         // parse image name
         int iPos = cszFilename.ReverseFind(_T('\\'));
         if (iPos != -1)
            cszFilename = cszFilename.Mid(iPos+1);

         cszFilename.MakeLower();
         cszFilename.Replace(_T("img_"), _T(""));

         iPos = cszFilename.ReverseFind(_T('.'));
         cszFilename = cszFilename.Left(iPos);

         unsigned int uiImageNr = _tcstoul(cszFilename, nullptr, 10);

         uiMaxImageNr = std::max(uiImageNr, uiMaxImageNr);
      }
   }

   m_uiNextImageIndex = uiMaxImageNr + 1;
}
