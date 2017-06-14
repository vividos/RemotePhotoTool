//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2017 Michael Fink
//
/// \file WiaPropertyAccess.hpp WIA - Property access
//
#pragma once

// includes
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

      /// gets property value
      CString Get(PROPID wiaPropertyId) const;

      /// returns a string from PROPVARIANT value
      static CString StringFromPropVariant(const PROPVARIANT& propVariant, ULONG elementIndex = 0);

   private:
      /// item access properties
      CComPtr<IWiaItem> m_item;
   };

} // namespace WIA
