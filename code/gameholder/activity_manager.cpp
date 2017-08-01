#include "gameholder_pch.h"
#include "activity_manager.h"
#include "player.h"
#include "player_manager.h"
#include "config_manager.h"
#include "center_handler.h"

IMPLEMENT_SINGLETON(ActivityManager)

ActivityManager::ActivityManager()
{
    Clear();
}

ActivityManager::~ActivityManager()
{

}

void ActivityManager::Clear()
{
}

void ActivityManager::OnCSRecv(uint32 msgID, void* data)
{
    switch(msgID)
    {
    case MSG_ACTIVITY_LIST_NTF:     // TS->GS
        {   // TS֪ͨGS���»�б�
            CnDbg("update activity list\n");
            OnUpdateActivityList((ACTIVITY_LIST*)data);       // ������Ҫ���״̬������״̬���¸����
            break;
        }
    default:
        break;
    }
}

void ActivityManager::OnPlayerRecv( Player* player, uint32 msgID, void* data )
{
    switch(msgID)
    {
    case MSG_ACTIVITY_LIST_REQ:
        {   // �ͻ��������б�
            player->SendToGate(MSG_ACTIVITY_LIST_NTF, &m_activitList);
        }
        break;
    default:
        break;
    }
}

void ActivityManager::OnServerReady()
{
    // ��TS�����б�
    ReqTSActivityList();
}

bool ActivityManager::IsActivityTypeOpen(uint32 activityType)
{
    for(std::vector<ACTIVITY_INFO>::iterator iter = m_activitList.activity_list.begin(); iter != m_activitList.activity_list.end(); iter++)
    {
        ACTIVITY_INFO& activity_info = *iter;
        if((activity_info.activity_type == activityType) && 
           (activity_info.activity_state == ACTIVITY_STATE_START))
        {
            return true;
        }
    }
    return false;
}

// ==================================================
// ���ݻ���ͻ�û
// ==================================================
ACTIVITY_INFO* ActivityManager::GetActivityByType(int32 type, bool checkState)
{
    for(std::vector<ACTIVITY_INFO>::iterator activityIt = m_activitList.activity_list.begin();
        activityIt != m_activitList.activity_list.end(); ++activityIt)
    {
        // ��Ҫ�����͵Ļ����Ҫ���ѿ���
        if(checkState)
        {
            if((activityIt->activity_type == type) && (ACTIVITY_STATE_CLOSED < activityIt->activity_state))
            {
                return &(*activityIt);
            }
        }
        else
        {
            if(activityIt->activity_type == type)
            {
                return &(*activityIt);
            }
        }
    }
    return 0;
}

// ==================================================
// ��û
// ==================================================
ACTIVITY_INFO* ActivityManager::GetActivityByID(uint32 id)
{
    for(std::vector<ACTIVITY_INFO>::iterator activityIt = m_activitList.activity_list.begin();
            activityIt != m_activitList.activity_list.end(); ++activityIt)
    {
        if(activityIt->activity_id == id)
        {
            return &(*activityIt);
        }
    }
    return 0;
}
// ��TS�����б�
void ActivityManager::ReqTSActivityList()
{
    NONE sendMsg;
    CenterHandler::Instance()->SendToMasterCenter(MSG_ACTIVITY_LIST_REQ, &sendMsg);
}
// ��ӻ
void ActivityManager::AddActivity(ACTIVITY_INFO& activity_info)
{
    CenterHandler::Instance()->SendToMasterCenter(MSG_ACTIVITY_ADD_REQ, &activity_info);
}

void ActivityManager::AddActivity( uint32 activity_id, uint32 activity_type, uint32 activity_state, uint32 start_time, uint32 end_time )
{
    ACTIVITY_INFO info;
    info.activity_id = activity_id;
    info.activity_type = activity_type;
    info.activity_state =activity_state;
    info.start_time = start_time;
    info.end_time = end_time;
    AddActivity(info);
}
// ɾ���
void ActivityManager::DelActivity( ACTIVITY_INFO& activity_info )
{
    CenterHandler::Instance()->SendToMasterCenter(MSG_ACTIVITY_DELETE_REQ, &activity_info);
}

void ActivityManager::DelActivity(uint32 activity_id)
{
    ACTIVITY_INFO info;
    info.activity_id = activity_id;
    DelActivity(info);
}

// ���»
void ActivityManager::UpdateActivity( ACTIVITY_INFO& activity_info )
{
    CenterHandler::Instance()->SendToMasterCenter(MSG_ACTIVITY_UPDATE_REQ, &activity_info);
}

// �رջ
void ActivityManager::CloseActivity(uint32 activity_id)
{
    if(!GetActivityByID(activity_id))
    {
        CnDbg("close activity %d not exist\n", activity_id);
        return;
    }

    ACTIVITY_INFO info;
    info.activity_id = activity_id;
    info.activity_state = ACTIVITY_STATE_CLOSED;
    UpdateActivity(info);
}

void ActivityManager::CloseActivityByType( uint32 activityType )
{
    ACTIVITY_INFO* pOpenActivityInfo = GetActivityByType(activityType, true);
    if(!pOpenActivityInfo)
        return;

    CloseActivity(pOpenActivityInfo->activity_id);
}
// �����
void ActivityManager::OpenActivity( uint32 activity_id )
{
    if(!GetActivityByID(activity_id))
    {
        CnDbg("open activity %d not exist\n", activity_id);
        return;
    }

    ACTIVITY_INFO info;
    info.activity_id = activity_id;
    info.activity_state = ACTIVITY_STATE_START;
    UpdateActivity(info);
}

// ==================================================
// ���»
// ==================================================
void ActivityManager::OnUpdateActivityList(const ACTIVITY_LIST* list)
{
    ACTIVITY_INFO* activityInfo = NULL;

    if(!list) return;   // ��������Ч��

    for(std::vector<ACTIVITY_INFO>::const_iterator activityIt = list->activity_list.begin();
        activityIt != list->activity_list.end(); ++activityIt)
    {
        activityInfo = GetActivityByID(activityIt->activity_id);
        if(activityInfo)
        {
            _UpdateActivity(*activityIt, activityInfo);
        }
        else
        {
            _AddActivity(*activityIt);
        }
    }
}

void ActivityManager::_UpdateActivity(const  ACTIVITY_INFO& newinfo, ACTIVITY_INFO* info)
{
    *info = newinfo;
}

void ActivityManager::_AddActivity( const ACTIVITY_INFO& newinfo )
{
    m_activitList.activity_list.push_back(newinfo);
}
