//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2006,2007,2008,2009,2015 Michael Fink
//
/// \file Exif.hpp Wrapper around libexif
//
#pragma once

// includes
#include <memory>
#include <functional>
#include <algorithm>
#include <vector>
#include <libexif/exif-data.h>
#include <libexif/exif-loader.h>

/// \brief Exif data access
namespace Exif
{

/// memory allocator class
class Mem
{
public:
   /// ctor; creates default memory allocator
   Mem() throw()
      :m_spMem(exif_mem_new_default(), exif_mem_unref)
   {
   }

   /// ctor; creates memory allocator with given alloc, realloc and free function
   Mem(ExifMemAllocFunc fnAlloc, ExifMemReallocFunc fnRealloc, ExifMemFreeFunc fnFree) throw()
      :m_spMem(exif_mem_new(fnAlloc, fnRealloc, fnFree), exif_mem_unref)
   {
   }

   /// allocates memory with given size
   void* Alloc(ExifLong size) const throw()
   {
      return exif_mem_alloc(m_spMem.get(), size);
   }

   /// reallocates memory with given size
   void* Realloc(void* p, ExifLong size) const throw()
   {
      return exif_mem_realloc(m_spMem.get(), p, size);
   }

   /// frees memory
   void Free(void* p) const throw()
   {
      exif_mem_free(m_spMem.get(), p);
   }

   /// return raw memory alloc object
   ExifMem* Get() const throw(){ return m_spMem.get(); }

private:
   /// memory allocator instance
   std::shared_ptr<_ExifMem> m_spMem;
};


/// exif entry
class Entry
{
public:
   /// ctor; creates a new tag
   Entry(ExifTag tag)
      :m_spEntry(exif_entry_new(), exif_entry_unref)
   {
      exif_entry_initialize(m_spEntry.get(), tag);
   }

   /// ctor; creates a new tag, with custom memory allocator
   Entry(Mem m, ExifTag tag)
      :m_spEntry(exif_entry_new_mem(m.Get()), exif_entry_unref)
   {
      exif_entry_initialize(m_spEntry.get(), tag);
   }

   /// returns tag value
   ExifTag Tag() const
   {
      return m_spEntry->tag;
   }

   /// returns the format of the entry
   ExifFormat Format() const
   {
      return m_spEntry->format;
   }

   /// returns number of components in this entry
   unsigned long NumComponents() const
   {
      return m_spEntry->components;
   }

   /// Returns entry value as formatted string, depending on the
   /// type and format of the entry.
   CString GetValue() const
   {
      CStringA cszaValue;
      exif_entry_get_value(m_spEntry.get(), cszaValue.GetBuffer(1024), 1024);
      cszaValue.ReleaseBuffer();

      return CString(cszaValue);
   }

   /// returns data in ascii format
   CString GetAscii() const
   {
      ATLASSERT(m_spEntry->format == EXIF_FORMAT_ASCII);

      LPCSTR pszaData = reinterpret_cast<LPCSTR>(m_spEntry->data);
      CString cszText(pszaData, m_spEntry->components);
      return cszText;
   }

   /// returns data as signed rational
   ExifSRational GetSRational(unsigned long ulIndex) const
   {
      ulIndex;
      ATLASSERT(m_spEntry->parent->parent != NULL);
      ATLASSERT(m_spEntry->format == EXIF_FORMAT_SRATIONAL);
      ATLASSERT(ulIndex <= m_spEntry->components);

      ExifByteOrder o = exif_data_get_byte_order(m_spEntry->parent->parent);

      return exif_get_srational(m_spEntry->data, o);
   }

   /// returns data as unsigned rational
   ExifRational GetRational(unsigned long ulIndex) const
   {
      ulIndex;
      ATLASSERT(m_spEntry->parent->parent != NULL);
      ATLASSERT(m_spEntry->format == EXIF_FORMAT_RATIONAL);
      ATLASSERT(ulIndex <= m_spEntry->components);

      ExifByteOrder o = exif_data_get_byte_order(m_spEntry->parent->parent);

      return exif_get_rational(m_spEntry->data, o);
   }

