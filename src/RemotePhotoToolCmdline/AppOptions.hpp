//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2016 Michael Fink
//
/// \file AppOptions.hpp Command line app options
//
#pragma once

// includes
#include "AppCommand.hpp"
#include <ulib/ProgramOptions.hpp>

/// options for command line application
class AppOptions: public ProgramOptions
{
public:
   /// ctor
   AppOptions(std::vector<AppCommand>& vecCommandList);

private:
   /// command without params
   bool OnAddSimpleCommand(AppCommand::T_enCommand enCommand);

   /// command with params
   bool OnAddCommandWithParam(AppCommand::T_enCommand enCommand, const std::vector<CString>& vecParam);

private:
   /// command list
   std::vector<AppCommand>& m_vecCommandList;
};
