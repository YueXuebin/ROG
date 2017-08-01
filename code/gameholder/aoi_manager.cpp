#include "gameholder_pch.h"
#include "aoi_manager.h"
#include "scene.h"
#include "sceneobject.h"
#include "battle_unit.h"
#include "scene_notifier_aoi.h"
#include "player.h"
#include "npc.h"
#include "aoi_object.h"
#include "aoi_system_manager.h"
#include "cell_visitor.h"
#include "notifier_utils.h"
#include "team_relationship.h"


// 计算用变量
const uint32 MAX_BOOLS = 1000;      // 不会一个Cell中有这么多人吧
bool disappearBools[MAX_BOOLS];
bool appearBools[MAX_BOOLS];


AOIManager::AOIManager(Scene *scene, SceneNotifierAOI* pSceneNotifier):
m_pScene(scene),
m_pSceneNotifier(pSceneNotifier)
{
    // 创建Cell对应的CellVisitor
    CoWorld* pCoWorld = scene->GetCoWorld();
    m_CellLenX = pCoWorld->GetCellMaxX() - pCoWorld->GetCellMinX() + 1;
    m_CellLenY = pCoWorld->GetCellMaxY() - pCoWorld->GetCellMinY() + 1;

    m_CellVisitors = (CellVisitor**)CnMalloc(sizeof(CellVisitor*)*m_CellLenY);
    for(uint32 y=0; y<m_CellLenY; y++)
    {
        m_CellVisitors[y] = CnNew CellVisitor[m_CellLenX];
    }
}

AOIManager::~AOIManager()
{
    for(uint32 y=0; y<m_CellLenY; y++)
    {
        CnDelete[] m_CellVisitors[y];
    }
    CnFree(m_CellVisitors);
    m_CellVisitors = NULL;
}

CellVisitor* AOIManager::GetCellVisitor(int32 x, int32 y)
{
    return &m_CellVisitors[y][x];
}
// 基于场景管理的消息通知
void AOIManager::NotifyMessage(SceneObject* sender, uint32 msgId, void* msg, bool exclude_sender)
{
    uint32 objectID = sender->GetId();
    AOIObject* obj = GetAOIObject(objectID);
    CnAssert(obj);

    CellVisitor& cellVisitor = m_CellVisitors[obj->GetCellY()][obj->GetCellX()];

    std::list<Player*>& seePlayers = cellVisitor.GetSeePlayers();

    // 非玩家对象,通知可以看见该对象的玩家
    for(std::list<Player*>::iterator iter = seePlayers.begin(); iter != seePlayers.end(); iter++)
    {
        Player* pSeePlayer = *iter;
        if(exclude_sender && sender == pSeePlayer)
            continue;
        
        if(TeamRelationShip::Instance()->HasRelation(sender, pSeePlayer))
            continue;

        pSeePlayer->SendToGate(msgId, msg);
    }
}

// 对象进入场景
void AOIManager::Enter(SceneObject* sceneObj)
{
    uint32 objectID = sceneObj->GetId();
    AOIObject* obj = NULL;
#ifdef DEBUG
    // 检验加入的对象必定不存在
    obj = GetAOIObject(objectID);
    CnAssert(obj == NULL);
#endif

    obj = AOISystemManager::Instance()->CreateAOIObject(sceneObj, this);
    m_AOIObjectMap.insert(std::make_pair(objectID, obj));

    // 进入场景通知
    EnterAction(obj);
}

// 对象离开场景
void AOIManager::Leave(SceneObject* sceneObj)
{
    uint32 objectID = sceneObj->GetId();
    std::map<uint32, AOIObject*>::iterator diter = m_AOIObjectMap.find(objectID);
    CnAssert(diter != m_AOIObjectMap.end());        // 必定在场景中

    AOIObject *obj = diter->second;

    // 离开场景通知
    LeaveAction(obj);

    m_AOIObjectMap.erase(diter);
    AOISystemManager::Instance()->DestoryAOIObject(obj);        // 对象离开场景
}