   /// returns data as unsigned short value
   ExifShort GetShort() const
   {
      ATLASSERT(m_spEntry->format == EXIF_FORMAT_SHORT);

      ExifByteOrder o = exif_data_get_byte_order(m_spEntry->parent->parent);

      return exif_get_short(m_spEntry->data, o);
   }

   /// sets new ASCII formatted value for entry
   void SetAscii(LPCSTR pszText)
   {
      ATLASSERT(m_spEntry->format == EXIF_FORMAT_ASCII);
      ATLASSERT(m_spEntry->components > 1);

      memset(m_spEntry->data, 0, static_cast<size_t>(m_spEntry->components));

      size_t iSize = std::min(static_cast<size_t>(m_spEntry->components-1), strlen(pszText));
      memcpy(m_spEntry->data, pszText, iSize);
   }

   /// sets new unsigned rational value for entry
   void SetRational(ExifRational adRational[], unsigned long ulMax)
   {
      ATLASSERT(m_spEntry->parent->parent != NULL);
      ATLASSERT(m_spEntry->format == EXIF_FORMAT_RATIONAL);
      ATLASSERT(ulMax <= m_spEntry->components);

      ExifByteOrder o = exif_data_get_byte_order(m_spEntry->parent->parent);

      unsigned char ucSize = exif_format_get_size(m_spEntry->format);
      for(unsigned long ul=0; ul<ulMax; ul++)
         exif_set_rational(m_spEntry->data + ul * ucSize, o, adRational[ul]);
   }

   /// dumps entry to stdout (useful for debugging)
   void Dump()
   {
      exif_entry_dump(m_spEntry.get(), 0);
   }

   /// gets raw exif entry data struct
   ExifEntry* Get(){ return m_spEntry.get(); }

protected:
   friend class Content;

   /// ctor; constructs entry from raw exif entry data struct
   Entry(ExifEntry* p)
      :m_spEntry(p, exif_entry_unref)
   {
      exif_entry_ref(p);
   }

private:
   /// raw exif entry pointer
   std::shared_ptr<_ExifEntry> m_spEntry;
};

/// exif content
class Content
{
public:
   /// ctor; creates new content element
   Content()
      :m_spContent(exif_content_new(), exif_content_unref)
   {
   }

   /// ctor; creates new content element, with custom allocator
   Content(Mem m)
      :m_spContent(exif_content_new_mem(m.Get()), exif_content_unref)
   {
   }

   /// creates entry in content element
   Entry CreateEntry(ExifTag tag)
   {
      ExifEntry* e = exif_entry_new();

      exif_content_add_entry(m_spContent.get(), e);

//      exif_content_ref(m_spContent.get());
//      e->parent = m_spContent.get();

      exif_entry_initialize(e, tag);

      return Entry(e);
   }

   /// removes entry from content element
   void RemoveEntry(Entry e)
   {
      exif_content_remove_entry(m_spContent.get(), e.Get());
   }

   /// gets specific entry in content element, with given tag value
   Entry GetEntry(ExifTag tag)
   {
      return Entry(exif_content_get_entry(m_spContent.get(), tag));
   }

   /// returns if an entry with given tag value exists in the content element
   bool IsEntryAvail(ExifTag tag)
   {
      return NULL != exif_content_get_entry(m_spContent.get(), tag);
   }

   /// function to call for every entry; used in ForeachContent()
   typedef std::function<void (Entry)> T_fnForeachEntry;

   /// iterates through all entries in the content element
   void ForeachContent(T_fnForeachEntry fnForeachEntry)
   {
      exif_content_foreach_entry(m_spContent.get(), ForeachCallback, &fnForeachEntry);
   }

   /// dumps content element to stdout (useful for debugging)
   void Dump()
   {
      exif_content_dump(m_spContent.get(), 0);
   }

   /// gets raw exif content data struct
   ExifContent* Get(){ return m_spContent.get(); }

private:
   /// callback used in ForeachContent() to iterate through entry elements
   static void ForeachCallback(ExifEntry* entry, void* user)
   {
      T_fnForeachEntry* pfnData = reinterpret_cast<T_fnForeachEntry*>(user);
      Entry e(entry);
      (*pfnData)(e);
   }

protected:
   friend class Data;

