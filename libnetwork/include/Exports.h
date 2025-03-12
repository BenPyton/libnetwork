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