#ifndef skill_info_manager_h__
#define skill_info_manager_h__

#include "skill_info.h"
#include "item_config_manager.h"

class BattleUnit;
class Creature;
class ItemStone;
class ItemManager;
class ItemController;
struct SKILL_ROOT_SKILL_STRUCT;

/*
*   技能系统
*/
class SkillInfoManager
{
public:
    friend class ItemManager;
    friend class ItemController;
    ItemManager* GetItemMgr();

public:
    SkillInfoManager(Creature* pCreature, uint8 place, uint32 num);
    ~SkillInfoManager();

    void LoadSkillComponentInfo(const SKILL_COMPONENT_INFO& info);
    void FillSkillComponentInfo(SKILL_COMPONENT_INFO& info);

    void OnRecv(uint32 msgID, void * data);

    void ReceiveEvent( uint32 frameCount, uint32 eventId, int32 arg1, int32 arg2, int32 arg3 );

    void OnPropertyChange();            // 属性改变影响技能改变

    void AddExp(int32 val);

    bool CheckStonePosWithType(int32 pos, int32 type);
    bool TryToCorrectStonePosWithType(int32 type, int32* pos);

    static void makeSkillPropSetByBattleUnit(PropertySet* skillPropertySet, BattleUnit* battleUnit, const SKILL_ROOT_SKILL_STRUCT* skillConfig);

    void OnUseBaseSkill(void* data);                        // 使用技能

    
    SkillInfo*  GetSkillInfoBySlot(int32 skillSlotId);
    SkillInfo*  GetNormalAttackSkillInfo(uint32 skillId);

    void ReFreshSkillInfo(int32 skillSlotId);

private:
    SkillInfo*  GetSkillInfoById(uint32 skillId);           // 此接口不宜再用(可能装备相同的两个技能)

    void AddToGrid(ItemStone * pStone, bool bSwap = false);

    void OnSkillInfoReq();                                  // 请求技能信息

    uint16 OptInstall( ItemBase* pSrc,  const int32 dstPos );
    uint16 OptUninstall( int32 srcPos, int32 dstPos = -1 );
    uint16 OptSwap( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos );

    void    RemoveStone(int32 pos);

    void Clear();

    void CalcAllSkillInfo();                                // 重新计算技能属性(技能书或符文调整后都要调用)
    void CalcSkillInfoEx(SkillInfo* skillInfo, ItemStone* pRuneStone1, ItemStone* pRuneStone2);
    void CheckAura();                                       // 光环技能移除后，对应的光环buff也移除

    static void CalcDamageMul(PropertySet* skillPropertySet, real mul);

    bool CheckSkillInvalid(uint32 skillID);                               // 计算技能是否可用

    bool    IsEmptyGrid( int32 pos);
    bool    IsValidGrid(int32 pos) const;

    ItemStone* GetStoneByPos(int32 pos);

    void SendFinalSkillInfoList();
    void FillFinalSkillInfoList(FINAL_SKILL_INFO_LIST& info);

    uint32 SlotToIndex(uint32 slotId) {return slotId-1;}
    uint32 IndexToSlot(uint32 index) {return index+1;}

public:
    const uint8     m_Place;
    const int32     MAX_SKILL_NUM;
    const int32     MAX_SKILL_GRID_NUM;

private:
    Creature*       m_pCreature;                                        // 角色 
    StoneGrid       m_StoneGrid;                                        // 前6个技能书位，后12个符文位置

    SkillInfo   m_SkillInfo[SKILL_SLOT_WHOLE];
    bool        m_SkillInfoDirty[SKILL_SLOT_WHOLE];

    SkillInfo   m_NormalAttackNear;                 // 近战普攻
    SkillInfo   m_NormalAttackFar;                  // 远程普攻
    bool        m_NormalAttackDirty;


};

#endif
