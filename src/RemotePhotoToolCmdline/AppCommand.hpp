#pragma once

struct AppCommand
{
   enum T_enCommand
   {
      showVersion,
      listDevices,
      openDevice,
      closeDevice,
      deviceInfo,
      deviceProperties,
      imageProperties,
      releaseShutter,
      runScript,
   };

   /// ctor
   AppCommand(T_enCommand enCommand, const CString& cszData = _T(""))
      :m_enCommand(enCommand),
       m_cszData(cszData)
   {
   }

   T_enCommand m_enCommand;
   CString m_cszData;
};
