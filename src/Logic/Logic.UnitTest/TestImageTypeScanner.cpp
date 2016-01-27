//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file TestImageTypeScanner.cpp tests ImageTypeScanner class
//

// includes
#include "stdafx.h"
#include "ImageTypeScanner.hpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

/// unit tests for project Logic
namespace LogicUnitTest
{
   /// Tests for class ImageTypeScanner
   TEST_CLASS(TestImageTypeScanner)
   {
   public:
      /// creates a time offset, for use in CreateImage() call
      static ATL::CTime GetTimeOffset(double secondsToAdd)
      {
         ATL::CTime time(2016, 1, 27, 18, 0, 0);
         time += ATL::CTimeSpan(0, 0, 0, static_cast<int>(secondsToAdd));

         return time;
      }

      /// creates an image info for use in tests
      static ImageFileInfo CreateImage(double aperture, double shutterSpeed, double focalLength,
         ATL::CTime time = GetTimeOffset(0.0))
      {
         ImageFileInfo image(_T("IMG_0000.jpg"));

         image.Aperture(aperture);
         image.ShutterSpeed(shutterSpeed);
         image.FocalLength(focalLength);

         image.ImageDateStart(time);
         image.ImageDateEnd(time + ATL::CTimeSpan(0, 0, 0, static_cast<int>(shutterSpeed)));

         return image;
      }

      /// creates an HDR image info for use in tests
      static ImageFileInfo CreateHDRImage(double exposureComp, ATL::CTime time = GetTimeOffset(0.0))
      {
         ImageFileInfo image = CreateImage(2.8, 0.01, 17.0, time);

         image.AutoBracketMode(true);
         image.ExposureComp(exposureComp);

         return image;
      }

      /// Tests scanning empty list of images.
      TEST_METHOD(TestNoImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.empty() == true, _T("there must be no result"));
      }

