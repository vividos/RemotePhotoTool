//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file AppOptions.cpp Command line app options
//

// includes
#include "stdafx.h"
#include "AppOptions.hpp"

AppOptions::AppOptions(std::vector<AppCommand>& vecCommandList)
:m_vecCommandList(vecCommandList)
{
   RegisterOutputHandler(&ProgramOptions::OutputConsole);
   RegisterHelpOption();

   RegisterOption(_T("v"), _T("version"), _T("shows version info"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::showVersion));

   RegisterOption(_T("l"), _T("list"), _T("lists all available devices"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::listDevices));

   RegisterOption(_T("o"), _T("open"), _T("opens device <arg1>; use {x} to open device with index x in list"),
      1, std::bind(&AppOptions::OnAddCommandWithParam, this, AppCommand::openDevice, std::placeholders::_1));

   RegisterOption(_T("c"), _T("close"), _T("closes opened device"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::closeDevice));

   RegisterOption(_T("d"), _T("device-info"), _T("shows device info of opened device"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::deviceInfo));

   RegisterOption(_T("p"), _T("device-props"), _T("shows device properties"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::deviceProperties));

   RegisterOption(_T(""), _T("check-unknown-device-props"), _T("checks for unknown device properties"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::checkUnknownDeviceProps));

   RegisterOption(_T("i"), _T("image-props"), _T("shows image properties"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::imageProperties));

   RegisterOption(_T(""), _T("check-unknown-image-props"), _T("checks for unknown image properties"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::checkUnknownImageProps));

   RegisterOption(_T(""), _T("remote-caps"), _T("shows remote capabilities"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::remoteCapabilities));

   RegisterOption(_T("e"), _T("listen-events"), _T("listens for events from camera and prints them"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::listenEvents));

   RegisterOption(_T("r"), _T("release"), _T("releases shutter"),
      0, std::bind(&AppOptions::OnAddSimpleCommand, this, AppCommand::releaseShutter));

   RegisterOption(_T("s"), _T("run-script"), _T("runs Lua script <arg1>"),
      1, std::bind(&AppOptions::OnAddCommandWithParam, this, AppCommand::runScript, std::placeholders::_1));
}

bool AppOptions::OnAddSimpleCommand(AppCommand::T_enCommand enCommand)
{
   m_vecCommandList.push_back(AppCommand(enCommand));
   return true;
}

bool AppOptions::OnAddCommandWithParam(AppCommand::T_enCommand enCommand, const std::vector<CString>& vecParam)
{
   ATLASSERT(vecParam.size() == 1);

   m_vecCommandList.push_back(AppCommand(enCommand, vecParam[0]));
   return true;
}
