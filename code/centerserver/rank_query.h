//=========================================================================
// ���в�ѯ���࣬Ŀǰ����ֻ�������ӿ�
//=========================================================================
#ifndef __Rank_Query_H__
#define __Rank_Query_H__

class RankListManager;

class RankQuery
{
public:
    virtual void Update() {};

    virtual void DailyRefresh() {};

    virtual void GMDailyRefresh() {};

    RankQuery(RankListManager *rankMgr);

    virtual ~RankQuery();

protected:
    RankListManager *m_rankMgr;
};

#endif