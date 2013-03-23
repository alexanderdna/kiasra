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
KRT_API KRESULT KrtLoadModule(const char *szPath, int isNative)
{
	const char *fullpath = KEnvironment::getModuleFullPath(szPath);
	if (!fullpath)
		return KRESULT_ERR;

	ModuleLoader *loader = KEnvironment::createModuleLoader(fullpath, isNative != 0);
	delete []fullpath;

	if (!loader->load())
		return KRESULT_ERR;

	KEnvironment::setRootModule(loader);
	return KRESULT_OK;
}

// Runs a loaded module
KRT_API KRESULT KrtRunModule(kuint_t argc, kstring_t *argv)
{
	return KEnvironment::run(argc, argv);
}
