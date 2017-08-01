#include "centerserver_pch.h"
#include "rank_query.h"

RankQuery::RankQuery(RankListManager *rankMgr) : m_rankMgr(rankMgr)
{
}

RankQuery::~RankQuery()
{
    m_rankMgr = NULL;
}