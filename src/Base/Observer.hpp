//
// ulib - a collection of useful classes
// Copyright (C) 2008-2013 Michael Fink
//
/// \file Observer.hpp observer pattern classes
//
#pragma once

// includes
#include <map>
#include <functional>

/// \brief observer subject
/// \details a subject can add one or more observers that will be notified
/// once the subject is "called".
template <typename T>
class Subject
{
public:
   /// function type to be called
   typedef std::function<T> T_fnSubject;

   /// map type that holds all subjects to be called
   typedef std::map<int, T_fnSubject> T_mapSubjects;

   /// ctor
   Subject()
      :m_iNextId(1)
   {
   }

   /// adds new observer; new id is returned
   int Add(std::function<T> fnObserver)
   {
      int iHandlerId = m_iNextId++;
      m_mapAllObserver.insert(std::make_pair(iHandlerId, fnObserver));
      return iHandlerId;
   }

   /// adds new observer
   Subject<T>& operator+=(std::function<T> fnObserver)
   {
      Add(fnObserver);
      return *this;
   }

   /// removes an observer by id
   void Remove(int id)
   {
      m_mapAllObserver.erase(id);
   }

   /// calls subject with zero arguments
   void Call()
   {
      T_mapSubjects::const_iterator iter = m_mapAllObserver.begin(), stop = m_mapAllObserver.end();
      for (; iter != stop; iter++)
         (iter->second)();
   }

   /// calls subject with one arguments
   template <typename T1>
   void Call(T1 param1)
   {
      T_mapSubjects::const_iterator iter = m_mapAllObserver.begin(), stop = m_mapAllObserver.end();
      for (; iter != stop; iter++)
         (iter->second)(param1);
   }

   /// calls subject with two arguments
   template <typename T1, typename T2>
   void Call(T1 param1, T2 param2)
   {
      T_mapSubjects::const_iterator iter = m_mapAllObserver.begin(), stop = m_mapAllObserver.end();
      for (; iter != stop; iter++)
      {
         (iter->second)(param1, param2);
      }
   }

   /// removes all observer
   void Clear()
   {
      m_mapAllObserver.clear();
   }

private:
   /// next id
   int m_iNextId;

   /// map with all observer subjects
   T_mapSubjects m_mapAllObserver;
};
