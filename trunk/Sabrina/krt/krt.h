#ifndef _KRT_H
#define _KRT_H

#include "kni.h"

/*===================================================*/

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

/*===================================================*/

/* Handle of module */
typedef void * HKMODULE;

/* Handle of exception */
typedef void * HKEXCEPTION;

/*===================================================*/

/* Creates a new execution environment */
KRESULT KRT_API KrtCreateEnvironment(HKENV *pHKEnv);

/* Destroys an execution environment and all its resources */
void KRT_API KrtDestroyEnvironment(HKENV *pHKEnv);

/* Loads a module from the specified path */
KRESULT KRT_API KrtLoadModule(HKENV hKEnv, kstring_t ksPath, HKMODULE *pHKModule);

/* Runs a loaded module */
KRESULT KRT_API KrtRunModule(HKENV hKEnv, HKMODULE hKModule);

/* Releases the module and all its resources */
void KRT_API KrtFreeModule(HKMODULE *pHKModule);

/* Gets the message describing why the last task failed */
kstring_t KRT_API KrtGetFailReason(void);

/* Gets the last exception if any */
HKEXCEPTION KRT_API KrtGetException(HKENV hKEnv);

/* Gets the message in the specified exception */
kstring_t KRT_API KrtGetExceptionMessage(HKENV hKEnv, HKEXCEPTION hKException);

/* Gets the stack trace from the specified exception */
kstring_t KRT_API KrtGetExceptionTrace(HKENV hKEnv, HKEXCEPTION hKException);

/* Releases a string retrieved from a KRT API function */
void KRT_API KrtFreeString(kstring_t *pKString);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif//_KRT_H
