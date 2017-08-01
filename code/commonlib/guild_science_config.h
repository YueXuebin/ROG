#ifndef guild_science_config_h__
#define guild_science_config_h__

// �Ƽ��ȼ��Բ����ӳ�ģ��
typedef struct GuildPromoteLevelTmpl
{
    void Clear()
    {
        level       = 0;
        addvalue    = 0;
        exp         = 0;
    }

    int32               level;      // �Ƽ��ȼ�
    int32               addvalue;   // �ӳ���ֱ�
    int32               exp;        // ������һ�����辭����
}GuildPromoteLevelTmpl;

// ����ӳ�����ģ��
typedef struct GuildAdditionTmpl
{
    void Clear()
    {
        id          = 0;
        maxlevel    = 0;
        levels.clear();
    }

    int32               id;         // �Ƽ�����
    int32               maxlevel;   // ���ȼ�
    std::vector<GuildPromoteLevelTmpl>        levels;       // �ȼ��Բ����ļӳ��б�
}GuildAdditionTmpl;
typedef std::vector<GuildAdditionTmpl> GuildAdditionVert;   // �Ƽ��ӳ�

// ���˾��״���ģ��
typedef struct GuildPromoteDonatePointTmpl
{
    void Clear()
    {
        times               = 0;
        guild_point         = 0;
        exp                 = 0;
        donate_num          = 0;
    }

    int32               times;              // ���״���
    int32               guild_point;        // ���ӹ��׵���
    int32               exp;                // ���ӿƼ�������
    int32               donate_num;         // ���ѻ�����
}GuildPromoteDonatePointTmpl;
typedef std::vector<GuildPromoteDonatePointTmpl> GuildPromoteDonatePointVert;       // �����Ƽ����Ĳ����б�(�������仯)

// ����Ƽ�
class GuildScienceConfig
{
public:
    GuildScienceConfig();
    ~GuildScienceConfig();

    DECLARE_SINGLETON(GuildScienceConfig)

    bool LoadConfig(const char* path);

    const GuildAdditionVert& GetGuildAdditions(){return m_additions;}                       // �õ��Ƽ��ӳɱ�
    const GuildPromoteDonatePointVert& GetPromoteGamePt() {return m_promoteGamePoints;}     // �õ���������Ƽ����������
    const GuildPromoteDonatePointVert& GetPromoteMoneyPt() {return m_promoteMoneyPoints;}   // �õ�ħʯ�����Ƽ����������

    const GuildAdditionTmpl* GetScienceAdditionTmpl(int32 id);                  // �ɿƼ�ID�õ��Ƽ��ӳ���Ϣ
    const GuildPromoteLevelTmpl* GetPromoteLevelTmpl(int32 id, int32 level);    // �õ�һ��Ƽ��ȼ��Բ����ӳ���Ϣ

    const GuildPromoteDonatePointTmpl* GetPromoteDonateGamePointTmpl(int32 times);  // �ɴ����õ��������ģ��
    const GuildPromoteDonatePointTmpl* GetPromoteDonateMoneyPointTmpl(int32 times);  // �ɴ����õ��������ģ��
private:
    bool LoadPromote(TiXmlElement* xmlEle);                     // ���ع���Ƽ��ӳ�
    bool LoadDonateGamePoint(TiXmlElement* xmlEle);             // ���ظ��˽�������Ƽ�ģ��
    bool LoadDonateMoneyPoint(TiXmlElement* xmlEle);            // ���ظ���ħʯ�����Ƽ�ģ��

private:
    GuildAdditionVert   m_additions;                    // �Ƽ��ӳɱ�

    GuildPromoteDonatePointVert m_promoteGamePoints;    // ��������Ƽ����������
    GuildPromoteDonatePointVert m_promoteMoneyPoints;   // ħʯ�����Ƽ����������
};

#endif // guild_science_config_h__
