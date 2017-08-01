// 签到系统

#ifndef SIGN_MANAGER_H
#define SIGN_MANAGER_H

class Player;

class SignManager
{
public:
    SignManager(Player* player);
    ~SignManager();

    void Clear();
    void OnRecv(uint32 msgID, void* data);
    
    void LoadInfo(SIGN_INFO& info);
    SIGN_INFO&    GetSignInfo() {return m_sign_info;}
    void FillSignInfo(SIGN_INFO& info);

    void OnSignInfoReq();
    void OnSignReq();
 
private:
    void CheckSignData();
    //5分钟
    static const int64 SIGN_Refresh = 5*60;
    //一个月
    //#define CALENDAR_Refresh  60*60*24*30;
    //for test
    static const int64 CALENDAR_Refresh =60*60*24;
private:
    Player*             m_player;
    SIGN_INFO           m_sign_info;
};

#endif