/*
 *	公会日志事件定义
 */
#ifndef guild_define_h__
#define guild_define_h__


enum 
{
    GUILD_LOG_CREATE = 1,               // 创建公会
    GUILD_LOG_DESTORY = 2,              // 解散公会
    GUILD_LOG_ADD_MEMBER = 3,           // 加入公会
    GUILD_LOG_REMOVE_MEMBER = 4,        // 离开公会
    GUILD_LOG_CHANGE_OFFICE = 5,        // 改变成员职位
    GUILD_LOG_CHANGE_FLAG = 6,          // 设置公会标记
    GUILD_LOG_DONATE = 7,               // 公会捐献
    GUILD_LOG_BULLETIN = 8,             // 公告公告修改
};
#endif // guild_define_h__
