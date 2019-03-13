#if (defined(_MSC_VER) || defined(__CYGWIN__) || defined(__MINGW32__))
	#if defined(VKT_EXPORTS)
		#define VKT_DECLSPEC __declspec(dllexport)
	#elif defined(HIVE_STATIC_LIBRARY)
		#define VKT_DECLSPEC
	#else
		#define VKT_DECLSPEC __declspec(dllimport)
	#endif
#else
	#define VKT_DECLSPEC
#endif