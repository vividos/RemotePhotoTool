//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2020 Michael Fink
//
/// \file GPhoto2BulbReleaseControlImpl.cpp gPhoto2 - Bulb release control impl
//
#include "stdafx.h"
#include "GPhoto2BulbReleaseControlImpl.hpp"
#include "GPhoto2PropertyAccess.hpp"

using GPhoto2::BulbReleaseControlImpl;

BulbReleaseControlImpl::BulbReleaseControlImpl(RefSp ref,
   std::shared_ptr<PropertyAccess> properties)
   :m_ref(ref),
   m_properties(properties)
{
   Variant value;
   value.Set(true);
   value.SetType(Variant::typeBool);

   properties->SetPropertyByName(_T("bulb"), value);

   m_bulbTimer.Start();
}

BulbReleaseControlImpl::~BulbReleaseControlImpl()
{
   try
   {
      Stop();
   }
   catch (...)
   {
   }
}

double BulbReleaseControlImpl::ElapsedTime() const
{
   return m_bulbTimer.TotalElapsed();
}

void BulbReleaseControlImpl::Stop()
{
   if (!m_bulbTimer.IsStarted())
      return;

   Variant value;
   value.Set(false);
   value.SetType(Variant::typeBool);

   m_properties->SetPropertyByName(_T("bulb"), value);

   m_bulbTimer.Stop();
}
