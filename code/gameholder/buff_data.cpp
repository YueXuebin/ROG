#include "gameholder_pch.h"
#include "buff_data.h"
#include "buff_define.h"


BuffData::BuffData()
{
    m_canSelf = false;
    m_canFriend = false;
    m_canEnemy = false;

    m_BuffInfo.buff_overlap = BUFF_NO_OVERLAP;
}
