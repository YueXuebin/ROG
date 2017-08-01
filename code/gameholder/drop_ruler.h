#ifndef drop_ruler_h__
#define drop_ruler_h__

#include "regular_data.h"

class ItemBase;
class RegularBuilder;
class ItemEquip;

/*
 *	副本掉落管理
 *  类别->装备->装备基础->品质
 *      ->材料
 *      ->消耗品
 *      ->石头
 */
class DropRuler
{
    friend class RegularBuilder;

    DropRuler(RegularBuilder* pReguler);
    ~DropRuler();

public:
    ItemBase* DropItem(uint32 owner_id, const RandomDropData& data);
    ItemBase* DropItem(uint32 owner_id, const REWARD_ITEM& table);

private:
    ItemBase* DropEquip(uint32 owner_id, const RandomDropData& data);           // 掉落装备(装备)
    ItemBase* DropMaterial(uint32 owner_id, const RandomDropData& data);        // 掉落材料(强化石,制作材料)
    ItemBase* DropConsume(uint32 onwer_id, const RandomDropData& data);         // 掉落消耗品(药水)
    ItemBase* DropStone(uint32 owner_id, const RandomDropData& data);           // 掉落石头(技能石,符文石,属性宝石)

private:
    RegularBuilder* m_pReguler;

    int32 test_x;
    int32 test_q;   // 测试品质

};


#endif // drop_ruler_h__
