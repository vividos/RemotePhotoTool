//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file RemotePhotoTool.CameraControl.Viewfinder.cpp Viewfinder wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.CameraControl.Viewfinder.hpp"
#include "Viewfinder.hpp"

bool RemotePhotoTool::CameraControl::Viewfinder::GetCapability(ViewfinderCapability viewfinderCapability)
{
   return m_viewfinder->get()->GetCapability(
      static_cast<::Viewfinder::T_enViewfinderCapability>(viewfinderCapability));
}

RemotePhotoTool::CameraControl::Viewfinder::OutputType RemotePhotoTool::CameraControl::Viewfinder::Output::get()
{
   return m_lastOutputType;
}

void RemotePhotoTool::CameraControl::Viewfinder::Output::set(RemotePhotoTool::CameraControl::Viewfinder::OutputType value)
{
   m_lastOutputType = value;

   m_viewfinder->get()->SetOutputType(
      static_cast<::Viewfinder::T_enOutputType>(value));
}

array<unsigned int>^
RemotePhotoTool::CameraControl::Viewfinder::GetHistogram(HistogramType histogramType)
{
   ::Viewfinder::T_enHistogramType enHistogramType =
      static_cast<::Viewfinder::T_enHistogramType>(histogramType);

   std::vector<unsigned int> histogramValues;
   m_viewfinder->get()->GetHistogram(enHistogramType, histogramValues);

   auto histogramList = gcnew array<unsigned int>(
      static_cast<int>(histogramValues.size()));

   for (size_t index = 0, max = histogramValues.size(); index < max; index++)
   {
      histogramList[index] = histogramValues[index];
   }

   return histogramList;
}

void RemotePhotoTool::CameraControl::Viewfinder::Close()
{
   m_viewfinder->get()->SetAvailImageHandler();
   m_viewfinder->get()->Close();
   m_viewfinder->reset();
}

/// cleans up unmanaged resources
RemotePhotoTool::CameraControl::Viewfinder::!Viewfinder()
{
   delete m_viewfinder;
   m_viewfinder = nullptr;
}

/// cleans up managed resources
RemotePhotoTool::CameraControl::Viewfinder::~Viewfinder()
{
}

RemotePhotoTool::CameraControl::Viewfinder::Viewfinder(
   std::shared_ptr<::Viewfinder> viewfinder)
:m_viewfinder(new std::shared_ptr<::Viewfinder>(viewfinder)),
m_lastOutputType(OutputType::Undefined)
{
}

void OnAvailViewfinderImage(
   msclr::gcroot<RemotePhotoTool::CameraControl::Viewfinder::AvailViewfinderImageHandler^> handler,
   const std::vector<unsigned char>& jpegImageData)
{
   RemotePhotoTool::CameraControl::Viewfinder::AvailViewfinderImageHandler^ callback = handler;

   auto jpegImageDataArray = gcnew array<unsigned char>(
      static_cast<int>(jpegImageData.size()));

   for (size_t index = 0, max = jpegImageData.size(); index < max; index++)
   {
      jpegImageDataArray[index] = jpegImageData[index];
   }

   callback(jpegImageDataArray);
}

void RemotePhotoTool::CameraControl::Viewfinder::OnAvailViewfinderImage(
   array<unsigned char>^ jpegImageData)
{
   AvailViewfinderImage(jpegImageData);
}

void RemotePhotoTool::CameraControl::Viewfinder::InitEventHandler()
{
   AvailViewfinderImageHandler^ availViewfinderImageHandler =
      gcnew AvailViewfinderImageHandler(this, &RemotePhotoTool::CameraControl::Viewfinder::OnAvailViewfinderImage);

   msclr::gcroot<AvailViewfinderImageHandler^> availViewfinderImageCallback(availViewfinderImageHandler);

   m_viewfinder->get()->SetAvailImageHandler(
      std::bind(&::OnAvailViewfinderImage, availViewfinderImageCallback, std::placeholders::_1));
}
