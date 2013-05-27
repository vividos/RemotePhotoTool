//
// RemotePhotoTool - remote camera control software
// Copyright (C) 2008-2013 Michael Fink
//
//! \file CanonControlExports.hpp Canon control - Exports for a dll
//

// includes

#ifndef _LIB
// dynamic lib
#  ifdef CANONCONTROL_EXPORTS
#    define CANONCONTROL_DECLSPEC __declspec(dllexport)
#  else
#    define CANONCONTROL_DECLSPEC __declspec(dllimport)
#  endif
#else
// static lib
#  define CANONCONTROL_DECLSPEC
#endif

#ifndef _LIB
#pragma warning(disable: 4251) // 'T1' : class 'T2' needs to have dll-interface to be used by clients of class 'T3'
#pragma warning(disable: 4275) // non dll-interface class 'I' used as base for dll-interface class 'T'
#endif
