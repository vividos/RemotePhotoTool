//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2023 Michael Fink
//
/// \file atldataobject.h IDataObject implementation for drag & drop
/// \details most of this code is derived from these two articles:
/// \see https://www.codeproject.com/articles/14482/wtl-for-mfc-programmers-part-x-implementing-a-drag
/// \see https://web.archive.org/web/20050824130636/http://msdn.microsoft.com/library/en-us/dnwui/html/ddhelp_pt2.asp
//
#pragma once

#include <atlcom.h>
#include <oleidl.h>
#include <vector>

/// \brief Implements the IDataObject for drag & drop between your application
/// and other apps, such as Windows Explorer
/// \details Add this to your class that wants to implement IDataObject and,
/// e.g. IDropSource. This class must also be derived from CComObjectRootEx,
/// so that the ::DoDragDrop() call can properly work with the object. Call
/// SetData() to store the data to drop somewhere else, and call DoDragDrop().
template <typename T>
class CDataObjectImpl : public IDataObject
{
public:
   /// Each active FORMATETC gets one of these
   typedef struct
   {
      FORMATETC fe;
      STGMEDIUM stgm;
   } DATAENTRY, * LPDATAENTRY;

   /// ctor
   CDataObjectImpl()
      :m_rgde(NULL),
      m_cde(0)
   {
   }

   /// dtor
   ~CDataObjectImpl()
   {
      for (int ide = 0; ide < m_cde; ide++)
      {
         CoTaskMemFree(m_rgde[ide].fe.ptd);
         ReleaseStgMedium(&m_rgde[ide].stgm);
      }

      CoTaskMemFree(m_rgde);
   }

   // virtual methods from IDataObject

   STDMETHODIMP SetData(FORMATETC* pformatetc, STGMEDIUM* pmedium, BOOL fRelease) override
   {
      if (!fRelease)
         return E_NOTIMPL;

      LPDATAENTRY pde = NULL;
      HRESULT hres = FindFORMATETC(pformatetc, &pde, TRUE);
      if (SUCCEEDED(hres))
      {
         if (pde->stgm.tymed)
         {
            ReleaseStgMedium(&pde->stgm);
            ZeroMemory(&pde->stgm, sizeof(STGMEDIUM));
         }

         if (fRelease)
         {
            pde->stgm = *pmedium;
            hres = S_OK;
         }
         else
         {
            hres = AddRefStgMedium(pmedium, &pde->stgm, TRUE);
         }

         pde->fe.tymed = pde->stgm.tymed;    /* Keep in sync */

         /* Subtlety!  Break circular reference loop */
         if (GetCanonicalIUnknown(pde->stgm.pUnkForRelease) ==
            GetCanonicalIUnknown(static_cast<IDataObject*>(this)))
         {
            pde->stgm.pUnkForRelease->Release();
            pde->stgm.pUnkForRelease = NULL;
         }
      }

      return hres;
   }

   STDMETHODIMP GetData(FORMATETC* pformatetc, STGMEDIUM* pstgm) override
   {
      LPDATAENTRY pde = NULL;
      HRESULT hres = FindFORMATETC(pformatetc, &pde, FALSE);
      if (SUCCEEDED(hres))
         hres = AddRefStgMedium(&pde->stgm, pstgm, FALSE);

      return hres;
   }

   // This typedef creates an IEnumFORMATETC enumerator that the drag source
   // uses in EnumFormatEtc().
   typedef CComEnumOnSTL<
      IEnumFORMATETC,           // name of enumerator interface
      &IID_IEnumFORMATETC,      // IID of enumerator interface
      FORMATETC,                // type of object to return
      _Copy<FORMATETC>,         // copy policy class
      std::vector<FORMATETC> >  // type of collection holding the data
      CEnumFORMATETCImpl;

   STDMETHODIMP EnumFormatEtc(DWORD dwDirection, IEnumFORMATETC** ppenumFormatEtc) override
   {
      UNUSED(dwDirection);

      HRESULT hr;
      CComObject<CEnumFORMATETCImpl>* pImpl;

      // Create an enumerator object.
      hr = CComObject<CEnumFORMATETCImpl>::CreateInstance(&pImpl);

      if (FAILED(hr))
         return hr;

      pImpl->AddRef();

      // Fill in m_vecSupportedFormats with the formats that the caller has
      // put in this object.
      m_vecSupportedFormats.clear();

      for (int i = 0; i < m_cde; i++)
         m_vecSupportedFormats.push_back(m_rgde[i].fe);

      // Init the enumerator, passing it our vector of supported FORMATETCs.
      hr = pImpl->Init(static_cast<T*>(this)->GetUnknown(), m_vecSupportedFormats);

      if (FAILED(hr))
      {
         pImpl->Release();
         return E_UNEXPECTED;
      }

      // Return the requested interface to the caller.
      hr = pImpl->QueryInterface(ppenumFormatEtc);

      pImpl->Release();
      return hr;
   }

   STDMETHODIMP QueryGetData(FORMATETC* pformatetc) override
   {
      LPDATAENTRY pde = NULL;
      return FindFORMATETC(pformatetc, &pde, FALSE);
   }

