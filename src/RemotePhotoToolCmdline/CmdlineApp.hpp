#pragma once

#include <vector>
#include <memory>
#include "AppCommand.hpp"

class SourceDevice;
class RemoteReleaseControl;

class CmdlineApp
{
public:
   CmdlineApp();

   void Run(int argc, TCHAR* argv[]);

private:
   void Exec(const AppCommand& cmd);

   void PrintVersionInfo();
   void ListDevices();
   void OpenByName(const CString& cszName);
   void OutputDeviceInfo();
   void ListDeviceProperties();
   void ListImageProperties();
   void EnsureReleaseControl();
   void ReleaseShutter();
   void RunScript(const CString& cszFilename);

private:
   std::vector<AppCommand> m_vecCommandList;

   std::shared_ptr<SourceDevice> m_spSourceDevice;
   std::shared_ptr<RemoteReleaseControl> m_spReleaseControl;
};
