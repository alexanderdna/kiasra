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

//===================================================

enum ModuleAttributes
{
	KMODA_SYSTEM = 0,
	KMODA_USER   = 1,
	KMODA_KIASRA = 2,
	KMODA_NATIVE = 4,
};

enum ClassAttributes
{
	KCA_PUBLIC  = 0,
	KCA_PRIVATE = 1,
	KCA_STATIC  = 2,
};

enum FieldAttributes
{
	KFA_PUBLIC  = 0,
	KFA_PRIVATE = 1,
	KFA_STATIC  = 2,
	KFA_FINAL   = 4,
};

enum MethodAttributes
{
	KMA_PUBLIC  = 0,
	KMA_PRIVATE = 1,
	KMA_STATIC  = 2,
	KMA_CTOR    = 4,
	KMA_NATIVE  = 8
};

/*===================================================*/

/* Creates a new execution environment */
KRESULT KRT_API KrtCreateEnvironment(void);

/* Destroys an execution environment and all its resources */
void KRT_API KrtDestroyEnvironment(void);

/* Loads a module from the specified path */
KRESULT KRT_API KrtLoadModule(kstring_t ksPath, HKMODULE *pHKModule);

/* Runs a loaded module */
KRESULT KRT_API KrtRunModule(HKMODULE hKModule);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif//_KRT_H
