// Copyright (c) 2019 Benoit Pelletier
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE or copy at https://www.boost.org/LICENSE_1_0.txt)

#ifndef _UTILS_H
#define _UTILS_H

#ifdef _DEBUG
	#define DebugLog(fmt, ...) printf(fmt, ##__VA_ARGS__)
	#define WDebugLog(fmt, ...) wprintf(fmt, ##__VA_ARGS__)
#else
	#define DebugLog(fmt, ...)
	#define WDebugLog(fmt, ...)
#endif

#endif // _UTILS_H