      /// Tests scanning normal images that are neither panorama nor HDR images.
      TEST_METHOD(TestNormalImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(5.6, 0.02, 17.0, GetTimeOffset(23.0)));
         imageFileList.push_back(CreateImage(2.8, 0.02, 22.0, GetTimeOffset(50.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 3, _T("there must be 3 normal images"));
      }

      /// Tests three images that make up a panorama, without other normal images.
      TEST_METHOD(TestOnlyPanoramaImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(2.8, 0.011, 17.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateImage(3.2, 0.01, 17.0, GetTimeOffset(3.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypePano, _T("result type must be 'panorama'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests 3 panorama images, together with a normal image at the start of the list.
      TEST_METHOD(TestPanoramaWithNormalAtStart)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(5.6, 0.01, 22.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(5.0)));
         imageFileList.push_back(CreateImage(2.8, 0.011, 17.0, GetTimeOffset(6.0)));
         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(9.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 2, _T("there must be two results"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("first result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 1, _T("there must be 1 normal images"));

         Assert::IsTrue(resultList[1].ImageType() == T_enImageType::imageTypePano, _T("second result type must be 'panorama'"));
         Assert::IsTrue(resultList[1].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests 3 panorama images, together with a normal image at the end of the list.
      TEST_METHOD(TestPanoramaWithNormalAtEnd)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(2.8, 0.011, 17.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(5.0)));
         imageFileList.push_back(CreateImage(5.6, 0.01, 22.0, GetTimeOffset(27.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 2, _T("there must be two results"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("first result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 1, _T("there must be 1 normal images"));

         Assert::IsTrue(resultList[1].ImageType() == T_enImageType::imageTypePano, _T("second result type must be 'panorama'"));
         Assert::IsTrue(resultList[1].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests 3 panorama images, together with two normal images, at the start and end of the
      /// list, respectively.
      TEST_METHOD(TestPanoramaWithNormalAtStartAndEnd)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(5.6, 0.01, 22.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(10.0)));
         imageFileList.push_back(CreateImage(2.8, 0.011, 17.0, GetTimeOffset(11.0)));
         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(15.0)));
         imageFileList.push_back(CreateImage(5.6, 0.01, 22.0, GetTimeOffset(37.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 2, _T("there must be two results"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("first result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 2, _T("there must be 2 normal images"));

         Assert::IsTrue(resultList[1].ImageType() == T_enImageType::imageTypePano, _T("second result type must be 'panorama'"));
         Assert::IsTrue(resultList[1].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests three images that make up a HDR image, without other images.
      TEST_METHOD(TestOnlyHDRImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(2.8, 0.005, 17.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateImage(2.8, 0.02, 17.0, GetTimeOffset(3.0)));

         imageFileList[0].AutoBracketMode(true);
         imageFileList[1].AutoBracketMode(true);
         imageFileList[2].AutoBracketMode(true);

         imageFileList[0].ExposureComp(0.0);
         imageFileList[1].ExposureComp(-2.0);
         imageFileList[2].ExposureComp(2.0);

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeHDR, _T("result type must be 'HDR'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 3, _T("there must be 3 HDR images"));
      }

      /// Tests three HDR images, together with a normal image at the start of the list.
      TEST_METHOD(TestHDRWithNormalAtStart)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(5.6, 0.02, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(20.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(21.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(23.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 2, _T("there must be two results"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("first result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 1, _T("there must be 1 normal images"));

         Assert::IsTrue(resultList[1].ImageType() == T_enImageType::imageTypeHDR, _T("second result type must be 'HDR'"));
         Assert::IsTrue(resultList[1].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests three HDR images, together with a normal image at the end of the list.
      TEST_METHOD(TestHDRWithNormalAtEnd)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(3.0)));
         imageFileList.push_back(CreateImage(5.6, 0.02, 17.0, GetTimeOffset(23.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 2, _T("there must be two results"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("first result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 1, _T("there must be 1 normal images"));

         Assert::IsTrue(resultList[1].ImageType() == T_enImageType::imageTypeHDR, _T("second result type must be 'HDR'"));
         Assert::IsTrue(resultList[1].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests three HDR images, together with two normal images, at the start and end of the
      /// list, respectively.
      TEST_METHOD(TestHDRWithNormalAtStartAndEnd)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(5.6, 0.02, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(20.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(21.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(23.0)));
         imageFileList.push_back(CreateImage(5.6, 0.02, 17.0, GetTimeOffset(23.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 2, _T("there must be two results"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("first result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 2, _T("there must be 2 normal images"));

         Assert::IsTrue(resultList[1].ImageType() == T_enImageType::imageTypeHDR, _T("second result type must be 'HDR'"));
         Assert::IsTrue(resultList[1].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests multiple HDR images without other images, not being a HDR panorama.
      TEST_METHOD(TestMultipleHDRImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(3.0)));

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(20.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(21.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(23.0)));

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(40.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(41.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(43.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeHDR, _T("result type must be 'HDR'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 9, _T("there must be 9 HDR images"));
      }

      /// Tests images forming a single HDR panorama image.
      TEST_METHOD(TestOnlyHDRPanoramaImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(3.0)));

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(10.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(11.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(13.0)));

         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(20.0)));
         imageFileList.push_back(CreateHDRImage(-2.0, GetTimeOffset(21.0)));
         imageFileList.push_back(CreateHDRImage(2.0, GetTimeOffset(23.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeHDRPano, _T("result type must be 'HDR Panorama'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 9, _T("there must be 9 HDR images"));
      }

      /// Tests image that is marked as the start of a HDR image, but there are no other HDR
      /// images, only normal ones.
      TEST_METHOD(TestSingleStartedHDRImages)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(5.6, 0.02, 22.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateHDRImage(0.0, GetTimeOffset(5.0)));
         imageFileList.push_back(CreateImage(3.2, 0.02, 55.0, GetTimeOffset(13.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeNormal, _T("result type must be 'normal'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 3, _T("there must be 3 panorama images"));
      }

      /// Tests three images that make up a HDR image, but don't use the AEB flags, but other
      /// attributes like shutter speed, etc.
      TEST_METHOD(TestNonAEBHDRImage)
      {
         // set up
         std::vector<ImageFileInfo> imageFileList;

         imageFileList.push_back(CreateImage(2.8, 0.005, 17.0, GetTimeOffset(0.0)));
         imageFileList.push_back(CreateImage(2.8, 0.01, 17.0, GetTimeOffset(1.0)));
         imageFileList.push_back(CreateImage(2.8, 0.02, 17.0, GetTimeOffset(3.0)));

         // run
         ImageTypeScannerOptions options;
         ImageTypeScanner scanner(options);

         std::vector<ImageTypeFilesList> resultList;
         scanner.ScanImages(imageFileList, resultList);

         // check
         Assert::IsTrue(resultList.size() == 1, _T("there must be one result"));

         Assert::IsTrue(resultList[0].ImageType() == T_enImageType::imageTypeHDR, _T("result type must be 'HDR'"));
         Assert::IsTrue(resultList[0].ImageFileInfoList().size() == 3, _T("there must be 3 HDR images"));
      }

   };
}