void AOIManager::Update()
{
    // 场景对象的移动导致,出现消失通知
    for(std::map<uint32, AOIObject*>::iterator iter = m_AOIObjectMap.begin(); iter != m_AOIObjectMap.end(); iter++)
    {
        AOIObject* pAOIObj = iter->second;
        pAOIObj->UpdateCellPos();           // 物理空间->AOI空间位置更新
        Update_AOIObject(pAOIObj);
    }

    // 玩家的视野变化导致，出现消失通知
    for(std::map<uint32, AOIObject*>::iterator iter = m_AOIObjectMap.begin(); iter != m_AOIObjectMap.end(); iter++)
    {
        AOIObject* pAOIObj = iter->second;
        if(pAOIObj->CanSee())
            Update_Player(pAOIObj);       // 玩家的视野移动处理
    }

}

void AOIManager::Update_AOIObject(AOIObject* aoiObject)
{
    // 检验该场景对象是否从一个Cell进入另一个Cell
    SceneObject* pSceneObject = aoiObject->GetSceneObject();
    
    int32 oldCellX = aoiObject->GetLastCellX();
    int32 oldCellY = aoiObject->GetLastCellY();
    int32 newCellX = aoiObject->GetCellX();
    int32 newCellY = aoiObject->GetCellY();

    if(oldCellX == newCellX && oldCellY == newCellY)
        return;         // 该对象没有移动到新的Cell中
    
    CellVisitor& oldVisitor = m_CellVisitors[oldCellY][oldCellX];
    CellVisitor& newVisitor = m_CellVisitors[newCellY][newCellX];

    CnAssert(oldVisitor.HasAOIObject(aoiObject));
    CnAssert(!newVisitor.HasAOIObject(aoiObject));
    oldVisitor.RemoveAOIObject(aoiObject);
    newVisitor.AddAOIObject(aoiObject);

    std::list<Player*>& disappearPlayers = oldVisitor.GetSeePlayers();
    std::list<Player*>& appearPlayers = newVisitor.GetSeePlayers();

    memset(disappearBools, 0, sizeof(disappearBools));
    memset(appearBools, 0, sizeof(appearBools));
    
    int32 disappearIndex = 0;
    int32 appearIndex = 0;
    for(std::list<Player*>::iterator disappearIter = disappearPlayers.begin(); disappearIter != disappearPlayers.end(); disappearIter++)
    {
        appearIndex = 0;
        for(std::list<Player*>::iterator appearIter = appearPlayers.begin(); appearIter != appearPlayers.end(); appearIter++)
        {
            if(*appearIter == *disappearIter)
            {   // 旧Cell和新Cell都存在的玩家，标记为不发送通知
                CnAssert(disappearBools[disappearIndex] == 0);
                CnAssert(appearBools[appearIndex] == 0);
                disappearBools[disappearIndex] = 1;
                appearBools[appearIndex] = 1;
            }
            appearIndex++;
        }
        disappearIndex++;
    }
    
    // 消失通知
    disappearIndex = -1;
    for(std::list<Player*>::iterator disappearIter = disappearPlayers.begin(); disappearIter != disappearPlayers.end(); disappearIter++)
    {
        disappearIndex++;

        if(disappearBools[disappearIndex])
            continue;
        
        Player* pOtherPlayer = *disappearIter;

        if(pSceneObject == pOtherPlayer)
            continue;

        if(!TeamRelationShip::Instance()->HasRelation(pSceneObject, pOtherPlayer))
        {
            if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            {
                BATTLEUNIT_DISAPPEAR sendMsg1;
                sendMsg1.object_id = pSceneObject->GetId();
                pOtherPlayer->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);
            }
            else
            {
                SCENEOBJECT_DISAPPEAR sendMsg2;
                sendMsg2.object_id = pSceneObject->GetId();
                sendMsg2.object_type = pSceneObject->GetObjectType();
                pOtherPlayer->SendToGate(MSG_SCENEOBJECT_DISAPPEAR, &sendMsg2);
            }
        }

        AOIObject* otherPlayerAOIObj = GetAOIObject(pOtherPlayer->GetId());
        otherPlayerAOIObj->LeaveSee(pSceneObject);
    }
    // 出现通知
    appearIndex = -1;
    for(std::list<Player*>::iterator appearIter = appearPlayers.begin(); appearIter != appearPlayers.end(); appearIter++)
    {
        appearIndex++;

        if(appearBools[appearIndex])
            continue;

        Player* pOtherPlayer = *appearIter;

        if(pSceneObject == pOtherPlayer)
            continue;

        if(!TeamRelationShip::Instance()->HasRelation(pSceneObject, pOtherPlayer))
        {
            if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            {
                BATTLEUNIT_APPEAR_LIST sendMsg1;
                FILL_MSG_APPEAR_LIST(sendMsg1, (BattleUnit*)pSceneObject, pOtherPlayer);
                pOtherPlayer->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg1);
            }
            else
            {
                SCENEOBJECT_APPEAR_LIST sendMsg2;
                FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendMsg2, pSceneObject, pOtherPlayer);
                pOtherPlayer->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendMsg2);
            }
        }

        AOIObject* otherPlayerAOIObj = GetAOIObject(pOtherPlayer->GetId());
        otherPlayerAOIObj->EnterSee(pSceneObject);
    }

}

