#ifndef _KRT_H
#define _KRT_H

#include "kni.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
	#ifdef KRT_EXPORTS
		#define KRT_API __declspec(dllexport)
	#else
		#define KRT_API __declspec(dllimport)
	#endif
#else
	#define KRT_API
#endif

// Handle of Execution Environment
typedef void * HKENV;

// Handle of module
typedef void * HKMODULE;

// Handle of exception
typedef void * HKEXCEPTION;

typedef enum KRESULT
{
	KRESULT_OK,
	KRESULT_ERR
} KRESULT;

// Creates a new execution environment
KRT_API KRESULT KrtCreateEnvironment(HKENV *pHKEnv);

// Destroys an execution environment and all its resources
KRT_API void KrtDestroyEnvironment(HKENV *pHKEnv);

// Loads a module from the specified path
KRT_API KRESULT KrtLoadModule(HKENV hKEnv, kstring_t ksPath, HKMODULE *pHKModule);

// Runs a loaded module
KRT_API KRESULT KrtRunModule(HKENV hKEnv, HKMODULE hKModule);

// Releases the module and all its resources
KRT_API void KrtFreeModule(HKMODULE *pHKModule);

// Gets the message describing why the last task failed
KRT_API kstring_t KrtGetFailReason(void);

// Gets the last exception if any
KRT_API HKEXCEPTION KrtGetException(HKENV hKEnv);

// Gets the message in the specified exception
KRT_API kstring_t KrtGetExceptionMessage(HKENV hKEnv, HKEXCEPTION hKException);

// Gets the stack trace from the specified exception
KRT_API kstring_t KrtGetExceptionTrace(HKENV hKEnv, HKEXCEPTION hKException);

// Releases a string retrieved from a KRT API function
KRT_API void KrtFreeString(kstring_t *pKString);

#ifdef __cplusplus
}
#endif

#endif//_KRT_H
