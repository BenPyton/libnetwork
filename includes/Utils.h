#ifndef _UTILS_H
#define _UTILS_H

#ifdef _DEBUG
	#define DebugLog(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
	#define DebugLog(fmt, ...)
#endif

#endif // _UTILS_H

