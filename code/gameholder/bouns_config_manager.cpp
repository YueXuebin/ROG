#include "gameholder_pch.h"
#include "bouns_config_manager.h"

IMPLEMENT_SINGLETON(BounsConfigManager)

BounsConfigManager::BounsConfigManager(void)
{
	SIGN_GAIN_ENTRY::CreateInstance();
}

BounsConfigManager::~BounsConfigManager(void)
{
	SIGN_GAIN_ENTRY::DestroyInstance();
}

bool BounsConfigManager::LoadConfig(const char* path)
{
	if (SIGN_GAIN_ENTRY::Instance()->LoadConfig(path))
	{
		m_SignGainRoot = &SIGN_GAIN_ENTRY::Instance()->GetStruct();
	}
	else
	{
		return false;

	}

	return true;
}

uint32 BounsConfigManager::GetSignGainsMonthLength()
{
	return m_SignGainRoot->month_list.size();
}

uint32 BounsConfigManager::GetSignGainsDayLength(uint32 monthIdx)
{
	if(monthIdx>= m_SignGainRoot->month_list.size())
		return 0;
	
	return  m_SignGainRoot->month_list[monthIdx].day_list.size();
}

const SIGN_GAIN_ROOT_MONTH_DAY_STRUCT* BounsConfigManager::getSignGainDayStruct(uint32 monthIdx, uint32 dayIdx)
{
	if(monthIdx>= GetSignGainsMonthLength())
		return nullptr;
	if(dayIdx>=GetSignGainsDayLength(monthIdx) )
		return nullptr;
	return  &m_SignGainRoot->month_list[monthIdx].day_list[dayIdx-1];
}