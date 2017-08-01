#ifndef drop_object_h__
#define drop_object_h__

#include "sceneobject.h"

typedef struct DropData
{
    CoVec2 from_pos;                    // 掉落物品抛物线开始位置
    CoVec2 pos;                         // 掉落物品抛物线结束位置
    uint32 owner_id;                    // 掉落物品的所有者id
    uint32 lifetime;                    // 掉落物品的持续时间(ms毫秒)

    REWARD_ITEM reward_item;            // 掉落的物品

    void clear()
    {
        owner_id = 0;
        lifetime = 10000;       
        reward_item.Clear();
    }

}DropData;

class ItemBase;
/*
* 掉落物品
*/
class DropObject : public SceneObject
{
    CnDeclareRTTI
public:
    DropObject(void);
    virtual ~DropObject(void);

    void Init(const DropData& dropdata, ItemBase* pItemBase);

    virtual void Update(uint32 frame_count);
    virtual void OnDestory() {}

    void SetLifeTime(uint32 time) {m_LifeTime = time;}      // 掉落物品的存在时间(ms)

    void DestroyDropObject();                                  //销毁本身

    const CoVec2& GetFromPos() {return m_FromPos;}

    uint32 GetPassTime() {return m_PassTime;}
    uint32 GetLifeTime() {return m_LifeTime;}

    uint32 GetOwnerID() {return m_OwnerID;}

    ItemBase* ExtractItem();            // 抽取走掉落物品
    void TransferItemOwnership();       // 转移物品指针的所有权

    const ItemBase* GetItem() {return m_pItemBase;}

private:
    CoVec2  m_FromPos;

    uint32  m_PassTime;             // 已经经过的时间(ms)
    uint32  m_LifeTime;             // 存在时长(ms)

    uint32 m_OwnerID;               // 物品主人(如果==0，表示无主人，就人人都可以捡)

    ItemBase*   m_pItemBase;        // 掉落的物品


};


#endif