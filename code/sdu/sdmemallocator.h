#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#ifndef SDALLOCATORM_H
#define SDALLOCATORM_H

#include "sdmemdefines.h"
#include "sdtype.h"

namespace Crown
{

	class  ISDMemAllocator
	{
	public:
		inline virtual ~ISDMemAllocator(){};
		ISDMemAllocator(){};
		// Encapsulate all memory management through a 
		// single set of API calls.

		// Note that the size and alignment are passed-by-reference. 
		// This allows the allocator to adjust the values
		// INT32ernally and pass the results back to the caller.
		virtual void* Allocate(
			size_t& stSizeInBytes, 
			size_t& stAlignment,
			SDMemEventType eEventType, 
			const char *pszSourceFile = SD_MEM_FILE_DEFAULT,
			int32 nSourceLine = SD_MEM_LINE_DEFAULT,
			const char* pszFunction = SD_MEM_FUNCTION_DEFAULT) =0;

		virtual void Deallocate(
			void* pvMemory, 
			SDMemEventType eEventType, 
			size_t stSizeinBytes = SD_MEM_DEALLOC_SIZE_DEFAULT) =0;

		virtual void* Reallocate(
			void* pvMemory, 
			size_t& stSizeInBytes,
			size_t& stAlignment, 
			SDMemEventType eEventType, 
			size_t stSizeCurrent = SD_MEM_DEALLOC_SIZE_DEFAULT,
			const char *pszSourceFile = SD_MEM_FILE_DEFAULT,
			int32 nSourceLine = SD_MEM_LINE_DEFAULT,
			const char* pszFunction = SD_MEM_FUNCTION_DEFAULT) =0;

		// Functions to allow tracking of memory. These functions should NOT
		// touch the input addresses in any way.
		virtual bool TrackAllocate(
			const void* const pvMemory, 
			size_t stSizeInBytes, 
			SDMemEventType eEventType, 
			const char *pszSourceFile = SD_MEM_FILE_DEFAULT,
			int32 nSourceLine = SD_MEM_LINE_DEFAULT,
			const char* pszFunction = SD_MEM_FUNCTION_DEFAULT) =0;

		virtual bool TrackDeallocate(
			const void* const pvMemory, 
			SDMemEventType eEventType) = 0;

		// Functions to allow marking of memory allocations/deallocations.
		virtual bool SetMarker(const char* pcMarkerType, const char* pszClassifier,
			const char* pszString) = 0;

		// Called by the memory manager during NiInit()
		virtual void Initialize() = 0;
		// Called by the memory manager during NiShutdown()
		virtual void Shutdown() = 0;

		// Called by the memory manager to verify a memory address
		virtual bool VerifyAddress(const void* pvMemory) = 0;
	};

};

#endif
