#ifdef KRT_EXPORTS
#define KRT_API __declspec(dllexport)
#else
#define KRT_API __declspec(dllimport)
#endif
