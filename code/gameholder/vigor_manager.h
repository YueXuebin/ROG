#ifndef vigor_manager_h__
#define vigor_manager_h__

class Player;
struct MealTimeTemplate;

// 体力系统
class VigorManager
{
public:
    VigorManager(Player* player);
    ~VigorManager();

    void    Update(uint32 frame_count);

    void Clear();
    void OnRecv(uint32 msgID, void* data);

    void LoadInfo(const VIGOR_INFO& info);
    void FillInfo(VIGOR_INFO& info);

    VIGOR_INFO& GetInfo() {return m_meal_vigor_info;}

    void RecvGetMealVigor(void* data);                                  ///< 领取饭点返回的体力

private:
    const MealTimeTemplate* GetMealVigorTemplate();                     ///< 从配置读取饭点体力

private:
    Player*                     m_player;
    VIGOR_INFO                  m_meal_vigor_info;
};

#endif // vigor_manager_h__





