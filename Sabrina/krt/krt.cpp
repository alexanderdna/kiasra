#include "krt.h"

#include "kenv.hpp"
#include "kmeta.hpp"
#include "kobject.hpp"
#include "kmodule.hpp"

#ifndef NULL
#define NULL 0
#endif

// Creates a new execution environment
KRT_API KRESULT KrtCreateEnvironment(void)
{
	KEnvironment::initialize(true);
	return KRESULT_OK;
}

// Destroys an execution environment and all its resources
KRT_API void KrtDestroyEnvironment(void)
{
	KEnvironment::finalize();
}

// Loads a module from the specified path
KRT_API KRESULT KrtLoadModule(KMODULEATTRIBUTES attrs, kstring_t path, HKMODULE *pHKModule)
{
	ModuleLoader *loader = KEnvironment::createModuleLoader(attrs, L"", path);
	if (!loader->load())
	{
		*pHKModule = NULL;
		return KRESULT_ERR;
	}

	*pHKModule = loader->getModule();
	return KRESULT_OK;
}

// Runs a loaded module
KRT_API KRESULT KrtRunModule(kuint_t argc, kstring_t *argv)
{
	return KRESULT_OK;
}
