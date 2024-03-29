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
	#ifdef KRT_EXPORTS
		#define KRT_API __attribute__ ((visibility("default")))
	#else
		#define KRT_API
	#endif
#endif

/*===================================================*/

/* Handle of module */
typedef void * HKMODULE;

/* Handle of exception */
typedef void * HKEXCEPTION;

/*===================================================*/

typedef enum KMODULEATTRIBUTES
{
	KMODA_SYSTEM = 0,
	KMODA_USER   = 1,
	KMODA_KIASRA = 2,
	KMODA_NATIVE = 4,
} KMODULEATTRIBUTES;

typedef enum KCLASSATTRIBUTES
{
	KCA_PUBLIC  = 0,
	KCA_PRIVATE = 1,
	KCA_STATIC  = 2,
} KCLASSATTRIBUTES;

typedef enum KFIELDATTRIBUTES
{
	KFA_PUBLIC  = 0,
	KFA_PRIVATE = 1,
	KFA_STATIC  = 2,
	KFA_FINAL   = 4,
} KFIELDATTRIBUTES;

typedef enum KMETHODATTRIBUTES
{
	KMA_PUBLIC  = 0,
	KMA_PRIVATE = 1,
	KMA_STATIC  = 2,
	KMA_CTOR    = 4,
	KMA_NATIVE  = 8
} KMETHODATTRIBUTES;

/*===================================================*/

/* Creates a new execution environment */
KRT_API KRESULT KrtCreateEnvironment(void);

/* Destroys an execution environment and all its resources */
KRT_API void KrtDestroyEnvironment(void);

/* Loads a module from the specified path */
KRT_API KRESULT KrtLoadModule(const char *szPath, int isNative);

/* Runs a loaded module */
KRT_API KRESULT KrtRunModule(kuint_t argc, kstring_t *argv);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif//_KRT_H
