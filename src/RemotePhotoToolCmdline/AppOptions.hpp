#pragma once

#include "AppCommand.hpp"
#include "ProgramOptions.hpp"

/// options for application
class AppOptions: public ProgramOptions
{
public:
   /// ctor
   AppOptions(std::vector<AppCommand>& vecCommandList);

private:
   bool OnAddSimpleCommand(AppCommand::T_enCommand enCommand);
   bool OnAddCommandWithParam(AppCommand::T_enCommand enCommand, const std::vector<CString>& vecParam);

private:
   std::vector<AppCommand>& m_vecCommandList;
};
