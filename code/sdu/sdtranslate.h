#pragma once

#ifndef SDU_TRANSLATE_H
#define SDU_TRANSLATE_H
/**
* @file sdtranslate.h
* @author edeng_2000
* @brief sdu 版本信息
*
**/

#include "sdmacros.h"
#include "sdtype.h"
#include <string>

#if SDU_WITH_LIBICONV
#include "iconv.h"
#include "localcharset.h"
namespace Crown
{
	class CSDTranslate
	{
	public:
		CSDTranslate();
		
		~CSDTranslate();


	public:
		size_t Translate(char *cpSrc, 
						 size_t nSrcLen, 
						 char * cpDesc,
						 size_t nDescLen ) ;
		bool   Init(const char *cpFromCharset,const char *cpToCharset);
	private:
		iconv_t    m_hHandle;
		string	   m_strFromCharset ;
		string	   m_strToCharset ;
	};


	const char * SDAPI SDGetLocaleCharset (void);
}



#endif	// SDU_WITH_LIBICONV

#endif	// SDU_TRANSLATE_H

