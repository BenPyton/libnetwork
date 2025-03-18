// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _LIBNETWORK_EXPORT_H
#define _LIBNETWORK_EXPORT_H

#ifdef LIBNETWORK_EXPORTS
	#define LIBNETWORK_API __declspec(dllexport)
#else
	#ifdef LIBNETWORK_STATIC
		#define LIBNETWORK_API
	#else
		#define LIBNETWORK_API __declspec(dllimport)
	#endif
#endif

#endif // _LIBNETWORK_EXPORT_H
