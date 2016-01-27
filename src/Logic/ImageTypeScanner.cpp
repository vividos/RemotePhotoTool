//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageTypeScanner.cpp Scanner for image types
//

/// includes
#include "stdafx.h"
#include "ImageTypeScanner.hpp"
#include <set>
#include <iterator>

/// \details Scans given list of image file infos for different types of image (sets) and produces
/// zero, one or more entries of type ImageTypeFilesList, depending on found sets. If there's no
/// images, no entries are returned; if there are at least some unrecognized image sets, they are
/// put in a first ImageTypeFilesList with type 'normal'. For HDR images, only one
/// ImageTypeFilesList is returned, containing all HDR images. For Panorama and HDR Panorama image
/// sets, a separate ImageTypeFilesList entry is returned.
/// \param[in,out] imageFileList list of image file infos to examine
/// \param[out] imageTypeFilesList list of entries of sets of found images
void ImageTypeScanner::ScanImages(std::vector<ImageFileInfo>& imageFileList,
   std::vector<ImageTypeFilesList>& imageTypeFilesList)
{
   ImageTypeFilesList hdrFilesList(T_enImageType::imageTypeHDR);
   CollectHDRImages(imageFileList, hdrFilesList);

   CollectPanoramaImages(imageFileList, imageTypeFilesList, false);

   if (!hdrFilesList.ImageFileInfoList().empty())
   {
      CollectPanoramaImages(hdrFilesList.ImageFileInfoList(), imageTypeFilesList, true);

      if (!hdrFilesList.ImageFileInfoList().empty())
         imageTypeFilesList.push_back(hdrFilesList);
   }

   if (!imageFileList.empty())
   {
      ImageTypeFilesList normalFilesList(T_enImageType::imageTypeNormal);
      normalFilesList.ImageFileInfoList().assign(imageFileList.begin(), imageFileList.end());

      imageTypeFilesList.insert(imageTypeFilesList.begin(), normalFilesList);

      imageFileList.clear();
   }
}

/// \details Collects all HDR images in one big list that may be analyzed further.
/// HDR images have the following properties:
/// - same AEB property, but different AEB exposure compensation values
/// - same orientation
/// OR have:
/// - same focal length
/// - same ISO speed
/// - consecutive images have a double or half shutter speed value
/// - image date start is less than x seconds of image date end of previous image
/// - same orientation
/// x is specified in the image scanner options
/// \param[in,out] imageFileList list of image infos to examine for sets of HDR images; the list
/// is reduced by the found HDR images
/// \param[out] hdrFilesList collection of found HDR images (may be none)
void ImageTypeScanner::CollectHDRImages(std::vector<ImageFileInfo>& imageFileList, ImageTypeFilesList& hdrFilesList)
{
   if (imageFileList.empty())
      return;

   for (size_t index = 0, maxIndex = imageFileList.size(); index < maxIndex; )
   {
      const ImageFileInfo& currentInfo = imageFileList[index];

      size_t lastImageIndex = (size_t)-1;

      bool isAEBImage = currentInfo.AutoBracketMode();

      if (isAEBImage)
         FindAEBHDRImages(imageFileList, index, lastImageIndex);
      else
         FindNonAEBHDRImages(imageFileList, index, lastImageIndex);

      if (lastImageIndex != (size_t)-1)
      {
         size_t length = lastImageIndex - index + 1;

         ATLASSERT(length > 0); // must not occur
         if (length == 0)
         {
            index++;
            continue;
         }

         imageFileList[index].StartHDRImage(true);

         MoveImages(imageFileList, index, length, hdrFilesList.ImageFileInfoList());

         // don't adjust index
         maxIndex -= length;
      }
      else
         index++;
   }
}

