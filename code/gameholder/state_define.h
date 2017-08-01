/*
 *	状态机定义
 */
#ifndef state_define_h__
#define state_define_h__

enum
{
    STATE_NONE = 0,         // 无状态
    STATE_IDLE = 1,         // 待机状态
    STATE_WALK = 2,         // 走
    STATE_RUN = 3,          // 移动状态
    STATE_STUN = 4,         // 眩晕状态
    STATE_DIE = 5,          // 死亡状态
    STATE_ATTACK = 6,       // 施法状态
    STATE_MOVIE = 7,        // 演绎状态(怪物出场、演绎等)
    STATE_DANCE = 9,        // 跳舞状态
    STATE_TOUCH = 10,       // 采集状态

};

const int32 STATE_FOREVER_TIME  = -1;
const int32 STATE_NO_TIME       = 0;

inline bool IsServerState(int32 state)
{
    return state == STATE_STUN || state == STATE_DIE || state == STATE_DANCE || state == STATE_TOUCH || state == STATE_MOVIE;
}



#endif // state_define_h__