void AOIManager::Update_Player(AOIObject* aoiObject)
{
    int32 oldCellX = aoiObject->GetLastCellX();
    int32 oldCellY = aoiObject->GetLastCellY();
    int32 newCellX = aoiObject->GetCellX();
    int32 newCellY = aoiObject->GetCellY();

    if(oldCellX == newCellX && oldCellY == newCellY)
        return;         // 该对象没有移动到新的Cell中

    Player* pPlayer = (Player*)aoiObject->GetSceneObject();
    
    int32 oldMinSeeX = aoiObject->GetLastMinSeeX();
    int32 oldMinSeeY = aoiObject->GetLastMinSeeY();
    int32 oldMaxSeeX = aoiObject->GetLastMaxSeeX();
    int32 oldMaxSeeY = aoiObject->GetLastMaxSeeY();

    int32 newMinSeeX = aoiObject->GetMinSeeX();
    int32 newMinSeeY = aoiObject->GetMinSeeY();
    int32 newMaxSeeX = aoiObject->GetMaxSeeX();
    int32 newMaxSeeY = aoiObject->GetMaxSeeY();

    // 计算老视野和新视野的交集
    int32 crossMinX = 0;
    int32 crossMaxX = 0;
    int32 crossMinY = 0;
    int32 crossMaxY = 0;
    crossMinX = Crown::SDMax(oldMinSeeX, newMinSeeX);
    crossMaxX = Crown::SDMin(oldMaxSeeX, newMaxSeeX);
    crossMinY = Crown::SDMax(oldMinSeeY, newMinSeeY);
    crossMaxY = Crown::SDMin(oldMaxSeeY, newMaxSeeY);

    // 老视野中的对象消失
    for(int32 y=oldMinSeeY; y<=oldMaxSeeY; y++)
    {
        for(int32 x=oldMinSeeX; x<=oldMaxSeeX; x++)
        {
            if(x>=crossMinX && x<=crossMaxX && y>=crossMinY && y<=crossMaxY)
                continue;           // 交集Cell

            CellVisitor& cellVisitor = m_CellVisitors[y][x];
            std::list<AOIObject*>& otherAOIObjects = cellVisitor.GetAOIObjects();

            for(std::list<AOIObject*>::iterator iter = otherAOIObjects.begin(); iter != otherAOIObjects.end(); iter++)
            {
                SceneObject* pOtherSceneObject = (*iter)->GetSceneObject();
                if(pOtherSceneObject == pPlayer)
                    continue;

                if(!TeamRelationShip::Instance()->HasRelation(pPlayer, pOtherSceneObject))
                {
                    if(pOtherSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
                    {
                        BATTLEUNIT_DISAPPEAR sendMsg1;
                        sendMsg1.object_id = pOtherSceneObject->GetId();
                        pPlayer->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);
                    }
                    else
                    {
                        SCENEOBJECT_DISAPPEAR sendMsg2;
                        sendMsg2.object_id = pOtherSceneObject->GetId();
                        sendMsg2.object_type = pOtherSceneObject->GetObjectType();
                        pPlayer->SendToGate(MSG_SCENEOBJECT_DISAPPEAR, &sendMsg2);
                    }
                }

                aoiObject->LeaveSee(pOtherSceneObject);
            }
            
            m_CellVisitors[y][x].RemoveSeePlayer(pPlayer);
        }
    }

    // 新视野中的对象出现
    BATTLEUNIT_APPEAR_LIST sendMsg1;
    SCENEOBJECT_APPEAR_LIST sendMsg2;
    uint32 count1 = 0;
    uint32 count2 = 0;
    for(int32 y=newMinSeeY; y<=newMaxSeeY; y++)
    {
        for(int32 x=newMinSeeX; x<=newMaxSeeX; x++)
        {
            if(x>=crossMinX && x<=crossMaxX && y>=crossMinY && y<=crossMaxY)
                continue;           // 交集Cell
            
            CellVisitor& cellVisitor = m_CellVisitors[y][x];
            std::list<AOIObject*>& otherAOIObjects = cellVisitor.GetAOIObjects();

            for(std::list<AOIObject*>::iterator iter = otherAOIObjects.begin(); iter != otherAOIObjects.end(); iter++)
            {
                SceneObject* pOtherSceneObject = (*iter)->GetSceneObject();
                if(pOtherSceneObject == pPlayer)
                    continue;

                if(!TeamRelationShip::Instance()->HasRelation(pPlayer, pOtherSceneObject))
                {
                    if(pOtherSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
                    {
                        FILL_MSG_APPEAR_LIST(sendMsg1, (BattleUnit*)pOtherSceneObject, pPlayer);
                        count1++;
                    }
                    else
                    {
                        FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendMsg2, pOtherSceneObject, pPlayer);
                        count2++;
                    }
                }

                aoiObject->EnterSee(pOtherSceneObject);
            }

            m_CellVisitors[y][x].AddSeePlayer(pPlayer);
        }
    }

    if(count1 > 0)
        pPlayer->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg1);
    if(count2 > 0)
        pPlayer->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendMsg2);
}

