#pragma once

#ifndef SDU_UTIL_H
#define SDU_UTIL_H
/**
* @file sdutil.h
* @author edeng_2000
* @brief sdu 版本信息
*
**/
#include "sdtime.h"

#define   SDSAFE_NEW            new 
#define   SDSAFE_DELETE         delete 
#define   SDTRY_BEGIN	\
		  try {
#define   SDTRY_END \
		  } \
		  catch(...) \
		  { \
			char cpLine[256];	\
			FILE * fp = fopen("sdexception.log", "a+"); \
			if(fp) \
			{ \
				char cpTempLine[128] = {0}; \
				CSDDateTime ti = SDNow(); \
				sprintf(cpTempLine, "[%s]", ti.ToString("").c_str()); \
				fputs(cpTempLine, fp); \
				fputs(__FILE__, fp); \
				sprintf(cpLine, "(%d): %s", __LINE__, __FUNCTION__); \
				fputs(cpLine, fp); \
				fputs("\r\n", fp); \
				fclose(fp); \
			} \
		  }

#endif
