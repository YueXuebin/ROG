#ifndef common_util_h
#define common_util_h

// ���������в�����������
std::string MakeCmdName(int argc, char* argv[]);

// ����Ƿ��Ѿ��д��ڵ������¼�
Crown::CSDNamedSemaphoreObject CheckNamedSemaphore(int argc, char* argv[]);

// �Ƚ�ʱ���Ƿ���ͬ
bool IsSameDay(uint32 recordlasttime);

bool IsSame64(const BIGINT_ID& a, const BIGINT_ID& b);

// �ID����
uint32 CalFestivalTaskID(uint32 festivalID, uint32 taskID);     // ����������ID=�ID*100+������id+50000
uint32 CalOriginFestivalID(uint32 taskID);                      // ͨ������ID->����ID
uint32 CalOriginFestivalTaskID(uint32 taskID);                  // ��������ID->�������ID

// ����Gate��������RegionID(��CenterID��GateID��϶���)
uint32 CalRegionID(uint32 centerID, uint32 gateID);
uint32 CalGateIDByRegionID(uint32 regionID);
uint32 CalCenterIDByRegionID(uint32 regionID);
uint32 CalGameIDByRegionID(uint32 regionID);


// �������������Ż�ת
int32 ServerNameToRegionID(const std::string& serverName);
std::string RegionIDToServerName(int32 regionID);
// ������������
std::string MakeUpServerName(const std::string& serverName );

// ������ӵ������������
uint32 GetMaxTeamMenber(uint32 teamType);

// ��������
void MultiRewardTable(REWARD_TABLE* rewardTable, real multiple);
void MultiRewardItems(std::vector<REWARD_ITEM>* rewardTable, real multiple);


#endif // common_util__
