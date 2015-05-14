//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2015 Michael Fink
//
/// \file PreviousImagesManager.cpp Previous images manager
//

// includes
#include "stdafx.h"
#include "PreviousImagesManager.hpp"
#include "JpegMemoryReader.hpp"
#include "Exif.hpp"
#include "Thread.hpp"

/// list of tags to read and store in PreviousImageInfo; must exactly
/// match the list in T_enImageInfoType
static ExifTag c_aReadTags[] =
{
   EXIF_TAG_APERTURE_VALUE,      // of type EXIF_FORMAT_RATIONAL
   EXIF_TAG_SHUTTER_SPEED_VALUE, // of type EXIF_FORMAT_SRATIONAL
   EXIF_TAG_ISO_SPEED_RATINGS,   // of type EXIF_FORMAT_SHORT
   EXIF_TAG_FOCAL_LENGTH,        // of type EXIF_FORMAT_RATIONAL
   EXIF_TAG_FLASH,               // of type EXIF_FORMAT_SHORT
   EXIF_TAG_DATE_TIME_ORIGINAL   // of type EXIF_FORMAT_ASCII
};

static_assert(sizeof(c_aReadTags) / sizeof(*c_aReadTags) == PreviousImageInfo::typeMaxValue,
   "entries in T_enImageInfoType must match size of c_aReadTags array");

PreviousImagesManager::PreviousImagesManager(size_t /*uiMaxSizeCachedImagesInBytes*/)
:m_ioService(1), // single thread
m_upDefaultWork(new boost::asio::io_service::work(m_ioService))
{
   m_upThread.reset(new std::thread(std::bind(&PreviousImagesManager::RunWorkerThread, this)));
}

PreviousImagesManager::~PreviousImagesManager() throw()
{
   try
   {
      m_upDefaultWork.reset();
      m_upThread->join();
   }
   catch (...)
   {
   }
}

bool PreviousImagesManager::ImagesAvail() const throw()
{
   LightweightMutex::LockType lock(const_cast<PreviousImagesManager*>(this)->m_mtxPreviousImagesList);

   return !m_vecPreviousImages.empty();
}

void PreviousImagesManager::AddNewImage(const CString& cszFilename)
{
   std::shared_ptr<PreviousImageInfo> spPreviousImageInfo(new PreviousImageInfo);
   spPreviousImageInfo->Filename(cszFilename);

   {
      LightweightMutex::LockType lock(m_mtxPreviousImagesList);

      m_setCurrentlyLoadingImages.insert(spPreviousImageInfo);

      m_vecPreviousImages.push_back(spPreviousImageInfo);

      m_ioService.post(
         std::bind(&PreviousImagesManager::LoadImageData, this, spPreviousImageInfo));
   }
}

void PreviousImagesManager::AsyncGetImage(T_enRequestImageType enImageType,
   std::shared_ptr<PreviousImageInfo> spReferenceImage,
   T_fnImageInfoAvail fnImageAvail)
{
   LightweightMutex::LockType lock(m_mtxPreviousImagesList);

   if (m_vecPreviousImages.empty())
      return; // no images added yet

   std::shared_ptr<PreviousImageInfo> spImageInfo = SelectImageInfo(enImageType, spReferenceImage);
   if (spImageInfo == nullptr)
      return;

   if (spImageInfo->IsLoaded())
   {
      CallAndRemoveImageAvailHandlers(spImageInfo);

      fnImageAvail(spImageInfo);
      return;
   }

   if (m_mapCallbacksOnImageAvail.find(spImageInfo) == m_mapCallbacksOnImageAvail.end())
      m_mapCallbacksOnImageAvail[spImageInfo] = std::vector<T_fnImageInfoAvail>();

   m_mapCallbacksOnImageAvail[spImageInfo].push_back(fnImageAvail);

   // check if image is already loading
   if (m_setCurrentlyLoadingImages.find(spImageInfo) != m_setCurrentlyLoadingImages.end())
      return; // already loading

   m_ioService.post(
      std::bind(&PreviousImagesManager::LoadImageData, this, spImageInfo));
}

bool PreviousImagesManager::IsFirstImage(std::shared_ptr<PreviousImageInfo> spImageInfo)
{
   LightweightMutex::LockType lock(m_mtxPreviousImagesList);

   return m_vecPreviousImages.front() == spImageInfo;
}

bool PreviousImagesManager::IsLastImage(std::shared_ptr<PreviousImageInfo> spImageInfo)
{
   LightweightMutex::LockType lock(m_mtxPreviousImagesList);

   return m_vecPreviousImages.back() == spImageInfo;
}

