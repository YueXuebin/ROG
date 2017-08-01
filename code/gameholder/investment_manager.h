#ifndef investment_manager_h__
#define investment_manager_h__

class Player;
class InvestmentManager
{
public:
    enum InvestmentType
    {
        TYPE_NONE = 0,
        TYPE_DAY_INVESTMENT,
        TYPE_LEVEL_INVESTMENT
    };

    enum InvestmentOperation
    {
        OPERATION_NONE = 0,
        OPERATION_STORE,
        OPERATION_FETCH
    };

    InvestmentManager();
    ~InvestmentManager();
    DECLARE_SINGLETON(InvestmentManager)

    void OnRecv(Player* player, uint32 msgID, void* data);

    void InvestmentOperationNTF(Player* player, INVESTMENT_OPERATION_NTF* info);

private:
    void OnInvestmentOperationReq(Player* player, uint32 msgID, void* data);

};

#endif