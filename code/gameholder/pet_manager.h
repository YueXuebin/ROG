#ifndef pet_manager_h__
#define pet_manager_h__

class Pet;
class Player;
class Soldier;

typedef std::vector<Pet*> PetList;

/*
 *	宠物管理器
 */
class PetManager
{
public:
    PetManager(Player* pPlayer);
    ~PetManager();

    void    LoadPetInfo(const PET_INFO& info);
    void    FillPetInfo(PET_INFO& info) const;

    void    OnEndSet();

    void    OnRecv(uint32 msgID, void* data);

    void    AddExp(int32 val);

    void    CalcPetProperty();

    PetList&    GetPetList() {return m_PetList;}

    Soldier*    GetSoldier() {return m_pSoldier;}

    uint32  GetSoldierObjectID();

    void    Update(uint32 frame_count);

private:
    void    OnSoldierEnableReq(void* data);


private:
    Player*     m_pPlayer;

    Soldier*    m_pSoldier;

    PetList     m_PetList;          // 所有宠物
};

#endif // pet_manager_h__
