#ifndef game_common_setting_h__
#define game_common_setting_h__

class GameCommonSetting
{
public:
    GameCommonSetting(void);
    ~GameCommonSetting(void);

    DECLARE_SINGLETON(GameCommonSetting);

    bool LoadConfig(const char* fileName);

    std::string GetPayPoitLotterStartTime(){return m_pay_point_lotter_start_time;}
    std::string GetPayPoitLotterEndTime(){return m_pay_point_lotter_end_time;}

    std::string GetDayInvestmentsStartTime(){return m_day_investments_start_time;}
    std::string GetDayInvestmentsEndTime(){return m_day_investments_end_time;}

    const GAME_SETTING_DONATE_CONFIG& GetDonateTmpl(){return m_donate_config;}

    const VIP_STATISTICS_CONFIG& GetVipStatisticsTmpl(){return m_vip_statistics_config;}

    const GAME_SETTING_VOID_FIELD_MULTI_CONFIG GetVoidFieldMultiCfg() {return m_voidFieldMultiCfg;}
    const GAME_SETTING_VOID_FIELD_SOLO_CONFIG GetVoidFieldSoloCfg() {return m_voidFieldSoloCfg;}

private:
    std::string m_pay_point_lotter_start_time;//���˴�ת�̵Ŀ�ʼʱ��ͽ���ʱ��
    std::string m_pay_point_lotter_end_time;

    std::string m_day_investments_start_time;//��򾫻�Ͷ�����ʱ��
    std::string m_day_investments_end_time;

    GAME_SETTING_DONATE_CONFIG m_donate_config;     // ��������

    VIP_STATISTICS_CONFIG m_vip_statistics_config;  // vip����

    GAME_SETTING_VOID_FIELD_MULTI_CONFIG m_voidFieldMultiCfg;
    GAME_SETTING_VOID_FIELD_SOLO_CONFIG m_voidFieldSoloCfg;
};

#endif
