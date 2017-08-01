#ifndef frinde_action_h
#define frinde_action_h
#include "action_base.h"
#include "commin_const.h"
class Friend_Action :public ActionBase
{
public:
    Friend_Action(Robot* pRobot,int32 _name,int32 type,uint32 player_id_h);
    virtual ~Friend_Action(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    std::string m_name;
    int32 m_type;//添加还是删除好友,和好友应答
    uint32 m_player_id_h;//玩家的目标id
};
#endif
