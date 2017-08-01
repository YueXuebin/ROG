#ifndef item_material_h__
#define item_material_h__

#include "item_base.h"

/*
 *  材料
 *  可叠加
 */
class ItemMaterial : public ItemBase
{
    CnDeclareRTTI

public:
    ItemMaterial( uint32 id);
    virtual ~ItemMaterial() {}

public:
    bool Init();

    uint32 GetSubType() const { return m_SubType; }

public:
    virtual void LoadItem(const void* pInfo);
    virtual void SaveItem(void* pInfo) const;

private:
    uint32                      m_SubType;
};

#endif