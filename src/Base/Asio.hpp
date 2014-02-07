//
// MultiplayerOnlineGame - multiplayer game project
// Copyright (C) 2008-2014 Michael Fink
//
/// \file Asio.hpp Boost.Asio include
//
#pragma once

// don't link Regex and DateTime
//#define BOOST_DATE_TIME_NO_LIB // already defined in compiler options
#define BOOST_REGEX_NO_LIB

// asio but no winsock init
#define BOOST_ASIO_DETAIL_IMPL_WINSOCK_INIT_IPP

// includes
#include <boost/asio.hpp>

// asio but no winsock init

/// fake startup func
inline void boost::asio::detail::winsock_init_base::startup(boost::asio::detail::winsock_init_base::data &,unsigned char,unsigned char){}
/// fake cleanup func
inline void boost::asio::detail::winsock_init_base::cleanup(boost::asio::detail::winsock_init_base::data &){}
/// fake throw func
inline void boost::asio::detail::winsock_init_base::throw_on_error(boost::asio::detail::winsock_init_base::data &){}
