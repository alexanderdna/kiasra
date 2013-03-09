#include "krt.h"

#include "kenv.hpp"
#include "kmeta.hpp"
#include "kobject.hpp"

#ifndef NULL
#define NULL 0
#endif

// Creates a new execution environment
KRT_API KRESULT KrtCreateEnvironment(HKENV *pHKEnv)
{
	return KRESULT_OK;
}

// Destroys an execution environment and all its resources
KRT_API void KrtDestroyEnvironment(HKENV *pHKEnv)
{
}

// Loads a module from the specified path
KRT_API KRESULT KrtLoadModule(HKENV hKEnv, kstring_t path, HKMODULE *pHKModule)
{
	return KRESULT_OK;
}

// Runs a loaded module
KRT_API KRESULT KrtRunModule(HKENV hKEnv, HKMODULE hKModule)
{
	return KRESULT_OK;
}

// Releases the module and all its resources
KRT_API void KrtFreeModule(HKMODULE *pHKModule)
{
}

// Gets the message describing why the last task failed
KRT_API kstring_t KrtGetFailReason(void)
{
	return NULL;
}

// Gets the last exception if any
KRT_API HKEXCEPTION KrtGetException(HKENV hKEnv)
{
	return NULL;
}

// Gets the message in the specified exception
KRT_API kstring_t KrtGetExceptionMessage(HKENV hKEnv, HKEXCEPTION hKException)
{
	return NULL;
}

// Gets the stack trace from the specified exception
KRT_API kstring_t KrtGetExceptionTrace(HKENV hKEnv, HKEXCEPTION hKException)
{
	return NULL;
}

// Releases a string retrieved from a KRT API function
KRT_API void KrtFreeString(kstring_t *pKString)
{
	delete [] (*pKString);
	*pKString = NULL;
}