/// \note this function is always entered with a lock to m_mtxPreviousImagesList already held
std::shared_ptr<PreviousImageInfo> PreviousImagesManager::SelectImageInfo(T_enRequestImageType enImageType,
   std::shared_ptr<PreviousImageInfo> spReferenceImageInfo)
{
   switch (enImageType)
   {
   case imageTypeLast:
      return m_vecPreviousImages.back();
      break;

   case imageTypePrevious:
      {
         if (m_vecPreviousImages.front() == spReferenceImageInfo)
            return std::shared_ptr<PreviousImageInfo>(); // already at start

         auto iter = std::find(m_vecPreviousImages.begin(), m_vecPreviousImages.end(), spReferenceImageInfo);
         if (iter == m_vecPreviousImages.end())
            return std::shared_ptr<PreviousImageInfo>(); // not in this list!

         // at this point: iter != begin()
         --iter;
         return *iter;
      }
      break;

   case imageTypeNext:
      {
         if (m_vecPreviousImages.back() == spReferenceImageInfo)
            return false; // already at end

         auto iter = std::find(m_vecPreviousImages.begin(), m_vecPreviousImages.end(), spReferenceImageInfo);
         if (iter == m_vecPreviousImages.end())
            return false; // not in this list!

         // at this point: iter != end()
         ++iter;
         return *iter;
      }
      break;

   default:
      ATLASSERT(false);
      break;
   }

   return std::shared_ptr<PreviousImageInfo>();
}

/// \note this function is always entered with a lock to m_mtxPreviousImagesList already held
void PreviousImagesManager::CallAndRemoveImageAvailHandlers(std::shared_ptr<PreviousImageInfo> spImageInfo)
{
   if (m_mapCallbacksOnImageAvail.find(spImageInfo) == m_mapCallbacksOnImageAvail.end())
      return;

   const std::vector<T_fnImageInfoAvail>& vecFnImageInfoAvail = m_mapCallbacksOnImageAvail[spImageInfo];

   std::for_each(vecFnImageInfoAvail.begin(), vecFnImageInfoAvail.end(), [&spImageInfo](T_fnImageInfoAvail fnInfoAvail){
      fnInfoAvail(spImageInfo);
   });

   m_mapCallbacksOnImageAvail.erase(spImageInfo);
}

void PreviousImagesManager::LoadImageData(std::shared_ptr<PreviousImageInfo> spPreviousImageInfo)
{
   std::vector<BYTE> vecJpegData;

   if (!LoadFileBytes(spPreviousImageInfo->Filename(), vecJpegData))
      return;

   ReadJpegImage(spPreviousImageInfo, vecJpegData);

   AnalyzeImage(spPreviousImageInfo, vecJpegData);

   {
      LightweightMutex::LockType lock(m_mtxPreviousImagesList);
      spPreviousImageInfo->SetLoaded();

      m_setCurrentlyLoadingImages.erase(spPreviousImageInfo);

      CallAndRemoveImageAvailHandlers(spPreviousImageInfo);
   }
}

bool PreviousImagesManager::LoadFileBytes(const CString& cszFilename, std::vector<BYTE>& vecJpegData)
{
   FILE* fd = nullptr;
   if (0 != _tfopen_s(&fd, cszFilename, _T("rb")) || fd == nullptr)
      return false;

   std::shared_ptr<FILE> spFd(fd, fclose);
   //std::unique_ptr<FILE> upFd(fd, fclose);

   fseek(fd, 0, SEEK_END);
   long lLen = ftell(fd);
   fseek(fd, 0, SEEK_SET);

   vecJpegData.resize(static_cast<size_t>(lLen));

   size_t uiDataRead = fread(vecJpegData.data(), 1, lLen, fd);
   if (uiDataRead != static_cast<size_t>(lLen))
      return false; // error during loading; file just changed its size

   return true;
}

void PreviousImagesManager::ReadJpegImage(std::shared_ptr<PreviousImageInfo> spPreviousImageInfo, const std::vector<BYTE>& vecJpegData)
{
   JpegMemoryReader reader(vecJpegData);
   reader.Read();

   spPreviousImageInfo->Width(reader.ImageInfo().Width());
   spPreviousImageInfo->Height(reader.ImageInfo().Height());
   spPreviousImageInfo->BitmapData(reader.BitmapData());
}

void PreviousImagesManager::AnalyzeImage(std::shared_ptr<PreviousImageInfo> spPreviousImageInfo, const std::vector<BYTE>& vecJpegData)
{
   Exif::Data data(vecJpegData.data(), vecJpegData.size());

   if (!data.IsContentIfdAvail(EXIF_IFD_EXIF))
      return; // no EXIF IFD

   Exif::Content c = data.GetContent(EXIF_IFD_EXIF);

   for (unsigned int i = 0, iMax = sizeof(c_aReadTags) / sizeof(*c_aReadTags); i<iMax; i++)
   {
      Exif::Entry entry = c.GetEntry(c_aReadTags[i]);

      CString cszText = entry.GetValue();

      PreviousImageInfo::T_enImageInfoType enImageInfoType =
         static_cast<PreviousImageInfo::T_enImageInfoType>(i);

      spPreviousImageInfo->InfoText(enImageInfoType, cszText);
   }
}

void PreviousImagesManager::RunWorkerThread()
{
   Thread::SetName(_T("PreviousImagesManager worker thread"));

   for (;;)
   {
      try
      {
         m_ioService.run();
         return;
      }
      catch (...)
      {
      }
   }
}