/// \details searches for more HDR images that belong in the same set of HDR images, marked by the
/// AEB flag embedded in each image.
/// \param[in] imageFileList image file list to search for more HDR images
/// \param[in] startIndex index to start of first image of set of possible HDR images
/// \param[out] lastImageIndex last index of image in imageFileList that is in this set of HDR
/// images; when this is not set by the function, there was no recognized set of HDR images
void ImageTypeScanner::FindAEBHDRImages(const std::vector<ImageFileInfo>& imageFileList, size_t startIndex, size_t& lastImageIndex)
{
   std::set<double> allCurrentExposureCompValues;
   allCurrentExposureCompValues.insert(imageFileList[startIndex].ExposureComp());

   unsigned int orientationStart = imageFileList[startIndex].Orientation();

   // search for last image
   for (size_t searchIndex = startIndex + 1, maxIndex = imageFileList.size(); searchIndex < maxIndex; searchIndex++)
   {
      const ImageFileInfo& searchInfo = imageFileList[searchIndex];

      if (!searchInfo.AutoBracketMode())
         break; // non-AEB image

      double exposureCompValue = searchInfo.ExposureComp();

      // already stored? then it's the start of a new image
      if (allCurrentExposureCompValues.find(exposureCompValue) !=
         allCurrentExposureCompValues.end())
         break; // next AEB image series

      allCurrentExposureCompValues.insert(exposureCompValue);

      bool isSameOrientation =
         searchInfo.Orientation() == orientationStart;

      if (!isSameOrientation)
         break; // changed orientation

      lastImageIndex = searchIndex;
   }
}

void ImageTypeScanner::FindNonAEBHDRImages(const std::vector<ImageFileInfo>& imageFileList, size_t startIndex, size_t& lastImageIndex)
{
   std::set<double> allCurrentShutterSpeedValues;
   allCurrentShutterSpeedValues.insert(imageFileList[startIndex].ShutterSpeed());

   // search for last image
   for (size_t searchIndex = startIndex + 1, maxIndex = imageFileList.size(); searchIndex < maxIndex; searchIndex++)
   {
      const ImageFileInfo& previousInfo = imageFileList[searchIndex - 1];
      const ImageFileInfo& searchInfo = imageFileList[searchIndex];

      if (searchInfo.AutoBracketMode())
         break; // start of AEB image series

      bool isNonAEBHDRImage = CheckNonBracketHDRImage(searchInfo, previousInfo);

      if (!isNonAEBHDRImage)
         break; // non-bracketed image

      double shutterSpeedValue = searchInfo.ShutterSpeed();

      // already stored? then it's the start of a new image
      if (allCurrentShutterSpeedValues.find(shutterSpeedValue) !=
         allCurrentShutterSpeedValues.end())
         break; // next bracketed image series

      allCurrentShutterSpeedValues.insert(shutterSpeedValue);

      lastImageIndex = searchIndex;
   }
}

bool ImageTypeScanner::CheckNonBracketHDRImage(const ImageFileInfo& currentInfo, const ImageFileInfo& previousInfo)
{
   double focalLengthDifference = currentInfo.FocalLength() - previousInfo.FocalLength();
   bool isSameFocalLength = fabs(focalLengthDifference) < 1e-6;

   bool isSameIsoSpeed = currentInfo.IsoSpeed() == previousInfo.IsoSpeed();

   double currentShutterSpeed = currentInfo.ShutterSpeed();
   double previousShutterSpeed = previousInfo.ShutterSpeed();
   double shutterSpeedRatio = currentShutterSpeed / previousShutterSpeed;

   bool isShutterSpeedDoubleOrHalf =
      (fabs(shutterSpeedRatio - 0.5) < 0.2) ||
      (fabs(shutterSpeedRatio - 2.0) < 0.1);

   bool isSameOrientation =
      currentInfo.Orientation() == previousInfo.Orientation();

   ATL::CTime lastEndTime = previousInfo.ImageDateEnd();
   lastEndTime += ATL::CTimeSpan(0, 0, 0, m_options.SecondsBetweenHDRImages());

   bool isInTimeLimit = currentInfo.ImageDateStart() < lastEndTime;

   return isSameFocalLength && isSameIsoSpeed && isShutterSpeedDoubleOrHalf && isSameOrientation && isInTimeLimit;
}

