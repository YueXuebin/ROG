/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#if defined __linux__
#include <malloc.h>
#elif defined __APPLE__
#include <cstdlib>
#endif

#include "Assertion.h"

inline void* CSDExternalMemAllocator::Allocate(
	size_t& stSizeInBytes,
	size_t& stAlignment,
	SDMemEventType,
	const char*,
	int32,
	const char*)
{
	SDMEMVERIFY(SD_IS_POWER_OF_TWO(stAlignment));

#if defined __linux__ || defined __APPLE__
	return malloc(stSizeInBytes);
#else
	return SDExternalAlignedMalloc(stSizeInBytes, stAlignment);
#endif
}

inline void CSDExternalMemAllocator::Deallocate(
	void* pvMemory,
	SDMemEventType, 
	size_t)
{
	if (pvMemory == nullptr)
		return;

#if defined __linux__ || defined __APPLE__
	free(pvMemory);
#else
	SDExternalAlignedFree(pvMemory);
#endif
}

inline void* CSDExternalMemAllocator::Reallocate(
	void* pvMemory,
	size_t& stSizeInBytes,
	size_t& stAlignment,
	SDMemEventType,
	size_t,
	const char*,
	int32,
	const char*)
{
	SDMEMVERIFY(SD_IS_POWER_OF_TWO(stAlignment));

	// The deallocation case should have been caught by us before in
	// the allocation functions.
	SDMEMVERIFY(stSizeInBytes != 0);

#if defined __linux__ || defined __APPLE__
	return realloc(pvMemory, stSizeInBytes);
#else
	return SDExternalAlignedRealloc(pvMemory, stSizeInBytes, stAlignment);
#endif
}

inline bool CSDExternalMemAllocator::TrackAllocate(
	const void* const,
	size_t, 
	SDMemEventType, 
	const char*,
	int32,
	const char*)
{
	return false;
}

inline bool CSDExternalMemAllocator::TrackDeallocate(
	const void* const,
	SDMemEventType)
{
	return false;
}

inline bool CSDExternalMemAllocator::SetMarker(const char*,
											   const char*, const char*)
{
	return true;
}

inline void CSDExternalMemAllocator::Initialize()
{
}

inline void CSDExternalMemAllocator::Shutdown()
{
}

inline bool CSDExternalMemAllocator::VerifyAddress(const void*)
{
	return true;
}


