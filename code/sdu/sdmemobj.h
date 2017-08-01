#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#ifndef SDMEMOBJECT_H
#define SDMEMOBJECT_H

#include "sdmemdefines.h"
#include "sdtype.h"

namespace Crown
{

	#ifdef _USEMEMOBJ

	class CSDMemObject
	{
	private:
		// The memory debugger uses the file, line, function 
		// routines. Therefore new and new[] should be 
		// unavailable to the outer application.
		static void* operator new(size_t stSize);
		static void* operator new[](size_t stSize);

	public:
		static void *operator new(
			size_t stSize,
			const char *pszSourceFile, 
			int32 nSourceLine, 
			const char* pszFunction);

		static void *operator new[](
			size_t stSize, 
			const char *pszSourceFile, 
			int32 nSourceLine, 
			const char* pszFunction);

		// Required for exception handling in the compiler. These 
		// should not be used in practice.
		static void operator delete(
			void*, 
			const char*, 
			int32, 
			const char*){}

		static void operator delete[](
			void*, 
			const char*, 
			int32, 
			const char*){}

	public:
		// delete and delete[] must be implemented in SD_MEMORY_DEBUGGER
		// and non-SD_MEMORY_DEBUGGER builds.
		static void operator delete(void *pvMem, size_t stElementSize);
		static void operator delete[](void *pvMem, size_t stElementSize);

		// We don't want to hide the placement new functions:
		inline static void* operator new( size_t, void* p ) { return p; }
		inline static void* operator new[]( size_t, void* p ) { return p; }

		// Conversely we also then need a placement delete function:
		inline static void operator delete( void *, void* ) {}
		inline static void operator delete[]( void *, void* ) {}
	};

#endif //#ifdef _USEMEMOBJ

};

#endif // SDMEMOBJECT_H
