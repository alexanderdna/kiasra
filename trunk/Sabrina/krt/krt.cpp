#include "krt.h"

#include "kenv.hpp"
#include "kmeta.hpp"
#include "kobject.hpp"

#ifndef NULL
#define NULL 0
#endif

// Creates a new execution environment
KRESULT KRT_API KrtCreateEnvironment(void)
{
	KEnvironment::initialize(true);
	return KRESULT_OK;
}

// Destroys an execution environment and all its resources
void KRT_API KrtDestroyEnvironment(void)
{
	KEnvironment::finalize();
}

// Loads a module from the specified path
KRESULT KRT_API KrtLoadModule(kstring_t path, HKMODULE *pHKModule)
{
	return KRESULT_OK;
}

// Runs a loaded module
KRESULT KRT_API KrtRunModule(HKMODULE hKModule)
{
	return KRESULT_OK;
}
