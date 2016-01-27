//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file ImageFileInfo.hpp Infos about an image file
//
#pragma once

/// includes
#include <atltime.h>

/// contains infos about an image file
class ImageFileInfo
{
public:
   /// ctor
   ImageFileInfo(const CString& filename)
      :m_filename(filename),
      m_autoBracketMode(false),
      m_startHDRImage(false),
      m_exposureComp(0.0),
      m_aperture(2.0),
      m_shutterSpeed(1/1000.0),
      m_isoSpeed(100),
      m_focalLength(17.0),
      m_orientation(0)
   {
   }

   // getter

   /// returns filename of image
   const CString& Filename() const throw() { return m_filename; }

   /// returns if image was taken with AEB mode
   bool AutoBracketMode() const throw() { return m_autoBracketMode; }

   /// indicates if image is start of an HDR image
   bool StartHDRImage() const throw() { return m_startHDRImage; }

   /// returns exposure compensation value, when AEB image
   double ExposureComp() const throw() { return m_exposureComp; }

   /// returns aperture value, in f/ format
   double Aperture() const throw() { return m_aperture; }

   /// returns shutter speed, in seconds
   double ShutterSpeed() const throw() { return m_shutterSpeed; }

   /// returns ISO speed
   unsigned int IsoSpeed() const throw() { return m_isoSpeed; }

   /// returns focal length of image taken, in mm
   double FocalLength() const throw() { return m_focalLength; }

   /// returns orientation, as EXIF specific value
   unsigned int Orientation() const throw() { return m_orientation; }

   /// returns start date of image taken
   const ATL::CTime& ImageDateStart() const throw() { return m_imageDateStart; }

   /// returns end date of image taken; may differ from ImageDateStart() when
   /// doing long exposure images, e.g. with shutter speed > 1 seconds
   const ATL::CTime& ImageDateEnd() const throw() { return m_imageDateEnd; }

   // setter

   /// sets filename
   void Filename(const CString& filename) throw() { m_filename = filename; }

   /// sets if image was taken with AEB mode
   void AutoBracketMode(bool autoBracketMode) throw() { m_autoBracketMode = autoBracketMode; }

   /// sets if image is start of an HDR image
   void StartHDRImage(bool startHDRImage) throw() { m_startHDRImage = startHDRImage; }

   /// sets exposure compensation value, when AEB image
   void ExposureComp(double exposureComp) throw() { m_exposureComp = exposureComp; }

   /// sets aperture value, in f/ format
   void Aperture(double aperture) throw() { m_aperture = aperture; }

   /// sets shutter speed, in seconds
   void ShutterSpeed(double shutterSpeed) throw() { m_shutterSpeed = shutterSpeed; }

   /// sets ISO speed
   void IsoSpeed(unsigned int isoSpeed) throw() { m_isoSpeed = isoSpeed; }

   /// sets focal length of image taken, in mm
   void FocalLength(double focalLength) throw() { m_focalLength = focalLength; }

   /// sets orientation, as EXIF specific value
   void Orientation(unsigned int orientation) throw() { m_orientation = orientation; }

   /// sets start date of image taken
   void ImageDateStart(ATL::CTime imageDateStart) throw() { m_imageDateStart = imageDateStart; }

   /// sets end date of image taken
   void ImageDateEnd(ATL::CTime imageDateEnd) throw() { m_imageDateEnd = imageDateEnd; }

   // operators

   /// less operator; orders image file info by ImageDateStart()
   bool operator<(const ImageFileInfo& rhs) const throw()
   {
      return m_imageDateStart < rhs.m_imageDateStart;
   }

private:
   CString m_filename;        ///< filename of image
   bool m_autoBracketMode;    ///< indicates if image was taken with AEB mode
   bool m_startHDRImage;      ///< indicates if image is start of an HDR image
   double m_exposureComp;     ///< exposure compensation value, when AEB image
   double m_aperture;         ///< aperture value, in f/ format
   double m_shutterSpeed;     ///< shutter speed, in seconds
   unsigned int m_isoSpeed;   ///< ISO speed
   double m_focalLength;      ///< focal length of image taken, in mm
   unsigned int m_orientation;   ///< orientation, as EXIF specific value
   ATL::CTime m_imageDateStart;  ///< start date of image taken
   ATL::CTime m_imageDateEnd;    ///< end date of image taken
};
