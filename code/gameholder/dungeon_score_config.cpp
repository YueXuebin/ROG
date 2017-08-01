#include "gameholder_pch.h"
#include "dungeon_score_config.h"

//
//IMPLEMENT_SINGLETON(DungeonScoreConfig)
//
//DungeonScoreConfig::DungeonScoreConfig()
//{
//
//}
//
//DungeonScoreConfig::~DungeonScoreConfig()
//{
//
//}
//
//bool DungeonScoreConfig::LoadConfig( const char* path )
//{
//    TiXmlDocument xmlDoc;
//
//    if (!xmlDoc.LoadFile(path))
//    {
//        CnError("Load dungeon_score file: %s failed\n", path);
//        return false;
//    }
//
//    TiXmlElement* xmlRoot = xmlDoc.RootElement();
//    if(!xmlRoot) 
//        return false;
//
//    for(TiXmlElement* rootEle = xmlRoot->FirstChildElement(); rootEle; rootEle = rootEle->NextSiblingElement())
//    {
//        const char* nodeName = rootEle->Value();
//        if(Crown::SDStrcmp(nodeName, "score_list") == 0)
//        {
//            ScoreData scoreData;
//            for(TiXmlElement* scoreEle = rootEle->FirstChildElement(); scoreEle; scoreEle = scoreEle->NextSiblingElement())
//            {
//                int32 score = 0;
//                scoreEle->Attribute("score", &score);
//                if(score == 0)
//                    continue;
//
//                if(m_score_list.find(score) != m_score_list.end())
//                {
//                    CnWarn("score data %d already exist!\n", score);
//                    continue;
//                }
//
//                scoreData.Clear();
//                scoreEle->Attribute("need_point", &scoreData.needPoint);
//                scoreEle->Attribute("extra_reward_percent", &scoreData.extraRewardPercent);
//
//                m_score_list[score] = scoreData;
//            }
//        }
//        else if(Crown::SDStrcmp(nodeName, "specifydungeons") == 0)
//        {
//            if(!LoadSpecifyScore(rootEle))
//                CnWarn("can't load specify dungeon score!\n");
//        }
//        else if (Crown::SDStrcmp(nodeName, "dungeon_list") == 0)
//        {
//            DungeonScoreData scoreData;
//            for(TiXmlElement* dungeonEle = rootEle->FirstChildElement(); dungeonEle; dungeonEle = dungeonEle->NextSiblingElement())
//            {
//                int32 dungeonId = 0;
//                dungeonEle->Attribute("id", &dungeonId);
//                if(dungeonId == 0)
//                    continue;
//
//                if(m_dungeon_list.find(dungeonId) != m_dungeon_list.end())
//                {
//                    CnWarn("dungeon data %d already exist!\n", dungeonId);
//                    continue;
//                }
//
//                scoreData.Clear();
//                scoreData.id = dungeonId;
//                dungeonEle->Attribute("time", &scoreData.time);
//                dungeonEle->Attribute("monster_num", &scoreData.monsterNum);
//
//                m_dungeon_list[scoreData.id] = scoreData;
//            }
//        }
//    }
//
//    return true;
//}
//
//
//bool DungeonScoreConfig::LoadSpecifyScore(TiXmlElement* rootEle)
//{
//    for(TiXmlElement* dungeonEle = rootEle->FirstChildElement(); dungeonEle; dungeonEle = dungeonEle->NextSiblingElement())
//    {
//        uint32 dungeonId = 0;
//        dungeonEle->QueryUnsignedAttribute("id", &dungeonId);
//        int32 score = 0;
//        dungeonEle->QueryIntAttribute("score", &score);
//        if(m_dungeonsorelist.find(dungeonId)== m_dungeonsorelist.end())
//        {
//            m_dungeonsorelist[dungeonId] = score;
//        }
//        else
//        {
//            CnWarn("dungeon data %d already exist!\n", dungeonId);
//        }
//    }
//    return true;
//}
//
//DungeonScoreData* DungeonScoreConfig::GetDungeonScoreData( uint32 dungeonId )
//{
//    if(m_dungeon_list.find(dungeonId) == m_dungeon_list.end())
//        return NULL;
//
//    return &m_dungeon_list[dungeonId];
//}
//
//uint32 DungeonScoreConfig::GetScoreByPoint( int32 point )
//{
//    std::map<uint32, ScoreData>::reverse_iterator iter = m_score_list.rbegin();
//    for (; iter != m_score_list.rend(); iter++)
//    {
//        int32 num = iter->second.needPoint;
//        if (point >= iter->second.needPoint)
//        {
//            return iter->first;
//        }
//    }
//
//    // 最低C级
//    return REGULAR_SCORE_C;
//}
//
//uint32 DungeonScoreConfig::GetExtraRewardPercentByScore( int32 score )
//{
//    if(m_score_list.find(score) == m_score_list.end())
//        return 0;
//
//    return m_score_list[score].extraRewardPercent;
//}
//
//int32 DungeonScoreConfig::GetSpecifyDungeonScore(uint32 dungeonid)
//{
//    std::map<uint32,int32> ::iterator iterscore;
//
//    iterscore = m_dungeonsorelist.find(dungeonid);
//
//    if(iterscore != m_dungeonsorelist.end())
//        return iterscore->second;
//
//    return REGULAR_SCORE_EMPTY;
//}