   /// ctor; constructs content from raw exif content data struct
   Content(ExifContent* p)
      :m_spContent(p, exif_content_unref)
   {
      exif_content_ref(p);
   }

private:
   /// raw exif content pointer
   std::shared_ptr<_ExifContent> m_spContent;
};


/// exif data
class Data
{
public:
   /// constructs new and empty data element
   Data()
      :m_spData(exif_data_new(), exif_data_unref)
   {
   }

   /// constructs new and empty data element, with custom allocator
   Data(Mem m)
      :m_spData(exif_data_new_mem(m.Get()), exif_data_unref)
   {
   }

   /// loads data from file
   Data(const CString& cszFilename)
      :m_spData(exif_data_new_from_file(CStringA(cszFilename)), exif_data_unref)
   {
   }

   /// loads data from memory block
   Data(const unsigned char* pucData, unsigned int uiLength)
      :m_spData(exif_data_new_from_data(pucData, uiLength), exif_data_unref)
   {
   }

   /// returns if Exif::Content data for a given IFD (image file descriptor) is available
   /// \note: use EXIF_IFD_EXIF to access Exif data of Jpeg image
   bool IsContentIfdAvail(ExifIfd ifd)
   {
      return NULL != m_spData->ifd[ifd];
   }

   /// returns content for given ifd
   /// \note: use EXIF_IFD_EXIF to access Exif data of Jpeg image
   Content GetContent(ExifIfd ifd)
   {
      ATLASSERT(true == IsContentIfdAvail(ifd));
      return Content(m_spData->ifd[ifd]);
   }

   /// returns byte order of data
   ExifByteOrder ByteOrder()
   {
      return exif_data_get_byte_order(m_spData.get());
   }

   /// sets new byte order for data
   void ByteOrder(ExifByteOrder byteOrder)
   {
      exif_data_set_byte_order(m_spData.get(), byteOrder);
   }

   /// function to call for every content element; used in ForeachContent()
   typedef std::function<void(Content)> T_fnForeachContent;

   /// iterates through all content elements in the data element
   void ForeachContent(T_fnForeachContent fnForeachContent)
   {
      exif_data_foreach_content(m_spData.get(), ForeachCallback, &fnForeachContent);
   }

   /// saves data block including modified content and elements to byte vector
   void Save(std::vector<BYTE>& vecData)
   {
      unsigned char* data = NULL;
      unsigned int size = 0;
      exif_data_save_data(m_spData.get(), &data, &size);

      vecData.assign(data, data + size);

      free(data); // as seen in test-mem.c from libexif
   }

   /// dumps data element to stdout (useful for debugging)
   void Dump()
   {
      exif_data_dump(m_spData.get());
   }

   /// gets raw exif data struct
   ExifData* Get(){ return m_spData.get(); }

private:
   /// callback used in ForeachContent() to iterate through content elements
   static void ForeachCallback(ExifContent* content, void* user)
   {
      T_fnForeachContent* pfnData = reinterpret_cast<T_fnForeachContent*>(user);
      Content c(content);
      (*pfnData)(c);
   }

protected:
   friend class Loader;

   /// ctor; constructs data element from raw exif data struct
   Data(ExifData* p)
      :m_spData(p, exif_data_unref)
   {
      exif_data_ref(p);
   }

private:
   /// raw exif data pointer
   std::shared_ptr<_ExifData> m_spData;
};


/// exif loader
class Loader
{
public:
   /// ctor; creates a new loader object
   Loader()
      :m_spLoader(exif_loader_new(), exif_loader_unref)
   {
   }

   /// ctor; creates a new loader object, with custom memory allocator
   Loader(Mem m)
      :m_spLoader(exif_loader_new_mem(m.Get()), exif_loader_unref)
   {
   }

   /// loads a file
   void LoadFile(const CString& cszFilename)
   {
      exif_loader_write_file(m_spLoader.get(), CStringA(cszFilename));
   }

   /// returns the data element after file was loaded
   Data GetData()
   {
      return Data(exif_loader_get_data(m_spLoader.get()));
   }

private:
   /// raw exif loader pointer
   std::shared_ptr<_ExifLoader> m_spLoader;
};

} // namespace Exif
