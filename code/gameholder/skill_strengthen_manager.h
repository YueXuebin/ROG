 /*
*   强化技能和符文
*/
#ifndef skill_strengthen_manager_h__
#define skill_strengthen_manager_h__

class Player;

class ItemStone;

class SkillStrengthenManager
{
public:
    friend class ItemManager;
    ItemManager* GetItemMgr();

public:
    SkillStrengthenManager(Player* player);
    ~SkillStrengthenManager();

    void OnRecv(uint32 msgID, void* data);

private:
    void OnFuse(void* data);                       // 技能升品

    void CalcSkillProperty(uint32 equipPlace, int32 skillSlotId);

private:
     Player*             m_player;
};

#endif // passive_skill_manager_h__
