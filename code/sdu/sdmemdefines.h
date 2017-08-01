#pragma once

/*
 *	自定义内存管理
 */
#ifndef SDMEMDEFINES_H
#define SDMEMDEFINES_H

#ifdef APPLE
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

#define SD_MEM_FILE_DEFAULT 0
#define SD_MEM_LINE_DEFAULT -1
#define SD_MEM_FUNCTION_DEFAULT 0
#define SD_MEM_DEALLOC_SIZE_DEFAULT (size_t)-1

#define SD_MEM_ALIGNMENT_DEFAULT  4

#define SD_IS_POWER_OF_TWO(x)   (((x)&(x-1)) == 0)

#ifdef _USEMEMOBJ
#define SDMemObjNew new(__FILE__,__LINE__,__FUNCTION__)
#else
#define SDMemObjNew new
#endif
#define SDMemObjDelete delete

#define SDAlignOf(T) __alignof(T)
#define SDStackAlloc(T, count) (T*)_malloca(sizeof(T)*(count))
#define SDStackFree _freea

#define SDMEMASSERT(value) {CnAssert(value);}
#define SDMEMASSERTUNIT(value, unit) {CnAssert(value);}

#define SDMEMVERIFY(value) { CnVerify(value); }
#define SDMEMVERIFYUNIT(value, unit) { CnVerify(value); }

namespace Crown
{

	//---------------------------------------------------------------------------
	// SDMemEventType.
	//
	// Determine the overall allocation type. When an object is allocated with
	// an allocation routine, it should be deallocated with its corresponding
	// deallocation routine.
	//---------------------------------------------------------------------------

	enum SDMemEventType
	{
		SD_UNKNOWN = 0,                 // Unknown allocation/deallocation type.
		// This type will cause assertions in
		// the memory management interfaces.
		SD_OPER_NEW,                    // Allocated through SDMemObjNew. 
		// Should be deallocated with SDMemObjDelete
		SD_OPER_NEW_ARRAY,              // Allocated through SDMemObjNew[]. 
		// Should be deallocated with SDMemObjDelete[]
		SD_OPER_DELETE,                 // Deallocated with SDMemObjDelete.

		SD_OPER_DELETE_ARRAY,           // Deallocated with SDMemObjDelete[]

		SD_MALLOC,                      // Allocated through NiMalloc.
		// Should be deallocated with NiFree
		SD_REALLOC,                     // Allocated through NiRealloc.
		// Should be deallocated with NiFree.
		SD_ALIGNEDMALLOC,               // Allocated through NiAlignedAlloc.
		// Should be deallocated with 
		// NiAlignedFree
		SD_ALIGNEDREALLOC,              // Allocated through NiAlignedRealloc.
		// Should be deallocated with 
		// NiAlignedFree
		SD_FREE,                        // Deallocated with NiFree
		SD_ALIGNEDFREE,                 // Deallocated with NiAlignedFree
		SD_EXTERNALALLOC,               // Used to track external allocations
		SD_EXTERNALFREE                 // Used to track external frees
	};

};


#endif // SDMEMDEFINES_H
