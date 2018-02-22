//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2018 Michael Fink
//
/// \file SharedBuffer.hpp reference counted buffer class
//
#pragma once

// needed includes
#include <vector>
#include <memory>

/// a reference-counted non-modifiable buffer class
/// from boost::asio examples; see:
/// http://www.boost.org/doc/libs/1_35_0/doc/html/boost_asio/example/buffers/reference_counted.cpp
class SharedConstBuffer
{
public:
   /// construct from a given vector
   template <typename InputIterator>
   explicit SharedConstBuffer(InputIterator itBegin, InputIterator itEnd)
      :m_spData(new std::vector<unsigned char>(itBegin, itEnd)),
      m_buffer(boost::asio::buffer(*m_spData))
   {
   }

   /// returns data in buffer; const version
   const std::vector<unsigned char>& Data() const { return *m_spData; }

   // implement the ConstBufferSequence requirements
   typedef boost::asio::const_buffer value_type;            ///< const buffer value type
   typedef const boost::asio::const_buffer* const_iterator; ///< const iterator type
   const boost::asio::const_buffer* begin() const { return &m_buffer; }    ///< returns buffer begin
   const boost::asio::const_buffer* end() const { return &m_buffer + 1; }  ///< returns buffer end

private:
   /// shared data
   std::shared_ptr<std::vector<unsigned char>> m_spData;

   /// buffer
   boost::asio::const_buffer m_buffer;
};

/// reference-count modifiable buffer class
class SharedMutableBuffer
{
public:
   /// ctor
   SharedMutableBuffer(size_t sizeInBytes)
      :m_spData(new std::vector<unsigned char>(sizeInBytes)),
      m_buffer(boost::asio::buffer(*m_spData))
   {
   }

   /// returns data in buffer; non-const version
   std::vector<unsigned char>& Data() { return *m_spData; }

   // implement the MutableBufferSequence requirements
   typedef boost::asio::mutable_buffer value_type;             ///< mutable buffer value type
   typedef const boost::asio::mutable_buffer* const_iterator;  ///< const iterator type
   const boost::asio::mutable_buffer* begin() const { return &m_buffer; }     ///< returns buffer begin
   const boost::asio::mutable_buffer* end() const { return &m_buffer + 1; }   ///< returns buffer end

private:
   /// shared data
   std::shared_ptr<std::vector<unsigned char>> m_spData;

   /// buffer
   boost::asio::mutable_buffer m_buffer;
};
