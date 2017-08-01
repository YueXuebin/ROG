#include "gameholder_pch.h"
#include "ai_manager.h"
#include "game_define.h"
#include "player.h"
#include "player_manager.h"



IMPLEMENT_SINGLETON(AIManager)

AIManager::AIManager()
{

}

AIManager::~AIManager()
{
    for(AIDocMap::iterator iter = m_AiDocMap.begin(); iter != m_AiDocMap.end(); iter++)
    {
        CnDelete iter->second;
    }
    m_AiDocMap.clear();
}

bool AIManager::Init()
{
    return true;
}

TiXmlDocument* AIManager::GetAIDoc( const std::string& aiName )
{
    if(m_AiDocMap.find(aiName) != m_AiDocMap.end())
    {
        return m_AiDocMap[aiName];
    }

    std::string filename = RES_PATH + aiName;

    TiXmlDocument* pXMLDoc = CnNew TiXmlDocument(filename.c_str());
    if(!pXMLDoc->LoadFile())
    {
        CRITICAL("ai file %s load failed\n", filename.c_str());
        CnDelete pXMLDoc;
        return NULL;
    }

    m_AiDocMap[aiName] = pXMLDoc;
    return pXMLDoc;
}

void AIManager::AddDebugListener(Player* listener, BattleUnit* target)
{
    uint64 playerID = listener->GetPlayerID();
    if(target)
    {
        uint32 targetID = target->GetId();
        m_DebuggerListenerMap[playerID] = targetID;
    }
    else
    {
        std::map<uint64, uint32>::iterator iter = m_DebuggerListenerMap.find(playerID);
        if(iter != m_DebuggerListenerMap.end())
            m_DebuggerListenerMap.erase(iter);
    }
}

void AIManager::OnAIChangeState(BattleUnit* pBattle, const std::string& ai_name, uint32 oldStateId, uint32 newStateId)
{
    // ai debug
    for(std::map<uint64, uint32>::iterator iter = m_DebuggerListenerMap.begin(); iter != m_DebuggerListenerMap.end(); iter++)
    {
        uint64 playerID = iter->first;
        uint32 targetID = iter->second;
        if(targetID == pBattle->GetId())
        {
            Player* player = PlayerManager::Instance()->GetPlayerByPlayerID(playerID);
            if(player)
            {
                AI_DEBUG_ACK ack;
                ack.ai_name = ai_name;
                ack.ai_state = newStateId;
                player->SendToGate(MSG_AI_DEBUG_ACK, &ack);
            }
        }
    }
}