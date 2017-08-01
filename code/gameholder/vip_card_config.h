/*
 *	vipʱЧ��
 */
#ifndef vip_card_config_h__
#define vip_card_config_h__

typedef struct VipCardTemplate
{
    uint32                  id;		            ///< �ȼ�
    uint32                  duration;           ///< ʱ������λ��
    uint32                  money_point;        ///< ��������ħʯ
    uint32                  vip;                ///< �����ֱͨ��VIP��Ȩ�ȼ�
    std::string             name;               ///< ����

    void Clear()
    {
        id                  = 0;
        duration            = 0;
        money_point         = 0;
        vip                 = 0;
        name                = "";
    }
} VipCardTemplate;

class VipCardConfig
{
public:
    VipCardConfig();
    ~VipCardConfig();

    DECLARE_SINGLETON(VipCardConfig)

    bool LoadConfig(const char* path);

    const VipCardTemplate*  GetVipCardTemplate(uint32 cardId);

    uint32 GetVipCardDuration(uint32 cardId);           ///< ���ʱ��
    uint32 GetVipCardMoneyPoint(uint32 cardId);         ///< ��ù�������ħʯ
    uint32 GetVipCardDirectVIP(uint32 cardId);          ///< ��ù���VIPʱЧ����ֱͨ��VIP��Ȩ�ȼ�

private:
    std::map<uint32, VipCardTemplate> m_vip_cards;
};

#endif // vip_card_config_h__
