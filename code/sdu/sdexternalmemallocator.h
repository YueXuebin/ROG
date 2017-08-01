#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#ifndef SDEXTERNALMEMALLOCATOR_H
#define SDEXTERNALMEMALLOCATOR_H

#include "sdmemallocator.h"
#include "sdmemdefines.h"
#include "sdtype.h"

#define SDExternalAlignedMalloc _aligned_malloc
#define SDExternalAlignedRealloc _aligned_realloc
#define SDExternalAlignedFree _aligned_free

namespace Crown
{

	class CSDExternalMemAllocator : public ISDMemAllocator
	{
	public:
		// Note that the size and alignment are passed-by-reference. 
		// This allows the allocator to adjust the values
		// INT32ernally and pass the results back to the caller.
		virtual void* Allocate(
			size_t& stSizeInBytes, 
			size_t& stAlignment,
			SDMemEventType eEventType, 
			const char *pszSourceFile = SD_MEM_FILE_DEFAULT,
			int32 nSourceLine = SD_MEM_LINE_DEFAULT,
			const char* pszFunction = SD_MEM_FUNCTION_DEFAULT);

		virtual void Deallocate(
			void* pvMemory, 
			SDMemEventType eEventType, 
			size_t stSizeinBytes = SD_MEM_DEALLOC_SIZE_DEFAULT);

		virtual void* Reallocate(
			void* pvMemory, 
			size_t& stSizeInBytes,
			size_t& stAlignment, 
			SDMemEventType eEventType, 
			size_t stSizeCurrent = SD_MEM_DEALLOC_SIZE_DEFAULT,
			const char *pszSourceFile = SD_MEM_FILE_DEFAULT,
			int32 nSourceLine = SD_MEM_LINE_DEFAULT,
			const char* pszFunction = SD_MEM_FUNCTION_DEFAULT);

		// Functions to allow tracking of memory. These functions should NOT
		// touch the input addresses in any way.
		virtual bool TrackAllocate(
			const void* const pvMemory, 
			size_t stSizeInBytes, 
			SDMemEventType eEventType, 
			const char *pszSourceFile = SD_MEM_FILE_DEFAULT,
			int32 nSourceLine = SD_MEM_LINE_DEFAULT,
			const char* pszFunction = SD_MEM_FUNCTION_DEFAULT);

		virtual bool TrackDeallocate(
			const void* const pvMemory, 
			SDMemEventType eEventType);

		// Functions to allow marking of memory allocations/deallocations.
		virtual bool SetMarker(const char* pcMarkerType, const char* pszClassifier,
			const char* pszString);

		// Called by the memory manager during NiInit()
		virtual void Initialize();
		// Called by the memory manager during NiShutdown()
		virtual void Shutdown();

		// Called by the memory manager to verify a memory address
		virtual bool VerifyAddress(const void* pvMemory);
	};


	//---------------------------------------------------------------------------
	// Inline include
#include "sdexternalmemallocator.inl"
	//---------------------------------------------------------------------------

};

#endif // SDEXTERNALMEMALLOCATOR_H
