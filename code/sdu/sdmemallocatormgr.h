#pragma once

/******************************************************************************
Copyright (C) Shanda Corporation. All rights reserved.


******************************************************************************/

#ifndef SDALLOCATORMGR_H
#define SDALLOCATORMGR_H

#include "sdmemallocator.h"
#include "sdexternalmemallocator.h"
#include "sdtype.h"
//
//enum AllocatorType
//{
//	AT_External = 0
//};


// 使用时，务必定义所使用分配器的宏，如CSDExternalMemAllocator的宏ATEXTERNAL


namespace Crown
{

#ifdef _USEMEMOBJ
	class  CSDMemAllocatorMgr
	{
	public:	
		~CSDMemAllocatorMgr(){};
		static CSDMemAllocatorMgr* Instance(){ static CSDMemAllocatorMgr oInstance; return &oInstance; }
		//bool Init(AllocatorType eAllocatorType);
		ISDMemAllocator* GetAllocator();
	protected:
		CSDMemAllocatorMgr(){};
	private:
	};
#endif //#ifdef _USEMEMOBJ

};

#endif //SDALLOCATORMGR_H
