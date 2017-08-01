#ifndef Assertion_h__
#define Assertion_h__

#include "Logging.h"
#include "colorlog.h"

#if defined(_DEBUG)

#define CnAssert(condition) \
do \
{ \
	if (!(condition)) \
	{ \
		LOG_E << "Assertion failed: " << #condition << std::endl \
			<< __FILE__ << ":" << __LINE__ << std::endl; \
		__CnAssert(false); \
	} \
} while (0)

#define CnVerify(f) CnAssert(f)

#define CnExit() \
    CnAssert(false); \
    exit(EXIT_FAILURE);

#else	// defined(_DEBUG)

//#define CnAssert(condition) ((void)0)
#define CnAssert(condition) \
do \
{ \
    if(!(condition)) \
    { \
        CnError("Assertion %s:%d\n", __FILE__, __LINE__); \
    } \
} while(0)

#define CnVerify(f) ((void)(f))

#define CnExit()    exit(EXIT_FAILURE)

#endif	// defined(_DEBUG)

#endif // Assertion_h__
