#pragma once
#ifdef VL_PLATFORM_WINDOWS
	#ifdef VL_BUILD_DLL
		#define VIOLET_API __declspec(dllexport)
	#else
		#define VIOLET_API __declspec(dllimport)
	#endif
#else
	#error Violet only support Windows!
#endif

#ifdef VL_ENABLE_ASSERT
	#define VL_ASSERT(x, ...) { if(!(x)) {VL_ERROR("Assertion Failed: {0}"), __VA_ARGS__); debugbreak();} }
	#define VL_CORE_ASSERT(x, ...) { if(!(x)) {VL_CORE_ERROR("Assert Failed: {0}", __VA_ARGS__); debugbreak();}}
#else
	#define VL_ASSERT(x, ...)
	#define VL_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)