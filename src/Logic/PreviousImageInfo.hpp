//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PreviousImageInfo.hpp Infos about a previously taken image
//
#pragma once

// includes
#include <array>
#include <vector>
#include <atomic>

/// Contains informations about a previously taken image, stored and managed
/// by the PreviousImagesManager class.
class PreviousImageInfo
{
public:
   /// image info text type; used in InfoText() as index
   /// \note the index must start at 0 and use contiguous indices
   enum T_enImageInfoType
   {
      typeAperture = 0, ///< aperture setting used for image
      typeShutterSpeed, ///< shutter speed
      typeIsoSetting,   ///< ISO setting
      typeFocalLength,  ///< focal length
      typeFlashFired,   ///< indicates if flash did fire
      typeDateTime,     ///< date/time of image taken

      typeMaxValue   ///< max. value, used for array length; must always be last!
   };

   /// ctor; constructs an empty image info object
   PreviousImageInfo()
      :m_uiWidth(0),
      m_uiHeight(0)
   {
   }

   // get methods

   /// returns filename of image
   const CString& Filename() const throw() { return m_cszFilename; }

   /// returns width of image
   unsigned int Width() const throw() { return m_uiWidth; }

   /// returns height of image
   unsigned int Height() const throw() { return m_uiHeight; }

   /// returns RGB bitmap data
   const std::vector<BYTE>& BitmapData() const throw() { return m_vecBitmapData; }

   /// returns one of the info texts for this image
   CString InfoText(T_enImageInfoType enImageInfoType) const throw()
   {
      ATLASSERT(enImageInfoType < PreviousImageInfo::typeMaxValue);

      return m_aInfoTextList[enImageInfoType];
   }

   /// returns if the image data is already loaded (BitmapData() and InfoText())
   bool IsLoaded() const throw() { return m_bIsLoaded; }

   // set methods

   /// sets filename of image
   void Filename(const CString& cszFilename) throw() { m_cszFilename = cszFilename; }

   /// sets width of image
   void Width(unsigned int uiWidth) throw(){ m_uiWidth = uiWidth; }

   /// sets height of image
   void Height(unsigned int uiHeight) throw() { m_uiHeight = uiHeight; }

   /// sets bitmap data
   void BitmapData(std::vector<BYTE>& vecBitmapData) throw() { m_vecBitmapData = vecBitmapData; }

   /// sets an info text for the image
   void InfoText(T_enImageInfoType enImageInfoType, const CString& cszText) throw()
   {
      ATLASSERT(enImageInfoType < PreviousImageInfo::typeMaxValue);

      m_aInfoTextList[enImageInfoType] = cszText;
   }

protected:
   friend class PreviousImagesManager;

   /// sets the flag that all image data is loaded
   void SetLoaded() throw() { m_bIsLoaded = true; }

private:
   /// filename of stored file
   CString m_cszFilename;

   /// width of image
   unsigned int m_uiWidth;

   /// height of image
   unsigned int m_uiHeight;

   /// actual RGB bitmap data
   std::vector<BYTE> m_vecBitmapData;

   /// additional info texts
   std::array<CString, typeMaxValue> m_aInfoTextList;

   /// flat to determine if all infos of image are already loaded
   std::atomic<bool> m_bIsLoaded;
};
