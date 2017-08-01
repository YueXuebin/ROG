#include "commonlib_pch.h"
#include "log_writer.h"

// �μ�/�뿪ȫ���
std::string WriterToLogByActivity(int32 playerLevel, int32 activityType, int32 intostate)
{
    return Crown::ToString(playerLevel) + "\t" + Crown::ToString(activityType) + "\t" + Crown::ToString(intostate);
}

// ħʯ����
std::string WriterToLogByMoneyPoint(int32 reason, int32 itemType, int32 itemNum, int32 itemId, uint64 orderId, int32 orderState, int32 unitPrice, int32 totalPrice, int32 moneyPoint)
{
    return Crown::ToString(reason)+ "\t" + Crown::ToString(itemType) + "\t" + Crown::ToString(itemNum) +"\t" + Crown::ToString(itemId) + "\t" +
            Crown::ToString(orderId) + "\t" + Crown::ToString(orderState) + "\t" + Crown::ToString(unitPrice)+ "\t" + Crown::ToString(totalPrice) + "\t" + 
            Crown::ToString(moneyPoint);
}

// ħʯ��ֵ
std::string WriterToLogByMoneyPointPay(int32 payType, int32 num, int32 oldNum, int32 newNum)
{
    return Crown::ToString(payType) + "\t" + Crown::ToString(num) + "\t" + Crown::ToString(oldNum) + "\t" + Crown::ToString(newNum);
}

// ���
std::string WriterToLogByBindMoneyPoint(int32 value, int32 reason, int32 oldValue, int32 newValue)
{
    return Crown::ToString(value) + "\t" + Crown::ToString(reason) + "\t" + Crown::ToString(oldValue) + "\t" + Crown::ToString(newValue);
}

// ����/�뿪����
std::string WriterToLogByDungeon(uint32 dungeomId, int32 isEnter)
{
    return Crown::ToString(dungeomId)+ "\t" + Crown::ToString(isEnter);
}

// ����/���ٽ��
std::string WriterToLogByGamePoint(int32 value, int32 reason, int32 oldValue, int32 newValue)
{
    return Crown::ToString(value) + "\t" + Crown::ToString(reason) + "\t" + Crown::ToString(oldValue) + "\t" + Crown::ToString(newValue);
}

// ����/���پ���
std::string WriterToLogBySoulPoint(int32 value, int32 reason, int32 oldValue, int32 newValue)
{
    return Crown::ToString(value) + "\t"+ Crown::ToString(reason)  + "\t" + Crown::ToString(oldValue) + "\t" + Crown::ToString(newValue);
}

// ����/����/����, ��Ʒid, ��Ʒ����, ԭ��
std::string WriterToLogByItem(int32 opt, int32 itemId, int32 itemNum, int32 reason, int32 nowNum)
{
    return Crown::ToString(opt)+ "\t"+ Crown::ToString(itemId)+ "\t" + Crown::ToString(itemNum)+ "\t" + Crown::ToString(reason) + "\t" + Crown::ToString(nowNum);
}

// ʹ�÷���
std::string WriterToLogByService(int32 serviceId, int32 num)
{
    return Crown::ToString(serviceId) + "\t" + Crown::ToString(num);
}

// ��½��־
std::string WriterToLogByLogin(uint32 loginTime, const std::string& platFromUname, const std::string& playerName, uint32 loginIP)
{
    return  Crown::ToString(loginTime)+ "\t"+platFromUname+ "\t" + playerName+ "\t" + Crown::ToString(loginIP);
}

// ���״̬��Ǩ
std::string WriterToLogByPlayerState(const std::string& playerName, int32 newState, int32 oldState)
{
    return playerName + "\t" + Crown::ToString(newState) + "\t" + Crown::ToString(oldState);
}

// ��Ҿ�������
std::string WriterToLogByExp(int32 addExp, int32 curExp, int32 maxExp, int32 level)
{
    return Crown::ToString(addExp) + "\t" + Crown::ToString(curExp) + "\t" + 
           Crown::ToString(maxExp) + "\t" + Crown::ToString(level);
}
// ����״̬
std::string WriterToLogByTask( int32 level, int32 taskType, int32 taskId, int32 taskState, int32 taskQuality )
{
    return Crown::ToString(level) + "\t" + Crown::ToString(taskType) + "\t" +
        Crown::ToString(taskId) + "\t" + Crown::ToString(taskState) + "\t" +
        Crown::ToString(taskQuality);
}
// ������ɫ
std::string WriterToLogByCreatePlayer(uint32 createTime, const std::string& platFromUname, const std::string& playerName, int8 sex, int8 career, uint32 login_ip)
{
    return  Crown::ToString(createTime) + "\t"+platFromUname + "\t" + playerName+ "\t" +
        Crown::ToString(sex) + "\t" + Crown::ToString(career) +"\t" + Crown::ToString(login_ip);
}
// PCU
std::string WriterToLogPcu(int32 pcu)
{
    return  Crown::ToString(pcu);
}
// �淨��־
std::string WriterToLogByPlaySystem(int32 playSystemId, int32 arg1, int32 arg2, const std::string& argstr)
{
    return  Crown::ToString(playSystemId) + "\t" + Crown::ToString(arg1) + "\t" + Crown::ToString(arg2)+ "\t" + argstr;
}
// ������־
std::string WriterToLogByGuild(uint64 guildId, uint32 eventId, const std::string& playerName, int32 arg1)
{
    return  Crown::ToString(guildId) + "\t" + Crown::ToString(eventId) + "\t" + playerName+ "\t" + Crown::ToString(arg1);
}
// �Ʋ���־
std::string WriterToLogByWealth(int32 wealthId, int32 value, int32 oldValue, int32 newValue)
{
    return Crown::ToString(wealthId) + "\t" + Crown::ToString(value) + "\t" + Crown::ToString(oldValue) + "\t" + Crown::ToString(newValue);
}