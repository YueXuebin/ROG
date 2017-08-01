#include "robot_pch.h"
#include "item_action.h"
#include "robot.h"

Item_Action::Item_Action(Robot* pRobot, uint8 opt, uint8 src_pos, uint8 dst_pos) :
ActionBase(pRobot)
{
    m_Name = "Item";

    m_opt = opt;                    // 6类型:使用物品m_srcPos为物品ID, m_dstPos为物品数量
    m_srcPos = src_pos;
    m_dstPos = dst_pos;
}

Item_Action::~Item_Action()
{

}

void Item_Action::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);

    //switch(msgID)
    //{
    //case MSG_OPT_BAG_ITEM_NTF:
    //    RecvOptBagItemNtf(data);
    //    break;
    //default:
    //    break;
    //}
}

void Item_Action::OnStart()
{
    //OPT_BAG_ITEM_REQ sendMsg;
    //sendMsg.opt = m_opt;
    //sendMsg.dst_pos = m_dstPos;
    //sendMsg.src_pos = m_srcPos;
    //m_Robot->Send(MSG_OPT_BAG_ITEM_REQ, &sendMsg);
    //End();
}

void Item_Action::RecvOptBagItemNtf(void* data)
{
    //OPT_BAG_ITEM_LIST* recvMsg = (OPT_BAG_ITEM_LIST*)data;

    //for(std::vector<OPT_BAG_ITEM>::iterator it = recvMsg->list.begin(); it != recvMsg->list.end(); ++it)
    //{
    //    printf("item_id:%d, num:%d, opt:%d, pos:%d\n", it->item_id, it->num, it->opt, it->pos);
    //}
}


