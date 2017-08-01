/******************************************************************************
Copyright (C) Doingame Corporation. All rights reserved.
* @brief ������Ϸ����
******************************************************************************/

#ifndef vigor_config_h__
#define vigor_config_h__

#include <map>

class Player;

typedef struct MealTimeTemplate
{
    void Clear()
    {
        start_time = "";
        end_time = "";
        add_vigor = 0;
    }

    std::string     start_time;  // ���㿪ʼʱ��
    std::string     end_time;   //  �������ʱ��
    uint32  add_vigor;  //  ��������������
}MealTimeTemplate;

class VigorConfig
{
public:
    VigorConfig();
    virtual ~VigorConfig();

    DECLARE_SINGLETON(VigorConfig)

    void Clear();                               ///< ���

    bool LoadConfig(const char* path);          ///< ���������ļ�

    uint32 GetVigorInterval() { return m_vigor_interval; }      // �������ˢ��ʱ����
    int32  GetVigorAddVal() { return m_vigor_addval; }          // �����������ֵ

    int32 GetMaxVigor(Player* player);          ///< ������ȼ�

    int32 GetMealVigorStartLevel() {return m_meal_vigor_start_level;}   // ��ÿ�ʼ��ȡ����ĵȼ�

    const std::map<uint32, MealTimeTemplate>& GetMealTime() { return m_mealtime_map; }	//��ȡ���з��㷵����ʱ��

private:
    std::map<int32, int32>  m_vigor_map;        ///< �ȼ����������
    int32                   m_max_level;        ///< ���ȼ���Ӧ��

    uint32                  m_vigor_interval;   ///< ����������
    int32                   m_vigor_addval;     ///< ���������ֵ

    std::map<uint32, MealTimeTemplate>  m_mealtime_map;        ///< ���еķ���ʱ��

    int32                   m_meal_vigor_start_level;         ///< ��ʼ��ȡ����ĵȼ�
};

#endif
