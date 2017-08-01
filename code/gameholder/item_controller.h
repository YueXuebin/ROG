#ifndef item_controller_h__
#define item_controller_h__

#include "config_reader.h"

class Player;
class ItemManager;
class ItemConsume;

/*
*   物品控制�?
*/
class ItemController
{
public:
    friend class ItemManager;

    ItemManager* GetItemMgr();

public:
    ItemController(Player* player);
    ~ItemController();

    void    Update(uint32 frame_count);
    void    ForceUpdate();

public:
    uint16  UseConsume(uint32 reason, ItemConsume* pConsume);

    void    AddConsumeCD(uint32 cdType, uint32 cdTime);
    bool    IsConsumeInCD(uint32 cdType) const;

    // 0代表移除
    void    AddNtf(uint32 cdType, uint32 cdTime = 0);

private:
    void    DoItemEffect(const ItemConsume* pConsume, const Data3& data);

    void    DoEffectProp(const ItemConsume* pConsume, int32 key, int32 value);
    void    DoEffectPackage(const ItemConsume* pConsume, int32 key, int32 value);
    void    DoEffectSkill(const ItemConsume* pConsume, int32 key, int32 value);
    void    DoEffectBuff(const ItemConsume* pConsume, int32 key, int32 value);

private:
    Player*     m_player;
    uint32      m_LastFrameCounter;

    std::list<TRIPLE_VALUE>    m_ConsumeCD;

    CONSUME_CD_INFO         m_ConsumeCDInfoNTF;
};


#endif // item_controller_h__
