#ifndef log_writer_h__
#define log_writer_h__

// 参加/离开全服活动
std::string WriterToLogByActivity(int32 playerLevel, int32 activityType, int32 intostate);
// 魔石消费(原因，物品类型，物品数量，物品id，订单号，订单状态，总价，单价，现有魔石)
std::string WriterToLogByMoneyPoint(int32 reason, int32 itemType, int32 itemNum, int32 itemId, uint64 orderId, int32 orderState, int32 totalPrice, int32 unitPrice, int32 moneyPoint);
// 魔石充值(充值类型，数量，原魔石数，现魔石数)
std::string WriterToLogByMoneyPointPay(int32 payType, int32 num, int32 oldNum, int32 newNum);
// 礼金(数量，原因，原礼金数，现礼金数)
std::string WriterToLogByBindMoneyPoint(int32 value, int32 reason, int32 oldValue, int32 newValue);
// 进入/离开副本
std::string WriterToLogByDungeon(uint32 dungeomId, int32 isEnter);
// 增加/减少金币(数量，原因，原数量，现数量)
std::string WriterToLogByGamePoint(int32 value, int32 reason, int32 oldValue, int32 newValue);
// 增加/减少物品(操作，物品id，物品数量，原因，现数量)
std::string WriterToLogByItem(int32 opt, int32 itemId, int32 itemNum, int32 reason, int32 nowNum);
// 购买服务
std::string WriterToLogByService(int32 serviceId, int32 num);
// 登陆日志
std::string WriterToLogByLogin(uint32 loginTime, const std::string& platFromUname, const std::string& playerName, uint32 loginIp);
// 增加/减少精魂(数量，原因，原数量，现数量)
std::string WriterToLogBySoulPoint(int32 value, int32 reason, int32 oldValue, int32 newValue);
// 玩家状态变迁
std::string WriterToLogByPlayerState(const std::string& playerName, int32 newState, int32 OldState);
// 玩家经验增加
std::string WriterToLogByExp(int32 addExp, int32 curExp, int32 maxExp, int32 level);
// 任务状态改变
std::string WriterToLogByTask(int32 level, int32 taskType, int32 taskId, int32 taskState, int32 taskQuality);
// 创建角色
std::string WriterToLogByCreatePlayer(uint32 createTime, const std::string& platFromUname, const std::string& playerName, int8 sex, int8 career, uint32 login_ip);
// PCU
std::string WriterToLogPcu(int32 pcu);
// 玩法系统日志
std::string WriterToLogByPlaySystem(int32 playSystemId, int32 arg1=0, int32 arg2=0, const std::string& argstr="");
// 公会日志
std::string WriterToLogByGuild(uint64 guildId, uint32 eventId, const std::string& playerName, int32 arg1);
// 财产日志
std::string WriterToLogByWealth(int32 wealthId, int32 value, int32 oldValue, int32 newValue);

#endif // log_writer_h__
 
