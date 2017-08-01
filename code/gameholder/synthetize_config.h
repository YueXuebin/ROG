/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief ������Ϸ����
******************************************************************************/

#ifndef synthetize_config_h__
#define synthetize_config_h__

// �ϳɻ���
typedef struct SynthetizeCost
{
    int32 lower_item_num;       // �ͼ���������
    int32 material_item_id;     // ���ϵ���id
    int32 material_item_num;    // ���ϵ�������
    int32 game_point;           // ���Ľ��
    int32 critical_ratio;       // ��������

    void Clear()
    {
        lower_item_num = 0;
        material_item_id = 0;
        material_item_num = 0;
        game_point = 0;
        critical_ratio = 0;
    }
}SynthetizeCost;

// �ϳɱ�ʯ
typedef struct SynthetizeJewel
{
    int32 level;
    std::vector<SynthetizeCost> list;
}SynthetizeJewel;

class SynthetizeConfig
{
public:
    SynthetizeConfig();
    virtual ~SynthetizeConfig();

    DECLARE_SINGLETON(SynthetizeConfig)

    bool LoadConfig(const char* path);                  ///< ��ȡ�����ļ�

    bool CanUseJewel(int32 jewel_level, int32 player_level);            ///< ʹ�ñ�ʯ

    //bool CanSynthetizeJewel(int32 jewel_level, int32 player_level);     ///< �ϳɱ�ʯ

    SynthetizeJewel* GetSynthJewel(int32 level);        ///< ���ݱ�ʯ�ĵȼ���úϳ��ʺͻ���

    uint32 GetCirtTime() {return cirt_time;}
    std::string GetPublicString() {return m_public;}


private:

    std::map<int32, int32>  m_use_jewel_level;                  ///< ʹ�ñ�ʯ�Ľ�ɫ�ȼ�����

    //std::map<int32, int32>  m_synthetize_jewel_level;           ///< �ϳɱ�ʯ�Ľ�ɫ�ȼ�����

    typedef std::map<int32, SynthetizeJewel> SynthJewelMap;     ///< [�ȼ�,�ϳɱ�ʯ�б�]
    SynthJewelMap m_synthjewel_map;                             ///< �ϳɱ�ʯӳ���

    uint32 cirt_time;                                          ///<.������ʱ�䷢һ�ι���
    std::string m_public;                                      ///<.��������
};

#endif
