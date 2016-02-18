//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file PreviousImagesView.hpp Previous images view
//
#pragma once

// includes
#include "IPhotoModeView.hpp"
#include "PreviousImagesManager.hpp"

// forward references
class IPhotoModeViewHost;
class SourceDevice;

/// previous images view
class PreviousImagesView :
   public CWindowImpl<PreviousImagesView>,
   public IPhotoModeView,
   public CIdleHandler
{
   /// base class type
   typedef CWindowImpl<PreviousImagesView> BaseClass;

public:
   /// ctor
   PreviousImagesView(IPhotoModeViewHost& host) throw();
   /// dtor
   virtual ~PreviousImagesView() throw() {}

   DECLARE_WND_SUPERCLASS(NULL, CListViewCtrl::GetWndClassName())

private:
   // virtual methods from IPhotoModeView

   virtual HWND CreateView(HWND hWndParent) override;

   virtual void SetSourceDevice(std::shared_ptr<SourceDevice> /*spSourceDevice*/) override
   {
   }

   virtual BOOL PreTranslateMessage(MSG* /*pMsg*/) override
   {
      return FALSE;
   }

   virtual void DestroyView() override;

   // virtual methods from CIdleHandler

   /// called to update controls when idle
   virtual BOOL OnIdle() override;

private:
   BEGIN_MSG_MAP(PreviousImagesView)
      MESSAGE_HANDLER(WM_PREV_IMAGES_UPDATE, OnMessageUpdatedCurrentImage)
      MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBkgnd)
      MESSAGE_HANDLER(WM_PAINT, OnPaint)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_PREV, OnPrevImagesPrev)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_NEXT, OnPrevImagesNext)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_OPEN_FOLDER, OnPrevImagesOpenFolder)
      COMMAND_ID_HANDLER(ID_PREV_IMAGES_EDIT, OnPrevImagesEdit)
   END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
// LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
// LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled)
// LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& bHandled)

   /// called when the "previous image" button is pressed
   LRESULT OnPrevImagesPrev(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when the "next image" button is pressed
   LRESULT OnPrevImagesNext(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when the "open folder" button is pressed
   LRESULT OnPrevImagesOpenFolder(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called when the "edit image" button is pressed
   LRESULT OnPrevImagesEdit(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);

   /// called by OnUpdatedCurrentImage() to update current image; runs in UI thread
   LRESULT OnMessageUpdatedCurrentImage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// erases background
   LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

   /// paints viewfinder image
   LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

private:
   /// draw empty view, when no image is currently available
   void DrawEmptyView(CPaintDC& dc);

   /// draws current image
   void DrawCurrentImage(CPaintDC& dc, int& iWidth);

   /// draws image infos
   void DrawImageInfos(CPaintDC& dc, int iWidth);

   /// starts asynchronously getting an image
   void StartGetImage(PreviousImagesManager::T_enRequestImageType enRequestImageType);

   /// called by PreviousImagesManager when a requested image has been loaded
   void OnUpdatedCurrentImage(std::shared_ptr<PreviousImageInfo> spImageInfo);

   /// updates bitmap to draw by using current image
   void UpdateCurrentImage();

   /// scales bitmap size, according to window size
   void ScaleBitmapSize(const BITMAP& bm, int& iWidth, int& iHeight);

   /// opens folder of current image
   void OpenFolder(const CString& cszFolderName);

   /// starts application to edit image
   void EditImage(const CString& cszImageFilename);

private:
   // UI

   /// host access
   IPhotoModeViewHost& m_host;

   /// bitmap for image to show
   CBitmap m_bmpPreviousImage;

   // Model

   /// previous images manager
   PreviousImagesManager& m_manager;

   /// currently shown image
   std::shared_ptr<PreviousImageInfo> m_spCurrentImage;
};
