#ifndef item_stone_h__
#define item_stone_h__

#include "item_base.h"

/*
 *	石头(技能石,符文石,属性宝石)
 *  不可叠加,有经验和等级
 */
class ItemStone : public ItemBase
{
    CnDeclareRTTI
public:
    ItemStone(uint32 id);
    virtual ~ItemStone();
    
    virtual bool Init();

    virtual void LoadItem(const void* pInfo);
    virtual void SaveItem(void* pInfo) const;

    int32   GetSubType() const {return m_SubType;}
    int32   GetStoneFuncId() const {return m_FuncId;}

public:
    uint32 GetQuality() const { return m_Quality; }
    void SetQuality( uint32 val) { m_Quality = val; }

    int32   GetLevel() const {return m_Level;}
    void    SetLevel(int32 val);
    void    AddLevel(int32 val = 1) {SetLevel(GetLevel() + val);}
    void    SubLevel(int32 val = 1) {SetLevel(GetLevel() - val);}

    int32   GetTotalExp() const;
    int32   GetExp() const {return m_Exp;}
    void    SetExp(int32 val);
    void    AddExp(int32 val);

public:
    static void    AddExpToItem(ITEM_STONE* item, int32 exp);

private:
    int32               m_SubType;
    int32               m_FuncId;

private:
    uint32              m_Quality;          // 品质
    int32               m_Level;            // 等级
    int32               m_Exp;              // 当前等级拥有的经验值
};

typedef std::vector<ItemStone* > StoneGrid;

#endif // item_stone_h__
