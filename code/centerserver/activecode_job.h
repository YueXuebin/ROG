class DBExecutor;
class DBRecorder;

class Player;

#ifndef activecode_job_h__
#define activecode_job_h__

class ActiveCodeJob
{
    ActiveCodeJob() { m_dbGameExecutor = 0; m_dbGlobalExecutor = 0; }
    ~ActiveCodeJob() { }

    DECLARE_SINGLETON(ActiveCodeJob)

    /// 设置SQL执行
    void SetDBExecutor(DBExecutor* val, DBExecutor* gval) { m_dbGameExecutor = val; m_dbGlobalExecutor = gval;}

    // 使用兑换码
    void UseGiftCode(uint64 playerID, std::string giftCode);

private:

    void CBGetUsedGiftCode(DBRecorder& res, uint64 playerID, std::string giftCode, uint32 giftType);

    /// 获得兑换码信息
    void GetGiftCodeInfo(uint64 playerID, std::string giftCode, std::vector<std::string> usedGiftIDs, uint32 giftType);

    void CBGetGiftCodeInfo(DBRecorder& res, uint64 playerID, std::string activeCode, std::vector<std::string> usedGiftIDs, uint32 giftType);     ///< 查询兑换码信息回调

    /// 使用兑换码
    void _UseGiftCode(uint64 playerID, std::string playerName, uint32 gameSvrID, std::string giftCode, int32 giftID, uint32 giftType);

    /// 更新兑换码使用信息
    void SetActiveCodeUseInfo(std::string giftCode, uint64 playerID, uint32 giftType);

private:
    ActiveCodeJob( const ActiveCodeJob& );
    ActiveCodeJob& operator=( const ActiveCodeJob& );
    ActiveCodeJob* operator&();
    const ActiveCodeJob* operator&() const;

    void CBUseGiftCode(DBRecorder& res, uint64 playerID, std::string playerName, uint32 gameSvrID, 
                            std::string giftCode, int32 giftID, uint32 giftType);          ///< 使用兑换码

    void CBSetActiveCodeUseInfo(DBRecorder& res, uint64 playerID);                                           ///< 更新兑换码使用信息回调

    void EnterUse(uint64 playerID);
    void LeaveUse(uint64 playerID);
    bool IsInUse(uint64 playerID);

private:
    DBExecutor*         m_dbGameExecutor;                                                       ///< SQL执行器
    DBExecutor*         m_dbGlobalExecutor;                                                       ///< SQL执行器

    std::vector<uint64>     m_InUsedPlayers;
};

#endif
