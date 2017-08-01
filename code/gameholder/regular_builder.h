#ifndef __regular_builder_h__
#define __regular_builder_h__

#include "regular.h"
#include "regular_data.h"

class PointBase;
class DropRuler;

/*
 *	ROG项目的关卡基类
 *  功能：通过添加PointBase来实现关卡设计功能
 */
class RegularBuilder : public Regular
{
    friend class PointBase;

    CnDeclareRTTI
public:
    RegularBuilder(RegularDoc *pDoc);
    virtual ~RegularBuilder();

    void Update(uint32 curr_tick);

    virtual void ReceiveEvent(uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3);

    uint32  GenerateValidTriggerID();

    void OnMonsterDrop(int32 nMonsterID, int32 nKillerID);
    void DropItem(const std::vector<RandomDropData>& data_list, const CoVec2& from_pos);
    void DropItem(const REWARD_TABLE& table, const CoVec2& from_pos);

    std::vector<PointBase*> GetBusinessPoints() {return m_BusinessPoints;}

    virtual bool IsEnemy(BattleUnit* attacker, BattleUnit* defencer);

private:
    void AddPoint(PointBase* point);

protected:
    std::vector<PointBase*> m_BusinessPoints;       // 所有的事件点

private:
    bool m_IsBuild;

    uint32  m_GenerateTriggerIDCounter; // 自动生成triggerID计数器

    DropRuler*  m_DropRuler;                // 掉落管理器
};

#endif // __regular_builder_h__