#include "robot_pch.h"
#include "equip_action.h"
#include "robot.h"
#include "commin_const.h"

Equip_Action::Equip_Action(Robot* pRobot, int32 equiptype, int32 type ,int32 pos):
ActionBase(pRobot)
{
    m_Name = "EquipLeveUp";
    m_equiptype = equiptype;
    m_type = type;
    m_pos = pos;
}

Equip_Action::~Equip_Action(void)
{
}
void Equip_Action::OnStart()
{
    ActionBase::OnStart();
    if(m_type == EQUIP_LEVEL_UP)
    {
        //OPT_EQUIP_UP req;
        //req.type = m_equiptype;
        //m_Robot->Send(MSG_LEVEL_UP_EQUIP_REQ, &req);
    }
    else if(m_type == EQUIP_START_UP)
    {
        //OPT_EQUIP_UP req;
        //req.type  = m_equiptype;
        //req.param = 80;
        //m_Robot->Send(MSG_STAR_UP_EQUIP_REQ,&req);
    }
    else if(m_type == EQUIP_QUALITY_UP)
    {
        //OPT_EQUIP_UP req;
        //req.type = m_equiptype;
        //m_Robot->Send(MSG_QUALITY_UP_EQUIP_REQ,&req);
    }
    else if(m_type == EQUIP_INLAID_JEWEL)
    {
        //EQUIP_INLAID_JEWEL_REQ req;
        //req.equip_type = m_equiptype;
        //req.item_pos = m_pos;
        //m_Robot->Send(MSG_EQUIP_INLAID_JEWEL_REQ,&req);
    }
    else if(m_type == EQUIP_DISMANT_JEWEL)
    {
        //EQUIP_DISMANTLE_JEWEL_REQ req;
        //req.equip_type = m_equiptype;
        //req.jewel_pos = m_pos;
        //m_Robot->Send(MSG_EQUIP_DISMANTLE_JEWEL_REQ,&req);
    }
}

void Equip_Action::OnRecv(uint32 msgID, void* data)
{
    ActionBase::OnRecv(msgID, data);
    //switch(msgID)
    //{
    //    //升级
    //case MSG_LEVEL_UP_EQUIP_ACK:
    //    {
    //        ERROR_CODE* pData = (ERROR_CODE*)data;
    //        if(pData->errcode == ERR_SUCCEED)
    //        {
    //            CnDbg("equpilevelup %d succed\n", m_equiptype);
    //        }
    //        else if(pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg("equpilevelup %d failed\n", m_equiptype);
    //        }
    //        else if(pData->errcode == ERR_UNFOUND_EQUIP)
    //        {
    //            CnDbg("not find equip in  equpilevelup %d failed\n", m_equiptype);
    //        }
    //        else if( pData->errcode == ERR_MAX_EQUIP_LEVEL)
    //        {
    //            //End();
    //            CnDbg("已经升到最大等级\n");
    //        }
    //        else if( pData->errcode == ERR_NO_ENOUGH_PLAYER_LEVEL)
    //        {
    //            CnDbg("player level in  equpilevelup %d failed\n", m_equiptype);
    //        }
    //        else if( pData->errcode == ERR_NO_ENOUGH_GAMEPOINT)
    //        {
    //            CnDbg("not enough gamepoint  equpilevelup %d failed\n", m_equiptype);
    //        }
    //        else if( pData->errcode == ERR_NO_ENOUGH_LVUPSTONE)
    //        {
    //            CnDbg("not enough leveupstone  equpilevelup %d failed\n", m_equiptype);
    //        }
    //    }
    //    End();
    //    break;
    //    //升星
    //case MSG_STAR_UP_EQUIP_ACK:
    //    {
    //        EQUIP_TIME* pData = (EQUIP_TIME*)data;
    //        if(pData->errcode == ERR_SUCCEED)
    //        {
    //            CnDbg("装备 %d 升星成功\n", m_equiptype);
    //        }
    //        else if(pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg("装备 %d 升星失败\n" ,m_equiptype);
    //        }
    //        else if ( pData->errcode == ERR_UNFOUND_EQUIP)
    //        {
    //            CnDbg("not find equip in  装备升星 %d failed\n", m_equiptype);
    //        }
    //        else if( pData->errcode == ERR_MAX_EQUIP_STAR)
    //        {
    //            CnDbg("升星已经升到最大等级\n");
    //        }
    //    }
    //    End();
    //    break;
    //    ///品质
    //case MSG_QUALITY_UP_EQUIP_ACK:
    //    {
    //        ERROR_CODE* pData = (ERROR_CODE*)data;
    //        if(pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg(" 装备品质升级 %d 成功\n", m_equiptype);
    //        }
    //        else if(pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg(" 装备品质升级 %d failed\n", m_equiptype);
    //        }
    //        else if ( pData->errcode == ERR_UNFOUND_EQUIP)
    //        {
    //            CnDbg("not find equip in 装备品质升级 %d failed\n", m_equiptype);
    //        }
    //        else if( pData->errcode == ERR_MAX_EQUIP_QUALITY)
    //        {
    //            CnDbg("装备品质升级到最大等级\n");
    //        }
    //    }
    //    End();
    //    break;
    //case  MSG_EQUIP_INLAID_JEWEL_ACK:
    //    {
    //        ERROR_CODE* pData = (ERROR_CODE*)data;
    //        if(pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg(" 装载装备宝石失败 %s failed\n",m_equiptype);
    //        }
    //        else if ( pData->errcode == ERR_UNFOUND_EQUIP)
    //        {
    //            CnDbg("not find equip in 装载装备宝石失败 %s failed\n",m_equiptype);
    //        }
    //    }
    //    End();
    //    break;
    //case MSG_EQUIP_DISMANTLE_JEWEL_ACK:
    //    {
    //        ERROR_CODE* pData = (ERROR_CODE*)data;
    //        if(pData->errcode == ERR_FAILED)
    //        {
    //            CnDbg(" 卸载装备宝石失败 %s failed\n",m_equiptype);
    //        }
    //        else if ( pData->errcode == ERR_UNFOUND_EQUIP)
    //        {
    //            CnDbg("not find equip in 卸载装备宝石失败 %s failed\n",m_equiptype);
    //        }
    //        else if( pData->errcode == ERR_NO_ENOUGH_BAGPOS)
    //        {
    //            CnDbg("背包里面没有要升级的宝石\n");
    //        }
    //    }
    //    End();
    //    break;
    //}
}
