//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file RemotePhotoTool.LuaScripting.CameraScriptProcessor.cpp Instance wrapper class
//

// includes
#include "stdafx.h"
#include "RemotePhotoTool.LuaScripting.CameraScriptProcessor.hpp"

RemotePhotoTool::LuaScripting::CameraScriptProcessor::CameraScriptProcessor()
:m_cameraScriptProcessor(new std::shared_ptr<::CameraScriptProcessor>(new ::CameraScriptProcessor))
{
}

/// cleans up managed resources
RemotePhotoTool::LuaScripting::CameraScriptProcessor::~CameraScriptProcessor()
{
}

/// cleans up unmanaged resources
RemotePhotoTool::LuaScripting::CameraScriptProcessor::!CameraScriptProcessor()
{
   delete m_cameraScriptProcessor;
   m_cameraScriptProcessor = nullptr;
}

void RemotePhotoTool::LuaScripting::CameraScriptProcessor::LoadScript(System::String^ filename)
{
   ATLASSERT(m_cameraScriptProcessor != nullptr);

   m_cameraScriptProcessor->get()->LoadScript(filename);
}

void RemotePhotoTool::LuaScripting::CameraScriptProcessor::LoadSourceString(System::String^ luaSource)
{
   ATLASSERT(m_cameraScriptProcessor != nullptr);

   m_cameraScriptProcessor->get()->LoadSourceString(luaSource);
}

void RemotePhotoTool::LuaScripting::CameraScriptProcessor::Run()
{
   ATLASSERT(m_cameraScriptProcessor != nullptr);

   m_cameraScriptProcessor->get()->Run();
}

void RemotePhotoTool::LuaScripting::CameraScriptProcessor::Stop()
{
   ATLASSERT(m_cameraScriptProcessor != nullptr);

   m_cameraScriptProcessor->get()->Stop();
}
