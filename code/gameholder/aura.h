#ifndef aura_h__
#define aura_h__

#include "buff.h"

const int32 MAX_AURA_MEMBER = 20;

/*
 *	光环
 */
class Aura : public Buff
{
public:
    Aura(const BuffData* pData, uint32 CasterId, int32 buffVal1, int32 buffVal2);
    virtual ~Aura();

    virtual void Update(uint32 delta_time);
    virtual bool IsAura() {return true;}

protected:
    void UpdateSearch();

private:
    int32   m_SecondTimer;

};

#endif // aura_h__
