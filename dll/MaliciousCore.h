#pragma once
#include <Windows.h>

#ifdef MALICIOUSCORE_EXPORTS
#define MALICIOUSCORE_API __declspec(dllexport)
#else
#define MALICIOUSCORE_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif

	MALICIOUSCORE_API BOOL ExecutePayload();

#ifdef __cplusplus
}
#endif
