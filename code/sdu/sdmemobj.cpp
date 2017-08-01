#include "SDuPCH.h"
#include "sdmemobj.h"
#include "sdmemallocatormgr.h"

namespace Crown
{

#ifdef _USEMEMOBJ

	void* CSDMemObject::operator new(size_t)
	{
		return nullptr;
	}

	void* CSDMemObject::operator new[](size_t)
	{
		return nullptr;
	}

	void* CSDMemObject::operator new(
		size_t stSizeInBytes, 
		const char* pszSourceFile, 
		int32 nSourceLine, 
		const char* pszFunction)
	{		
		if (stSizeInBytes == 0)
			stSizeInBytes = 1;

		size_t	stAlign = SD_MEM_ALIGNMENT_DEFAULT;
		void* p = CSDMemAllocatorMgr::Instance()->GetAllocator()->Allocate(stSizeInBytes
			, stAlign
			, SD_OPER_NEW
			, pszSourceFile
			, nSourceLine
			, pszFunction);

		return p;
	}

	void* CSDMemObject::operator new[](
		size_t stSizeInBytes,
		const char* pszSourceFile, 
		int32 nSourceLine, 
		const char* pszFunction)
	{

		if (stSizeInBytes == 0)
			stSizeInBytes = 1;

		size_t	stAlign = SD_MEM_ALIGNMENT_DEFAULT;
		void* p = CSDMemAllocatorMgr::Instance()->GetAllocator()->Allocate(stSizeInBytes
			, stAlign
			, SD_OPER_NEW_ARRAY
			, pszSourceFile
			, nSourceLine
			, pszFunction);        

		return p;
	}


	void CSDMemObject::operator delete(void* pvMem, size_t stElementSize)
	{
		if (pvMem)
		{
			CSDMemAllocatorMgr::Instance()->GetAllocator()->Deallocate(pvMem, SD_OPER_DELETE, stElementSize);
		}
	}

	void CSDMemObject::operator delete[](void* pvMem, size_t)
	{
		if (pvMem)
		{
			CSDMemAllocatorMgr::Instance()->GetAllocator()->Deallocate(pvMem, SD_OPER_DELETE_ARRAY);
		} 
	}

#endif //#ifndef _USEMEMOBJ

};