void ImageTypeScanner::CollectPanoramaImages(
   std::vector<ImageFileInfo>& imageFileList,
   std::vector<ImageTypeFilesList>& imageTypeFilesList,
   bool onlyHDRImages)
{
   if (imageFileList.empty())
      return;

   for (size_t index = 0, maxIndex = imageFileList.size(); index < maxIndex; )
   {
      size_t lastImageIndex = (size_t)-1;

      FindPanoramaImages(imageFileList, index, lastImageIndex, onlyHDRImages);

      if (lastImageIndex == (size_t)-1)
      {
         index++;
         continue;
      }

      size_t length = lastImageIndex - index + 1;

      ATLASSERT(length > 0); // must not occur
      if (length == 0)
      {
         index++;
         continue;
      }

      if (length < m_options.MinimumNumberOfPanoramaImages())
      {
         index += length;
         continue;
      }

      ImageTypeFilesList panoFilesList(onlyHDRImages ? T_enImageType::imageTypeHDRPano : T_enImageType::imageTypePano);

      MoveImages(imageFileList, index, length, panoFilesList.ImageFileInfoList());

      imageTypeFilesList.push_back(panoFilesList);

      // don't adjust index
      maxIndex -= length;
   }
}

static void MoveToNextImage(const std::vector<ImageFileInfo>& imageFileList, size_t& index, bool onlyHDRImages)
{
   index++;

   if (!onlyHDRImages)
      return;

   // forward to next HDR image, if any images still left
   for (size_t maxIndex = imageFileList.size(); index < maxIndex; index++)
   {
      if (imageFileList[index].StartHDRImage())
         break;
   }
}

/// \details searches for more panorama images that belong in the same set of panorama images.
/// Panorama images have the following properties:
/// - same focal length
/// - same ISO speed
/// - image date start is less than x seconds of image date end of previous image
/// - same orientation
/// - when an AEB image, must have the same exposure compensation value
/// \param[in] imageFileList image file list to search for more panorama images
/// \param[in] startIndex index to start of first image of set of possible panorama images
/// \param[out] lastImageIndex last index of image in imageFileList that is in this set of panorama
/// images; when this is not set by the function, there was no recognized set of panorama images.
/// \param[in] onlyHDRImages when set to true, only the images marked as "start of HDR image set"
/// are examined; these are determined by checking ImageFileInfo::StartHDRImage().
void ImageTypeScanner::FindPanoramaImages(const std::vector<ImageFileInfo>& imageFileList,
   size_t startIndex, size_t& lastImageIndex, bool onlyHDRImages)
{
   ATL::CTimeSpan spanBetweenImages(0, 0, 0, m_options.SecondsBetweenPanoramaImages());

   size_t searchIndex = startIndex;

   MoveToNextImage(imageFileList, searchIndex, onlyHDRImages);

   // search for last image
   for (size_t maxIndex = imageFileList.size(); searchIndex < maxIndex; )
   {
      const ImageFileInfo& previousInfo = imageFileList[searchIndex - 1];
      const ImageFileInfo& searchInfo = imageFileList[searchIndex];

      // note: in the HDR case, we check the direclty previously taken image, not the
      // image referenced in previousInfo, since that is the previous HDR start image
      bool isInTimeLimit =
         searchInfo.ImageDateStart() - previousInfo.ImageDateEnd() <= spanBetweenImages;

      bool isSameFocalLength =
         fabs(searchInfo.FocalLength() - previousInfo.FocalLength()) < 0.1;

      bool isSameIsoSpeed =
         searchInfo.IsoSpeed() == previousInfo.IsoSpeed();

      bool isSameOrientation =
         searchInfo.Orientation() == previousInfo.Orientation();

      bool isPanoramaImage = isInTimeLimit && isSameFocalLength && isSameIsoSpeed && isSameOrientation;

      if (!isPanoramaImage)
         break;

      MoveToNextImage(imageFileList, searchIndex, onlyHDRImages);

      lastImageIndex = searchIndex - 1;
   }
}

void ImageTypeScanner::MoveImages(std::vector<ImageFileInfo>& sourceImageFileList, size_t startIndex, size_t length, std::vector<ImageFileInfo>& destImageFileList)
{
   auto sourceIter = sourceImageFileList.begin() + startIndex;

   std::copy_n(sourceIter, length, std::back_insert_iterator<std::vector<ImageFileInfo>>(destImageFileList));

   sourceImageFileList.erase(sourceIter, sourceIter + length);
}
