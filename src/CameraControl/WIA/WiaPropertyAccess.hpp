//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file WiaPropertyAccess.hpp WIA - Property access
//
#pragma once

#include "WiaCommon.hpp"
#include <boost/noncopyable.hpp>

namespace WIA
{
   /// property access for WIA items
   class PropertyAccess : public boost::noncopyable
   {
   public:
      /// ctor
      PropertyAccess(CComPtr<IWiaItem> item)
         :m_item(item)
      {
      }

      /// gets string property value
      CString Get(PROPID wiaPropertyId) const;

      /// gets UINT property value
      UINT GetUint(PROPID wiaPropertyId) const;

      /// gets time property value
      time_t GetSystemTime(PROPID wiaPropertyId) const;

      /// returns a string from PROPVARIANT value
      static CString StringFromPropVariant(const PROPVARIANT& propVariant, ULONG elementIndex = 0);

   private:
      /// item access properties
      CComPtr<IWiaItem> m_item;
   };

} // namespace WIA
