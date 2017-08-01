#ifndef HEAD_PORTRAIT_NANAGER_H
#define HEAD_PORTRAIT_NANAGER_H

// ==================================================
/// 个性头像系统
// ==================================================

class Player;

class HeadPortraitManager
{
public:
    HeadPortraitManager(Player* player);
    ~HeadPortraitManager();

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const HEAD_PORTRAIT_INFO& info);
    void FillInfo(HEAD_PORTRAIT_INFO& info) const;

    uint32 GetCurHeadIconId(){ return m_head_portrait_info.cur; }

private:

    // 头像信息请求
    void OnSetHeadReq(void* data);
    void OnInitOpenInfoReq(void* data);

private:
    Player*             m_player;
    HEAD_PORTRAIT_INFO         m_head_portrait_info;
};

#endif