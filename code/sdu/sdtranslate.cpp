#include "SDuPCH.h"
#include "sdtranslate.h"

#if SDU_WITH_LIBICONV
namespace Crown
{
	CSDTranslate::CSDTranslate()
	{
		m_hHandle = nullptr ;
	}

	CSDTranslate::~CSDTranslate()
	{
		if (m_hHandle)
			iconv_close(m_hHandle);
	}

	bool CSDTranslate::Init(const char *cpFromCharset,const char *cpToCharset)
	{
		m_strFromCharset = cpFromCharset ;
		m_strToCharset   = cpToCharset ;
		bool bResult = false ;
		m_hHandle = iconv_open(cpToCharset, cpFromCharset);
		if ((iconv_t)-1 == m_hHandle) 
			printf("iconv open error!\n");
		else
			bResult = true ;
		return bResult ;
	}

	size_t CSDTranslate::Translate(char *src, size_t srcLen, char *desc, size_t descLen)
	{
		if ((iconv_t)-1 != m_hHandle)
		{
			char **inbuf = &src;
			char **outbuf = &desc;
			memset(desc, 0, descLen);
#ifdef __linux__
			return iconv(m_hHandle, (char**)inbuf, &srcLen, outbuf, &descLen);
#else
			return iconv(m_hHandle, (const char**)inbuf, &srcLen, outbuf, &descLen);
#endif
		}
		return -1 ;
	}
	
	const char * SDAPI SDGetLocaleCharset (void)
	{
		return locale_charset() ;
	}

}

#endif	// SDU_WITH_LIBICONV
