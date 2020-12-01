//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file PreviousImagesManager.hpp Previous images manager
//
#pragma once

#include "PreviousImageInfo.hpp"
#include <ulib/thread/LightweightMutex.hpp>
#include "SingleThreadExecutor.hpp"
#include <map>
#include <set>
#include <memory>
#include <thread>

/// manages list of previously taken images by any of the photo modes. The previous
/// images view shows the images. The manager only offers asynchronous access to the
/// stored images, as the images may still load in the background thread.
class PreviousImagesManager
{
public:
   /// image type request for async call to get an image
   enum T_enRequestImageType
   {
      /// returns the last image in the list of previous images
      imageTypeLast = 0,

      /// returns the previous image, in relation to given reference image
      imageTypePrevious,

      /// returns the next image, in relation to given reference image
      imageTypeNext,
   };

   /// function to notify caller that image data is available
   /// \note the handler must not access PreviousImagesManager during this call,
   /// since a lock on internal data structures are held.
   typedef std::function<void(std::shared_ptr<PreviousImageInfo>)> T_fnImageInfoAvail;

   /// ctor; takes max. size of cached images
   PreviousImagesManager(size_t uiMaxSizeCachedImagesInBytes = 10 * 1024 * 1024);
   /// dtor
   ~PreviousImagesManager();

   /// returns if there are images available at all
   bool ImagesAvail() const;

   /// adds newly taken image to the end of the list
   void AddNewImage(const CString& cszFilename);

   /// asynchronously gets image; when the image is already loaded, the given function is called synchronously
   void AsyncGetImage(T_enRequestImageType enImageType, std::shared_ptr<PreviousImageInfo> spReferenceImage,
      T_fnImageInfoAvail fnImageAvail);

   /// returns if the given image is the first image in the list
   bool IsFirstImage(std::shared_ptr<PreviousImageInfo> spImageInfo);

   /// returns if the given image is the last image in the list
   bool IsLastImage(std::shared_ptr<PreviousImageInfo> spImageInfo);

private:
   /// selects an image based on image request type and reference image info
   std::shared_ptr<PreviousImageInfo> SelectImageInfo(T_enRequestImageType enImageType,
      std::shared_ptr<PreviousImageInfo> spReferenceImageInfo);

   /// calls all "image avail" handler for given image info and removes them
   void CallAndRemoveImageAvailHandlers(std::shared_ptr<PreviousImageInfo> spImageInfo);

   /// loads raw data bytes for given file
   bool LoadFileBytes(const CString& cszFilename, std::vector<BYTE>& vecJpegData);

   /// loads image data for given image info
   void LoadImageData(std::shared_ptr<PreviousImageInfo> spPreviousImageInfo);

   /// reads JPEG image and stores it in BitmapData()
   void ReadJpegImage(std::shared_ptr<PreviousImageInfo> spPreviousImageInfo, const std::vector<BYTE>& vecJpegData);

   /// analyzes image and adds more image infos
   void AnalyzeImage(std::shared_ptr<PreviousImageInfo> spImageInfo, const std::vector<BYTE>& vecJpegData);

   /// runs worker thread that asynchronously loads images
   void RunWorkerThread();

private:
   /// mutex to protect access to m_vecPreviousImages, m_mapCallbacksOnImageAvail and m_setCurrentlyLoadingImages
   LightweightMutex m_mtxPreviousImagesList;

   /// List of previously taken images
   std::vector<std::shared_ptr<PreviousImageInfo>> m_vecPreviousImages;

   /// map of callbacks to call for every image info
   std::map<std::shared_ptr<PreviousImageInfo>, std::vector<T_fnImageInfoAvail>> m_mapCallbacksOnImageAvail;

   /// set of  image infos currently being loaded
   std::set<std::shared_ptr<PreviousImageInfo>> m_setCurrentlyLoadingImages;

   /// background thread executor
   SingleThreadExecutor m_executor;
};
