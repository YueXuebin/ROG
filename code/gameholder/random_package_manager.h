/*
*   Ëæ»ú°ü¹ü
*/
#ifndef random_package_manager_h__
#define random_package_manager_h__

class Player;
struct RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT;
struct RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT;

const uint32 MAX_LOOP_COUNT = 50;

const RANDOM_PACKAGE_RECORD* GetPackageRecord(uint32 package_id, const RANDOM_PACKAGE_RECORD_INFO* pInfo);
const RANDOM_PACKAGE_CARD_RECORD* GetPackageCardRecord(uint32 card_id, const RANDOM_PACKAGE_RECORD* pInfo);

void AddPackageRecord(uint32 package_id, RANDOM_PACKAGE_RECORD_INFO& info);
void AddCardRecord(uint32 package_id, uint32 card_id, RANDOM_PACKAGE_RECORD_INFO& info);

bool CheckCD(uint32 cd, const RANDOM_PACKAGE_CARD_RECORD* pCardRecord);

class ServerRandomPackageManager
{
public:
    ServerRandomPackageManager();
    ~ServerRandomPackageManager() {}

    DECLARE_SINGLETON(ServerRandomPackageManager)

    void    LoadInfo(const RANDOM_PACKAGE_RECORD_INFO& info) {m_RandomPackageRecordInfo = info;}
    void    FillInfo(RANDOM_PACKAGE_RECORD_INFO& info) const {info = m_RandomPackageRecordInfo;}

    void OnRecv(uint32 msgID, void* data);

    void Update();

    void SendPackageRecordToCenter();

    RANDOM_PACKAGE_RECORD_INFO& GetInfo() {return m_RandomPackageRecordInfo;}

    bool GetDirty() const {return m_bDirty;}
    void SetDirty() {m_bDirty = true;}
private:
    RANDOM_PACKAGE_RECORD_INFO      m_RandomPackageRecordInfo;
    bool                            m_bDirty;
};

class RandomPackageManager
{
public:
    RandomPackageManager(Player* player);
    ~RandomPackageManager();

    void    LoadInfo(const RANDOM_PACKAGE_RECORD_INFO& info);
    void    FillInfo(RANDOM_PACKAGE_RECORD_INFO& info) const;

    bool    GetPackage(const uint32 reason, const uint32 pkgID, REWARD_TABLE & table, const int32 loopCount = 0);

private:
    const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* RandomItem(const uint32 reason,
        const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pPackage, const std::set<uint32> & limitSet);

    void    GetRandomList(const uint32 reason, const RANDOM_PACKAGE_ROOT_PACKAGE_STRUCT* pPackage, const std::set<uint32> & limitSet,
        uint32& totalRate, std::vector<const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT*>& randomList);

    bool    CheckRepeat(const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard, const std::set<uint32> & limitSet);
    bool    CheckPersonalCD(const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard, const RANDOM_PACKAGE_RECORD* pPackageRecord);
    bool    CheckServerCD(uint32 package_id, const RANDOM_PACKAGE_ROOT_PACKAGE_ITEM_STRUCT* pCard);

private:
     Player*                        m_player;
     RANDOM_PACKAGE_RECORD_INFO     m_RandomPackageRecordInfo;
};


#endif // random_package_manager_h__
