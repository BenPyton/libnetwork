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
