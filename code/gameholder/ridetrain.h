/*
 *	坐骑训练
 */ 
#ifndef ridetrain_h
#define ridetrain_h


struct TrainModes;

class RideTrain 
{
public:
    RideTrain(Player* player , RideManager* ridemg);

    virtual ~RideTrain(void);

    virtual void OnRecv(uint32 msgID, void* data);
    //花费磨石的处理
    void UseMoneyWay(uint32 itemType, uint32 itemID, uint32 itemNum);

    void SetTrainInfo(const RidetTrainList& ridelist);
    RidetTrainList& GetTrainInfo();

    void SetTrainField(void* data);                                     // 设置训练栏位
    void SetTrainModels(void* data);                                    // 设置训练模式开始训练
    void EndTrain(void* data);                                          // 完成训练
    void RightNowTrain(void* data);                                     // 立即完成训练

    void StarTrain(void* data);                                         // 训练石添加经验

    void SetTrainModels(uint32 fieldid, uint32 modelid);
    void SetTrainField(uint32 fieldid);
    void RightNowTrain(uint32 fieldid,uint32 petid);

    void TrainInit();                                                   // 坐骑训练
    void FieldOpenCondi();                                              // 栏位开启
    void FieldCloseCondi();                                             // 栏位关闭
    void NotifyTrainNum();                                              // 通知训练次数

private:
    void SendRideTrainList(int32 flag, RIDE_TRAIN* ridetrain = 0);                                          // 发送骑宠训练表
    void SendRideEndTrain(int32 flag,uint32 time,RIDE_TRAIN* ridetrain = 0,RIDE_PET_INFO* rideinfo = 0);    // 发送完成训练的消息
    void TrainLeveup(RIDE_PET_INFO& ride_pet);                                                              // 坐骑升级请求
    bool TrainModesOpen(uint32 id);                                                                         // 训练模式是否打开
    RIDE_TRAIN* GetTrainingPet(uint32 fieldid);                                                             // 得到骑宠训练
    void AddTrain(uint32 fieldid,int32 defaut);                                                             // 添加骑宠训练

    bool CanLeveUp(RIDE_PET_INFO* ride_pet);                            // 坐骑是否超过玩家等级

    int32 FindOpenTrainFields();                                        // 得到开启栅栏的个数
    void SendUpdateRideTrainList(RidetTrainList& updateTrainList);      // 发送坐骑训练栏位

    int32 CalNeedStoneNum(RIDE_PET_INFO* ride_pet);

private:
    RidetTrainList  m_rideTrainList;                    // 坐骑训练列表
    Player*         m_player;                           // 角色指针
    RideManager* ridemg;
};
#endif
