#ifndef common_util_h
#define common_util_h

// 根据命令行参数创建名字
std::string MakeCmdName(int argc, char* argv[]);

// 检查是否已经有存在的命名事件
Crown::CSDNamedSemaphoreObject CheckNamedSemaphore(int argc, char* argv[]);

// 比较时间是否相同
bool IsSameDay(uint32 recordlasttime);

bool IsSame64(const BIGINT_ID& a, const BIGINT_ID& b);

// 活动ID推算
uint32 CalFestivalTaskID(uint32 festivalID, uint32 taskID);     // 计算活动的任务ID=活动ID*100+子任务id+50000
uint32 CalOriginFestivalID(uint32 taskID);                      // 通过任务ID->计算活动ID
uint32 CalOriginFestivalTaskID(uint32 taskID);                  // 根据任务ID->活动子任务ID

// 计算Gate服务器的RegionID(由CenterID和GateID组合而成)
uint32 CalRegionID(uint32 centerID, uint32 gateID);
uint32 CalGateIDByRegionID(uint32 regionID);
uint32 CalCenterIDByRegionID(uint32 regionID);
uint32 CalGameIDByRegionID(uint32 regionID);


// 服务器名和区号互转
int32 ServerNameToRegionID(const std::string& serverName);
std::string RegionIDToServerName(int32 regionID);
// 修正服务器名
std::string MakeUpServerName(const std::string& serverName );

// 单服组队的最大人数定义
uint32 GetMaxTeamMenber(uint32 teamType);

// 奖励倍数
void MultiRewardTable(REWARD_TABLE* rewardTable, real multiple);
void MultiRewardItems(std::vector<REWARD_ITEM>* rewardTable, real multiple);


#endif // common_util__
