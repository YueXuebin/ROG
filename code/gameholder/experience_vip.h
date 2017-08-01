/*
 *	体验VIP，一次性
 */
#ifndef __EXPERIENCE_VIP_H__
#define __EXPERIENCE_VIP_H__

class Player;

class ExperienceVIP
{
public:
    enum ExperienceVIPState
    {
        UNCLAIMED,
        RECEIVING,
        RECEIVED,
        EXPIRED,
        END,
    };

    ExperienceVIP(Player *player);

    ~ExperienceVIP();

    bool IsExperienceVIP(VIP_INFO &info);

    void FillExperienceVIPInfo(VIP_INFO &info);

    void ActiveExperienceVIP(VIP_INFO &info);

    void OnLoadVIPInfo(VIP_INFO &info);

    void OnFillVIPInfo(VIP_INFO &info);

    void OnReceiveReq(VIP_INFO &info);

    void OnExpireReq(VIP_INFO &info);

    void OnEndReq(VIP_INFO &info);

    void OnStateReq(VIP_INFO &info);



private:
    void SendStateAck(VIP_INFO &info);

    void CalExperienceVIPState(VIP_INFO &info);

    int32 CalVIPLevel();

private:
    Player *m_player;
};

#endif