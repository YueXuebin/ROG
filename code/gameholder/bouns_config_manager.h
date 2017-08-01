#ifndef bouns_config_manager_h_
#define bouns_config_manager_h_

#include "sign_gain_config.h"


class BounsConfigManager
{
public:
	BounsConfigManager(void);
	virtual ~BounsConfigManager(void);

	DECLARE_SINGLETON(BounsConfigManager);

	bool LoadConfig(const char* path);
    uint32 GetSignGainsMonthLength();
	uint32 GetSignGainsDayLength(uint32 monthIdx); 
	//const PASSIVE_SKILL_CARD_ROOT_TPLT_STRUCT* GetPassiveSkillCardConfig(uint32 id);
	const SIGN_GAIN_ROOT_MONTH_DAY_STRUCT* getSignGainDayStruct(uint32 monthIdx, uint32 dayIdx);
private: 
	const SIGN_GAIN_ROOT_STRUCT* m_SignGainRoot;
};

#endif // bouns_config_manager_h_