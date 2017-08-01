#ifndef soldier_talent_manager_h__
#define soldier_talent_manager_h__


#include "item_stone.h"

class Soldier;
class ItemManager;

/*
 *	Ӷ���츳
 */
class SoldierTalentManager
{
public:
    friend class ItemManager;

    ItemManager* GetItemMgr();

public:
    SoldierTalentManager(Soldier* pSoldier);
    ~SoldierTalentManager();

    void LoadStoneInfo(const STONE_INFO& info);                     // ����Ӷ���츳��Ϣ
    void FillStoneInfo(STONE_INFO& info) const;                     // ���Ӷ���츳��Ϣ

    const StoneGrid& GetStoneGrid() const {return m_StoneGrid;}

    void Clear();

    void OnRecv(uint32 msgID, void* data);

    bool IsEmptyGrid( int32 pos);

    void CalcProp();                                            // ����װ������

    ItemStone* GetStoneByPos(int32 pos);

    bool IsValidGrid(int32 pos) const;

public:
    bool CheckStoneTypeAndPos(uint32 type, int32 pos);
    bool TryToCorrectStonePosWithType(int32 type, int32* pos);

private:
    uint16 OptInstall( ItemBase* pSrc,  const int32 dstPos );
    uint16 OptUninstall( int32 srcPos, int32 dstPos = -1 );
    uint16 OptSwap( uint32 srcPlace, int32 srcPos, uint32 dstPlace, int32 dstPos );

    void AddToGrid(ItemStone * pStone, bool bSwap = false);

private:
    Soldier*        m_pSoldier;                             // ��ɫָ��

    StoneGrid       m_StoneGrid;                            // װ���б�
    bool            m_PropertyDirty;                        // ���Է����ı�
};

#endif