AOIObject* AOIManager::GetAOIObject(uint32 id)
{
    std::map<uint32, AOIObject*>::iterator iter = m_AOIObjectMap.find(id);
    if(iter == m_AOIObjectMap.end())
        return NULL;
    return iter->second;
}
// 对象进入场景通知
void AOIManager::EnterAction(AOIObject *aoiObject)
{
    SceneObject* pSceneObject = aoiObject->GetSceneObject();
    // 检测该对象的可见区域
    if(aoiObject->CanSee())
    {
        BATTLEUNIT_APPEAR_LIST sendMsg1;
        SCENEOBJECT_APPEAR_LIST sendMsg2;
        uint32 count1 = 0;
        uint32 count2 = 0;

        for(int32 y=aoiObject->GetMinSeeY(); y<=aoiObject->GetMaxSeeY(); y++)
        {
            for(int32 x=aoiObject->GetMinSeeX(); x<=aoiObject->GetMaxSeeX(); x++)
            {   
                // 将可见区域的对象通知给该对象
                CellVisitor& cellVisitor = m_CellVisitors[y][x];
                std::list<AOIObject*>& otherAOIObjects = cellVisitor.GetAOIObjects();

                for(std::list<AOIObject*>::iterator iter = otherAOIObjects.begin(); iter != otherAOIObjects.end(); iter++)
                {
                    SceneObject* pOtherSceneObject = (*iter)->GetSceneObject();
                    
                    if(pOtherSceneObject == pSceneObject)
                        continue;

                    if(!TeamRelationShip::Instance()->HasRelation(pSceneObject, pOtherSceneObject))
                    {
                        // 将对象通知给该对象，appear other
                        if(pOtherSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
                        {
                            FILL_MSG_APPEAR_LIST(sendMsg1, (BattleUnit*)pOtherSceneObject, (Player*)pSceneObject);
                            count1++;
                        }
                        else
                        {
                            FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendMsg2, pOtherSceneObject, (Player*)pSceneObject);
                            count2++;
                        }
                    }
                    else
                    {
                        // 有关系的对象,由关系管理器负责通知
                    }

                    aoiObject->EnterSee(pOtherSceneObject);
                }

                m_CellVisitors[y][x].AddSeePlayer((Player*)pSceneObject);
            }
        }
        if(count1>0)
            ((Player*)pSceneObject)->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg1);
        if(count2>0)
            ((Player*)pSceneObject)->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendMsg2);
    }

    // 将该对象通知给所在区域
    EnterCells(aoiObject);

    // 有关系对象间的通知
    TeamRelationShip::Instance()->SendAppearToTeamMemberInSameScene(aoiObject->GetSceneObject());
}
// 对象离开场景通知
void AOIManager::LeaveAction(AOIObject *aoiObject)
{
    SceneObject* pSceneObject = aoiObject->GetSceneObject();

    if(aoiObject->CanSee())
    {
        // 该对象的可见Cell中移除该对象
        for(int32 y=aoiObject->GetMinSeeY(); y<=aoiObject->GetMaxSeeY(); y++)
        {
            for(int32 x=aoiObject->GetMinSeeX(); x<=aoiObject->GetMaxSeeX(); x++)
            {   
                // 将可见区域的对象通知给该对象
                CellVisitor& cellVisitor = m_CellVisitors[y][x];
                cellVisitor.RemoveSeePlayer((Player*)pSceneObject);
            }
        }
    }
    
    // 通知能看见该对象所在Cell的对象
    LeaveCells(aoiObject);

    // 有关系对象间的通知
    TeamRelationShip::Instance()->SendDisappearToTeamMemberInSameScene(aoiObject->GetSceneObject());

#ifdef DEBUG
    // 检验所有的CellVisitor中都不应该有该对象
    if(aoiObject->CanSee())
    {
        for(int32 y=0; y<(GetCellLenY()-1); y++)
        {
            for(int32 x=0; x<(GetCellLenX()-1); x++)
            {
                CellVisitor& cellVisitor = m_CellVisitors[y][x];
                CnAssert(!cellVisitor.HasSeePlayer((Player*)pSceneObject));
            }
        }
    }
#endif
}