   STDMETHODIMP GetDataHere(FORMATETC* pformatetc, STGMEDIUM* pmedium) override
   {
      UNUSED(pformatetc);
      UNUSED(pmedium);
      return E_NOTIMPL;
   }

   STDMETHODIMP GetCanonicalFormatEtc(FORMATETC* pformatectIn,
      FORMATETC* pformatetcOut)
   {
      UNUSED(pformatectIn);
      UNUSED(pformatetcOut);
      return E_NOTIMPL;
   }

   STDMETHODIMP DAdvise(FORMATETC* pformatetc, DWORD advf,
      IAdviseSink* pAdvSink, DWORD* pdwConnection) override
   {
      UNUSED(pformatetc);
      UNUSED(advf);
      UNUSED(pAdvSink);
      UNUSED(pdwConnection);
      return E_NOTIMPL;
   }

   STDMETHODIMP DUnadvise(DWORD dwConnection) override
   {
      UNUSED(dwConnection);
      return E_NOTIMPL;
   }

   STDMETHODIMP EnumDAdvise(IEnumSTATDATA** ppenumAdvise) override
   {
      UNUSED(ppenumAdvise);
      return E_NOTIMPL;
   }

private:
   HRESULT FindFORMATETC(FORMATETC* pfe, LPDATAENTRY* ppde, BOOL fAdd)
   {
      *ppde = NULL;

      // Comparing two DVTARGETDEVICE structures is hard, so we don't even try
      if (pfe->ptd != NULL)
         return DV_E_DVTARGETDEVICE;

      // See if it's in our list
      int ide;
      for (ide = 0; ide < m_cde; ide++)
      {
         if (m_rgde[ide].fe.cfFormat == pfe->cfFormat &&
            m_rgde[ide].fe.dwAspect == pfe->dwAspect &&
            m_rgde[ide].fe.lindex == pfe->lindex)
         {
            if (fAdd || (m_rgde[ide].fe.tymed & pfe->tymed))
            {
               *ppde = &m_rgde[ide];
               return S_OK;
            }
            else
               return DV_E_TYMED;
         }
      }

      if (!fAdd)
         return DV_E_FORMATETC;

      LPDATAENTRY pdeT = (LPDATAENTRY)CoTaskMemRealloc(m_rgde,
         sizeof(DATAENTRY) * (m_cde + 1));

      if (pdeT)
      {
         m_rgde = pdeT;
         m_cde++;
         m_rgde[ide].fe = *pfe;
         ZeroMemory(&pdeT[ide].stgm, sizeof(STGMEDIUM));
         *ppde = &m_rgde[ide];
         return S_OK;
      }
      else
         return E_OUTOFMEMORY;
   }

   HRESULT AddRefStgMedium(STGMEDIUM* pstgmIn, STGMEDIUM* pstgmOut, BOOL fCopyIn)
   {
      HRESULT hres = S_OK;
      STGMEDIUM stgmOut = *pstgmIn;

      if (pstgmIn->pUnkForRelease == NULL &&
         !(pstgmIn->tymed & (TYMED_ISTREAM | TYMED_ISTORAGE)))
      {
         if (fCopyIn)
         {
            // Object needs to be cloned
            if (pstgmIn->tymed == TYMED_HGLOBAL)
            {
               stgmOut.hGlobal = GlobalClone(pstgmIn->hGlobal);
               if (!stgmOut.hGlobal) {
                  hres = E_OUTOFMEMORY;
               }
            }
            else
            {
               hres = DV_E_TYMED;      /* Don't know how to clone GDI objects */
            }
         }
         else
         {
            stgmOut.pUnkForRelease = static_cast<IDataObject*>(this);
         }
      }

      if (SUCCEEDED(hres))
      {
         switch (stgmOut.tymed)
         {
         case TYMED_ISTREAM:
            stgmOut.pstm->AddRef();
            break;
         case TYMED_ISTORAGE:
            stgmOut.pstg->AddRef();
            break;
         }

         if (stgmOut.pUnkForRelease)
            stgmOut.pUnkForRelease->AddRef();

         *pstgmOut = stgmOut;
      }

      return hres;
   }

   static HGLOBAL GlobalClone(HGLOBAL hglobIn)
   {
      HGLOBAL hglobOut = NULL;

      LPVOID pvIn = GlobalLock(hglobIn);
      if (pvIn)
      {
         SIZE_T cb = GlobalSize(hglobIn);
         hglobOut = GlobalAlloc(GMEM_FIXED, cb);
         if (hglobOut)
            CopyMemory(hglobOut, pvIn, cb);

         GlobalUnlock(hglobIn);
      }

      return hglobOut;
   }


   static IUnknown* GetCanonicalIUnknown(IUnknown* punk)
   {
      IUnknown* punkCanonical = NULL;
      if (punk && SUCCEEDED(punk->QueryInterface(IID_IUnknown,
         (LPVOID*)&punkCanonical)))
      {
         punkCanonical->Release();
      }
      else
      {
         punkCanonical = punk;
      }

      return punkCanonical;
   }

   /// Array of active DATAENTRY entries
   LPDATAENTRY m_rgde;

   /// Size of m_rgde
   int m_cde;

   // List of FORMATETCs for which we have data, used in EnumFormatEtc
   std::vector<FORMATETC> m_vecSupportedFormats;
};
