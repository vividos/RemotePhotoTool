//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2Common.hpp gPhoto2 - Common functions
//
#pragma once

#include "Instance.hpp"
#include "CameraException.hpp"
#include "SdkReferenceBase.hpp"

struct _GPContext;
struct _Camera;
struct _CameraAbilitiesList;
struct _GPPortInfoList;

/// gPhoto2 interface classes
namespace GPhoto2
{
   /// checks for gPhoto2 error and throws exception
   void CheckError(const CString& function, int errorCode, LPCSTR filename, UINT lineNumber);

   /// gPhoto2 reference
   class Ref : public SdkReferenceBase, public std::enable_shared_from_this<Ref>
   {
   public:
      /// ctor
      Ref();
      /// dtor
      ~Ref();

      /// adds gPhoto2 version text
      virtual void AddVersionText(CString& versionText) const override;

      /// enumerates cameras
      virtual void EnumerateDevices(std::vector<std::shared_ptr<SourceInfo>>& sourceDevicesList) const override;

      /// returns if AsyncWaitForCamera() is possible for this SDK
      virtual bool IsAsyncWaitPossible() const override { return false; }

      /// returns gPhoto2 context
      const std::shared_ptr<_GPContext>& GetContext() const { return m_context; }

      /// returns camera abilities list
      const std::shared_ptr<_CameraAbilitiesList>& GetCameraAbilitiesList() const { return m_abilitiesList; }

      /// returns port info list
      const std::shared_ptr<_GPPortInfoList>& GetPortInfoList() const { return m_portInfoList; }

   private:
      /// initializes the camera abilities list
      bool InitCameraAbilitiesList();

      /// initializes the port info list
      bool InitPortInfoList();

   private:
      /// gPhoto2 context
      std::shared_ptr<_GPContext> m_context;

      /// list of all abilities (loaded camlib drivers)
      std::shared_ptr<_CameraAbilitiesList> m_abilitiesList;

      /// list of all port infos (iolib drivers)
      std::shared_ptr<_GPPortInfoList> m_portInfoList;
   };

   /// smart pointer to gPhoto2 reference
   typedef std::shared_ptr<Ref> RefSp;

} // namespace GPhoto2
