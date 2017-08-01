#ifndef item_consume_h__
#define item_consume_h__
#include "rtti.h"
#include "item_base.h"
#include "config_reader.h"

/*
 *	消耗品(药水)
 *  可叠加
 */
class ItemConsume : public ItemBase
{
    CnDeclareRTTI

    ItemConsume( uint32 id);
    virtual ~ItemConsume() {}

public:
    bool Init();

    uint32 GetSubType() const { return m_SubType; }
    uint32 GetCDType() const { return m_CDType; }
    uint32 GetCDTime() const { return m_CDTime; }
    uint32 GetContinuedTime() const {return m_ContinuedTime;}
    int32 GetLevel() const {return m_Level;}

    const std::vector<Data3>& GetEffectData() {return m_EffectData;}

public:
    virtual void LoadItem(const void* pInfo);
    virtual void SaveItem(void* pInfo) const;

private:
    uint32                      m_SubType;
    uint32                      m_CDType;
    uint32                      m_CDTime;
    uint32                      m_ContinuedTime;
    int32                       m_Level;
    std::vector<Data3>          m_EffectData;
};

#endif