void AOIManager::EnterCells(AOIObject *obj)
{
    // 将该对象通知给所在区域
    SceneObject* pSceneObject = obj->GetSceneObject();
    CellVisitor& cellVisitor = m_CellVisitors[obj->GetCellY()][obj->GetCellX()];
    const std::list<Player*> players = cellVisitor.GetSeePlayers();
    for(std::list<Player*>::const_iterator iter = players.begin(); iter != players.end(); iter++)
    {
        // 将该对象通知其能看见该Cell的对象,appear this
        Player* pPlayer = *iter;
        if(pPlayer == pSceneObject)
            continue;

        if(!TeamRelationShip::Instance()->HasRelation(pSceneObject, pPlayer))
        {
            if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            {
                BATTLEUNIT_APPEAR_LIST sendMsg1;
                FILL_MSG_APPEAR_LIST(sendMsg1, (BattleUnit*)pSceneObject, pPlayer);
                pPlayer->SendToGate(MSG_BATTLEUNIT_APPEAR_LIST, &sendMsg1);
            }
            else
            {
                SCENEOBJECT_APPEAR_LIST sendMsg2;
                FILL_MSG_SCENEOBJECT_APPEAR_LIST(sendMsg2, pSceneObject, pPlayer);
                pPlayer->SendToGate(MSG_SCENEOBJECT_APPEAR_LIST, &sendMsg2);
            }
        }
        else
        {
            // 有关系的对象,由关系管理器负责通知
        }

        AOIObject* otherPlayerAOIObj = GetAOIObject(pPlayer->GetId());
        otherPlayerAOIObj->EnterSee(pSceneObject);
    }
    
    cellVisitor.AddAOIObject(obj);

#ifdef DEBUG
    if(obj->CanSee())
    {
        // 验证,在视野相同的情况下，应该能够互相看见(前提是视野相同)
        for(std::map<uint32, SceneObject*>::iterator iter = obj->m_SeeObjects.begin(); iter != obj->m_SeeObjects.end(); iter++)
        {
            SceneObject* pSObject = iter->second;
            if(!pSObject->IsKindOf(&Player::ms_RTTI))
                continue;

            CnAssert(cellVisitor.HasSeePlayer((Player*)pSObject));
        }

        for(std::list<Player*>::iterator iter = cellVisitor.GetSeePlayers().begin(); iter != cellVisitor.GetSeePlayers().end(); iter++)
        {
            Player* pPlayer = *iter;
            if(pPlayer == pSceneObject)
                continue;

            CnAssert(obj->m_SeeObjects.find(pPlayer->GetId()) != obj->m_SeeObjects.end());
        }
    }
#endif
}

