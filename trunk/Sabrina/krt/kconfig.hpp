#pragma once

#if defined(_MSC_VER)
	#if defined(_M_IA64) || defined(_M_X64) || defined(_M_AMD64) || defined(_WIN64)
		#define IS64BIT
	#endif
#else
	#if defined(__ia64__) || defined(__amd64__) || defined(__x86_64__)
		#define IS64BIT
	#endif
#endif

#ifdef IS64BIT
	#define INIT_DEFAULT_VALUE(obj, t) { obj.type = (t); obj.vRaw = NULL; obj.length = 0; }
#else
	#define INIT_DEFAULT_VALUE(obj, t) { obj.type = (t); obj.vULong = 0; obj.length = 0; }
#endif

