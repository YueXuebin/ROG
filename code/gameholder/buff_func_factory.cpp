#include "gameholder_pch.h"
#include "buff_define.h"
#include "buff_func_factory.h"
#include "buff_func_base.h"
#include "buff_func_stun.h"
#include "buff_func_damage.h"
#include "buff_func_damage_scale.h"
#include "buff_func_prop_change.h"
#include "buff_func_prop_scale.h"
#include "buff_func_silence.h"
#include "buff_func_immortal.h"
#include "buff_func_absorb_damage.h"
#include "buff_func_magic_damage.h"
#include "buff_func_exp_multiple.h"
#include "buff_func_heal.h"
#include "buff_func_shock.h"
#include "buff_func_bleed.h"



BuffFuncBase* CreateBuffFunc( int32 buffFuncType )
{
    BuffFuncBase* pNewBuffFunc = NULL;

    switch (buffFuncType)
    {
    case BUFF_FUNC_STUN:
        pNewBuffFunc = CnNew BuffFuncStun;
        break;
    case BUFF_FUNC_PROP_SCALE:
        pNewBuffFunc = CnNew BuffFuncPropScale;
        break;
    case BUFF_FUNC_PROP_CHANGE:
        pNewBuffFunc = CnNew BuffFuncPropChange;
        break;
    case BUFF_FUNC_HEAL:
        pNewBuffFunc = CnNew BuffFuncHeal;
        break;
    case BUFF_FUNC_IMMORTAL:
        pNewBuffFunc = CnNew BuffFuncImmortal;
        break;
    case BUFF_FUNC_DAMAGE:
        pNewBuffFunc = CnNew BuffFuncDamage;
        break;
    case BUFF_FUNC_SHOCK:
        pNewBuffFunc = CnNew BuffFuncShock;
        break;
    case BUFF_FUNC_BLEED:
        pNewBuffFunc = CnNew BuffFuncBleed;
        break;
    case BUFF_FUNC_EXP_MUL:
        pNewBuffFunc = CnNew BuffFuncBase(BUFF_FUNC_EXP_MUL);
        break;
    case BUFF_FUNC_DROP_QUALITY:
        pNewBuffFunc = CnNew BuffFuncBase(BUFF_FUNC_DROP_QUALITY);
        break;
    default:
        CnAssert(false);
        CnFatal("buff type %d no exist\n", buffFuncType);
        break;
    }

    return pNewBuffFunc;
}
