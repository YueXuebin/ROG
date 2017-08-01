#include "SDuPCH.h"
#include "sdutf8.h"
#include "sdtranslate.h"
#if (defined(WIN32) || defined(WIN64))
#include <Windows.h>
#else

#endif

namespace Crown
{
#define SD_UTF8_FLAG           "UTF-8"
#define SD_MAX_BUFF_LEN        65525

	SDHANDLE SDAPI SDUTF8FileOpen( const char* cpFilename, const char* cpMode )
	{
		FILE * hFile = fopen(cpFilename,cpMode) ;
		if (hFile)
		{
			char szBOM[4] = {(char)0xEF, (char)0xBB, (char)0xBF, 0};   
			fprintf(hFile, "%s", szBOM);  
		}
		return (SDHANDLE)hFile ;
	}

	bool SDAPI SDUTF8FileClose( const SDHANDLE hFile )
	{
		if (hFile)
		{
			return fclose((FILE*)hFile) == 0 ? true : false ;
		}
		return false ;
	}

	size_t SDAPI SDUTF8FileWrite( const void* pBuffer ,size_t nSize , size_t nCount,FILE* hFile)

	{
		size_t nResult = - 1 ;
		if (hFile)
		{
			if ( SDIsUTF8 ( pBuffer , nCount * nSize ))
			{
				nResult = fprintf((FILE*)hFile, "%s", (char*)pBuffer);   
			}
			else
			{
#if SDU_WITH_LIBICONV
				CSDTranslate tran ;
				if (tran.Init(SDGetLocaleCharset(),SD_UTF8_FLAG))
				{
					char szData[SD_MAX_BUFF_LEN] = {0} ;
					size_t nTranslate = tran.Translate((char*)pBuffer,nCount* nSize,szData,SD_MAX_BUFF_LEN) ;
					if ( nTranslate >= 0 )
					{
						nResult = fprintf((FILE*)hFile, "%s", szData);   
					}
				}
#endif
			}

		}
		return nResult ;
	}

	bool SDAPI SDIsUTF8(const void* pBuffer, uint32 nSize)
	{
		bool bIsUTF8 = true;
		byte * cpStart = (byte*)pBuffer;
		byte * cpEnd = (byte*)pBuffer + nSize;
		while (cpStart < cpEnd)
		{
			if (*cpStart < 0x80) // (10000000): 值小于0x80的为ASCII字符
			{
				cpStart++;
			}
			else if (*cpStart < (0xC0)) // (11000000): 值介于0x80与0xC0之间的为无效UTF-8字符
			{
				bIsUTF8 = false;
				break;
			}
			else if (*cpStart < (0xE0)) // (11100000): 此范围内为2字节UTF-8字符
			{
				if (cpStart >= cpEnd - 1) 
					break;
				if ((cpStart[1] & (0xC0)) != 0x80)
				{
					bIsUTF8 = false;
					break;
				}
				cpStart += 2;
			} 
			else if (*cpStart < (0xF0)) // (11110000): 此范围内为3字节UTF-8字符
			{
				if (cpStart >= cpEnd - 2) 
					break;
				if ((cpStart[1] & (0xC0)) != 0x80 || (cpStart[2] & (0xC0)) != 0x80)
				{
					bIsUTF8 = false;
					break;
				}
				cpStart += 3;
			} 
			else
			{
				bIsUTF8 = false;
				break;
			}
		}
		return bIsUTF8;
	}

}
