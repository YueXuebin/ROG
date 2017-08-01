#ifndef item_h__
#define item_h__
#include "rtti.h"

class ItemBase
{
    CnDeclareRootRTTI(ItemBase)

    ItemBase( uint32 id, uint32 itemType);
    virtual ~ItemBase();
public:
    virtual bool Init() = 0;
    virtual void LoadItem(const void* pInfo) = 0;
    virtual void SaveItem(void* pInfo) const = 0;

public:
    void LoadBase(const ITEM_BASE& info);
    void SaveBase(ITEM_BASE& info) const;

    uint32  GetId() const { return m_Id; }
    uint32  GetItemType() const { return m_ItemType; }
    int32   GetMaxStackNum() const { return m_MaxStackNum; }

public:
    uint8 GetPlace() const { return m_Place; }
    void SetPlace(uint8 val) { m_Place = val; }

    int32 GetPos() const { return m_Pos; }
    void SetPos( int32 val) { m_Pos = val; }

    int32 GetNum() const { return m_Num; }
    void SetNum( int32 val);
    void AddNum( int32 val = 1) { SetNum(GetNum() + val); }
    void SubNum( int32 val = 1) { SetNum(GetNum() - val); }

    int32   GetCanStackNum() const {return GetMaxStackNum() - GetNum();}

public:
    static int32                s_ItemObjectNum;

protected:
    uint32                      m_Id;               // ID
    uint32                      m_ItemType;         // 类型
    int32                       m_MaxStackNum;      // 最大堆叠上限

    uint8                       m_Place;            //
    int32                       m_Pos;              // 位置
    int32                       m_Num;
};


typedef std::vector<ItemBase*> ItemGrid;
typedef std::list<ItemBase*> ItemList;

#endif