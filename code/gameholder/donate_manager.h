#ifndef donate_manager_h__
#define donate_manager_h__

#include "donate_config.h"

class Player;

class DonateManager
{

public:
    DonateManager(Player * player);
    ~DonateManager();

    void LoadDonateInfo(const DONATE_INFO& info);
    void FillDonateInfo(DONATE_INFO& info);

    void OnRecv(uint32 msgID, void* data);

    void onMoneyPointAck(const std::string& str);
private:
    void onDonateInfo();
    void onDonatePut(void* data);
    void onRewardReq();
    void onRewardMoneypoint(void* data);

    uint32 getTimer();

    void sendDanateInfoAck();
private:
    Player * m_player;

    DONATE_INFO m_donate_info;
};

#endif