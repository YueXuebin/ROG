#pragma once


#include "action_base.h"


class RidePetAction :public ActionBase
{
public:
    RidePetAction(Robot* pRobot, int32 type, uint32 fieldid,uint32 modelid);
    ~RidePetAction(void);

    virtual void OnStart();
    virtual void OnRecv(uint32 msgID, void* data);

private:
    int32 m_type;
    uint32 m_fieldid;
    uint32 m_modeid;
    std::vector<RIDE_PET_INFO> m_ridepetlist; 
private:
    void RecruitPet(int32 petID);
    void sendgifPet(int32 petID);
    void RefRidePet();
    void StartTrain(int32 petid ,uint32 m_fieldid ,uint32 m_modeid);
    void RightNowTrain(uint32 m_fieldid,uint32 petid);
    void SendTrainPos(int32 fieldid);

};
