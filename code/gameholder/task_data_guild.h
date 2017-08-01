#ifndef task_data_guild_h__
#define task_data_guild_h__


#include "task_data_game.h"


// 公会任务
class GuildTask : public GameTask
{
    CnDeclareRTTI
public:
    GuildTask(const TASK_TEMPLATE* task_template, Player* player);
    virtual ~GuildTask();

    virtual uint8 GetType() {return TASK_TYPE_GUILD;}

    virtual void OnRewards();

    virtual void OnAdd();
};


#endif // task_data_guild_h__
