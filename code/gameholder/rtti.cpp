#include "gameholder_pch.h"
#include "rtti.h"

CnRTTI::CnRTTI(const char* pcName, const CnRTTI* pkBaseRTTI) :
m_pcName(pcName), m_pkBaseRTTI(pkBaseRTTI)
{
}

bool CnRTTI::CopyName(char* acNameBuffer, unsigned int uiMaxSize) const
{
	const char* pcName = GetName();
	if (!pcName || !acNameBuffer)
	{
		Crown::SDStrSafeCpy(acNameBuffer, uiMaxSize, "\0");
		return false;
	}

	Crown::SDStrSafeCpy(acNameBuffer, uiMaxSize, pcName);
	return true;
}