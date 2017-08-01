#ifndef pet_controller_h__
#define pet_controller_h__

class BattleUnit;
class Scene;
class Pet;
class Dungeon;

typedef std::vector<Pet*> PetList;

/*
 *	宠物控制器,(管理宠物对象)
 */
class PetController
{
public:
    PetController(BattleUnit* owner);
    ~PetController();

    void KillAllPet();            // 杀死所有宠物 

    void onEnterScene(Scene* pScene);
    void onLeaveScene(Scene* pScene);

    void OnEnterDungeon(Dungeon* dungeon, uint32 dungeonID);

    void OnDead(BattleUnit* killer);
    void OnDestory();

    void ReceiveEvent( uint32 eventId, uint32 arg1, uint32 arg2);

    uint32 GetPetNumber() const {return m_PetList.size();}

    void AddPet(Pet* pPet);
    void RemovePet(uint32 id);

    void SetTeamID(uint64 id);  // 设置所有宠物的teamID

private:
    BattleUnit* m_Owner;

    PetList m_PetList;
};



#endif // pet_controller_h__
