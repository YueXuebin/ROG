#include "SDuPCH.h"
#include "sdmemallocatormgr.h"
#include "sdutil.h"

//CSDMemAllocatorMgr::CSDMemAllocatorMgr()
//{
//	m_poAllocator = nullptr;
//}
//
//CSDMemAllocatorMgr::~CSDMemAllocatorMgr()
//{
//	SDSAFE_DELETE m_poAllocator;
//	m_poAllocator = nullptr;
//}
//
//bool CSDMemAllocatorMgr::Init(AllocatorType eAllocatorType)
//{	
//	switch(eAllocatorType)
//	{
//	case AT_External:
//		m_poAllocator = SDSAFE_NEW CSDExternalMemAllocator();
//		break;
//	default:
//		m_poAllocator = SDSAFE_NEW CSDExternalMemAllocator();
//		break;
//	}
//	return true;
//}

namespace Crown
{

#ifdef _USEMEMOBJ

#ifdef ATEXTERNAL
	static CSDExternalMemAllocator oSDExternalMemAllocator;
#else

#endif

	ISDMemAllocator* CSDMemAllocatorMgr::GetAllocator()
	{
#ifdef ATEXTERNAL 
		return &oSDExternalMemAllocator;
#else
#error "Please define one MemAllocator!"
#endif //#ifdef ATEXTERNAL
	}

#endif //#ifdef _USEMEMOBJ

};

