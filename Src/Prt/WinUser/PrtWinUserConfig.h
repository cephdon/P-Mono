/**
* \file PrtConfigWinUser.h
* \brief Defines the Windows user configurations.
*/
#ifndef PRTCONFIG_WINUSER_H
#define PRTCONFIG_WINUSER_H

#ifdef __cplusplus
extern "C"{
#endif

	/** "unsafe" string functions are used safely. Allows for portability of code between operating systems. */
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif

	/** Calling convention */
#define PRT_CALL_CONV __cdecl

	/** Linking method */
#ifdef PRT_API_IMPL
#define PRT_API __declspec(dllexport)
#else
#ifdef PRT_STATIC
#define PRT_API
#else
#ifdef _DLL
#define PRT_API __declspec(dllimport)
#else
#define PRT_API
#endif
#endif
#endif

#ifdef PRT_DEBUG
#ifndef _DEBUG
#define _DEBUG
#endif

#define _CRTDBG_MAP_ALLOC

#include <stdlib.h>
#include <malloc.h>
#include <crtdbg.h>
#include <sal.h>
#include <stddef.h>
#include <windows.h>
#include <stdio.h>

	//#define PrtMalloc(size) malloc(size)
	//#define PrtCalloc(nmemb, size) calloc(nmemb, size)
#define PRT_DBG_ASSERT(condition, message) PrtAssert((condition), (message))
#define PRT_DBG_START_MEM_BALANCED_REGION { _CrtMemState prtDbgMemStateInitial, prtDbgMemStateFinal, prtDbgMemStateDiff; _CrtMemCheckpoint(&prtDbgMemStateInitial);
#define PRT_DBG_END_MEM_BALANCED_REGION _CrtMemCheckpoint(&prtDbgMemStateFinal); PrtAssert(!_CrtMemDifference(&prtDbgMemStateDiff, &prtDbgMemStateInitial, &prtDbgMemStateFinal), "Memory leak"); }

#else

#include <sal.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <malloc.h>

#endif

#ifdef PRT_USE_IDL
#include "PrtBaseTypes_IDL.h"
#else
	/** PRT uses these definitions for boolean values */
	typedef enum PRT_BOOLEAN
	{
		PRT_FALSE = 0,   /**< 0 means false */
		PRT_TRUE = 1    /**< 1 means true  */
	} PRT_BOOLEAN;

	/** PRT_UINT8 is always an 8-bit unsigned integer. */
	typedef unsigned __int8  PRT_UINT8;
	/** PRT_UINT16 is always a 16-bit unsigned integer. */
	typedef unsigned __int16 PRT_UINT16;
	/** PRT_UINT32 is always a 32-bit unsigned integer. */
	typedef unsigned __int32 PRT_UINT32;
	/** PRT_UINT64 is always a 64-bit unsigned integer. */
	typedef unsigned __int64 PRT_UINT64;

	/** PRT_INT8 is always an 8-bit signed integer. */
	typedef signed __int8  PRT_INT8;
	/** PRT_INT16 is always a 16-bit signed integer. */
	typedef signed __int16 PRT_INT16;
	/** PRT_INT32 is always a 32-bit signed integer. */
	typedef signed __int32 PRT_INT32;
	/** PRT_INT64 is always a 64-bit signed integer. */
	typedef signed __int64 PRT_INT64;

	/** PRT_CHAR is always an ASCII character. */
	typedef char PRT_CHAR;
	/** PRT_STRING is always an array of ASCII characters. */
	typedef char * PRT_STRING;
	/** PRT_CSTRING is always a constant array of ASCII characters. */
	typedef char const * PRT_CSTRING;

#endif
	/** PRT_RECURSIVE_MUTEX identifies a recursive mutex. */
	typedef HANDLE PRT_RECURSIVE_MUTEX;

	/** Function for Assertion will be called whenever an assertion is checked */
	typedef void(PRT_CALL_CONV * PRT_ASSERT_FUN)(PRT_INT32, PRT_CSTRING);

	/** Function for printing string, will be invoked whenever print statement is called from the runtime */
	typedef void(PRT_CALL_CONV * PRT_PRINT_FUN)(PRT_CSTRING);

	/* declare the function to assert function */
	extern PRT_ASSERT_FUN PrtAssert;

	/* declare the function to print fucntion*/
	extern PRT_PRINT_FUN PrtPrintf;

	/**
	* Creates a fresh unnamed and unlocked recursive mutex. The mutex must be unlocked by a thread as many times as it was locked.
	* @return A configuration-specific value identifying the mutex.
	* @see PrtDestroyMutex
	* @see PrtLockMutex
	* @see PrtUnlockMutex
	*/
	PRT_API PRT_RECURSIVE_MUTEX PRT_CALL_CONV PrtCreateMutex();

	/**
	* Allows the system to dispose of this mutex. Destroy must be called at most once per mutex, and a destroyed mutex never be used again.
	* @param[in] mutex A mutex that has been created, but has not yet been released.
	* @see PrtCreateMutex
	* @see PrtLockMutex
	* @see PrtUnlockMutex
	*/
	PRT_API void PRT_CALL_CONV PrtDestroyMutex(_In_ PRT_RECURSIVE_MUTEX mutex);

	/**
	* Blocks until the mutex is locked. If the locking thread already owns the mutex, then succeeds and increments the lock count.
	* @param[in] mutex The mutex to lock.
	* @see PrtUnlockMutex
	* @see PrtCreateMutex
	* @see PrtDestroyMutex
	*/
	PRT_API void PRT_CALL_CONV PrtLockMutex(_In_ PRT_RECURSIVE_MUTEX mutex);

	/**
	* Unlocks a locked mutex. Should not be called more times than the mutex has been locked.
	* @param[in] mutex The mutex to unlock.
	* @see PrtLockMutex
	* @see PrtCreateMutex
	* @see PrtDestroyMutex
	*/
	PRT_API void PRT_CALL_CONV PrtUnlockMutex(_In_ PRT_RECURSIVE_MUTEX mutex);

	/**
	* Calls system-specific implementation of malloc.
	* Fails eagerly if memory cannot be allocated.
	* @param[in] size Number of bytes to allocate.
	* @returns A pointer to a memory location
	* @see PrtFree
	*/
	PRT_API void * PRT_CALL_CONV PrtMalloc(_In_ size_t size);

	/**
	* Calls system-specific implementation of calloc.
	* Fails eagerly if memory cannot be allocated.
	* @param[in] nmemb Number of bytes to allocate per member.
	* @param[in] size Number of bytes to allocate per member.
	* @returns A pointer to a memory location
	* @see PrtFree
	*/
	PRT_API void * PRT_CALL_CONV PrtCalloc(_In_ size_t nmemb, _In_ size_t size);

	/**
	* Calls system-specific implementation of realloc.
	* Fails eagerly if memory cannot be allocated.
	* @param[in,out] ptr A pointer to a memory block to reallocate.
	* @param[in] size Number of bytes to reallocate per member.
	* @returns A pointer to a memory location or NULL if size = 0
	* @see PrtFree
	*/
	void * PRT_CALL_CONV PrtRealloc(_Inout_ void * ptr, _In_ size_t size);

	/**
	* Calls system-specific implementation of free.
	* @param[in,out] ptr A pointer to a memory block to be freed.
	* @see PrtMalloc
	* @see PrtCalloc
	* @see PrtRealloc
	*/
	PRT_API void PRT_CALL_CONV PrtFree(void * ptr);

	/** Nondeterministic Boolean choice
	* @returns A nondeterministic Boolean value.
	*/
	PRT_API PRT_BOOLEAN PRT_CALL_CONV PrtChoose();

#ifdef __cplusplus
}
#endif

#endif
