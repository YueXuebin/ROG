#ifndef regular_rog_dungeon_h__
#define regular_rog_dungeon_h__

#include "regular_single.h"

/*
 *	ROG项目的副本关卡基类
 */
class RegularROGDungeon : public RegularSingle
{
    CnDeclareRTTI
public:
    RegularROGDungeon(RegularDoc *pDoc);
    virtual ~RegularROGDungeon();

    virtual void Update(uint32 curr_tick);
    virtual void OneSecondUpdate(int32 deltatime);
    virtual void OnCreatureEnter(BattleUnit* creature);
    virtual void OnCreatureLeave(BattleUnit* creatrue);

    virtual void SceneClear(bool success, std::vector<std::string>* arglist = NULL);     // 场景通关

private:
    bool m_doFirstSend;          // 标记是否给客户端发送过当前场景怪的id
    std::vector<uint32> m_MonsterIdList;
};

#endif // regular_rog_dungeon_h__