void AOIManager::LeaveCells(AOIObject* obj)
{
    // 通知能看见该对象所在Cell的对象
    SceneObject* pSceneObject = obj->GetSceneObject();
    CellVisitor& cellVisitor = m_CellVisitors[obj->GetCellY()][obj->GetCellX()];
    const std::list<Player*> players = cellVisitor.GetSeePlayers();
    for(std::list<Player*>::const_iterator iter = players.begin(); iter != players.end(); iter++)
    {
        Player* pPlayer = *iter;
        CnAssert(pPlayer != pSceneObject);          // 已经从CellVisitor中移除了

        if(!TeamRelationShip::Instance()->HasRelation(pPlayer, pSceneObject))
        {
            // 通知Cell中的对象,该对象消失,disappear this
            if(pSceneObject->IsKindOf(&BattleUnit::ms_RTTI))
            {
                BATTLEUNIT_DISAPPEAR sendMsg1;
                sendMsg1.object_id = pSceneObject->GetId();
                pPlayer->SendToGate(MSG_BATTLEUNIT_DISAPPEAR, &sendMsg1);
            }
            else
            {
                SCENEOBJECT_DISAPPEAR sendMsg2;
                sendMsg2.object_id = pSceneObject->GetId();
                sendMsg2.object_type = pSceneObject->GetObjectType();
                pPlayer->SendToGate(MSG_SCENEOBJECT_DISAPPEAR, &sendMsg2);
            }
        }
        else
        {

        }

        AOIObject* otherPlayerAOIObj = GetAOIObject(pPlayer->GetId());
        otherPlayerAOIObj->LeaveSee(pSceneObject);
    }

    cellVisitor.RemoveAOIObject(obj);
}

bool AOIManager::IsPlayerSee(SceneObject* pSceneObject)
{
    AOIObject* pAOIObject = GetAOIObject(pSceneObject->GetId());
    if(!pAOIObject)
    {
        CnAssert(false);
        return true;
    }
    CellVisitor& cellVisitor = m_CellVisitors[pAOIObject->GetCellY()][pAOIObject->GetCellX()];
    return cellVisitor.GetSeePlayers().size() > 0;
}

bool AOIManager::IsPlayerCanSee(Player* player, SceneObject* pSceneObject)
{
    AOIObject* pAOIObject = GetAOIObject(pSceneObject->GetId());
    if(!pAOIObject)
    {
        CnAssert(false);
        return false;
    }
    CellVisitor& cellVisitor = m_CellVisitors[pAOIObject->GetCellY()][pAOIObject->GetCellX()];

    const std::list<Player*>& seePlayerList = cellVisitor.GetSeePlayers();
    for(std::list<Player*>::const_iterator iter = seePlayerList.begin(); iter != seePlayerList.end(); iter++)
    {
        if(player == *iter)
            return true;
    }
    return false;       // 某玩家看不到该对象
}
