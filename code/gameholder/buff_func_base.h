/*
 *	buff 功能基类
 */
#ifndef buff_func_base_h__
#define buff_func_base_h__

#include "battle_define.h"

class Buff;
class BuffData;
class BattleUnit;
class BuffController;

class BuffFuncBase
{
public:
    typedef std::map<uint32, int32> PropertyMap;

    BuffFuncBase(uint32 funcId);
    virtual ~BuffFuncBase();

    BattleUnit* GetOwner();
    BuffController* GetOwnerBuffController();

    uint32  GetFuncId() {return m_FuncId;}

    virtual void    Start() {}
    virtual void    Update(uint32 delta_time) {}
    virtual void    End();
    virtual void    OnUpdateValue() {}

    virtual void ResetLifeTime(int32 lifeTime) {}

    void SetBuff(Buff* pBuff) {m_pBuff = pBuff;}
    void SetArgs(int32 arg1, int32 arg2, uint32 val_index) {m_arg1 = arg1; m_arg2 = arg2; m_BuffValIndex = val_index;}
    int32 GetArg1() {return m_arg1;}
    int32 GetArg2() {return m_arg2;}
    int32 GetBuffVal();

    const PropertyMap& GetPropertyMap() const {return m_property_map;}

    void AddProperty(uint32 property_id, int32 val);
    void SubProperty(uint32 property_id, int32 val);
    void SetProperty(uint32 property_id, int32 value);
    int32 GetProperty(uint32 property_id);

    void AddDamage(const DamageValue& damage);
    void AddHeal(int32 heal);

    virtual bool    CanDead() {return true;}

    virtual DAMAGE_INFO   FilterDamage(DAMAGE_INFO damage) {return damage;}

protected:
    Buff*       m_pBuff;
    uint32      m_FuncId;   // buff功能Id

private:
    PropertyMap                     m_property_map; // 改变的属性集合<m_PropertyId, m_PropertyValue>

    int32       m_arg1;     // 功能参数1
    int32       m_arg2;     // 功能参数2
    uint32      m_BuffValIndex;     // 使用第几个buff数值
};


#endif // buff_func_base_h__
