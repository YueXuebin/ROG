#ifndef tower_defence_manager_h__
#define tower_defence_manager_h__


class TowerDefenceManager
{
public:
    TowerDefenceManager();
    ~TowerDefenceManager();

    DECLARE_SINGLETON(TowerDefenceManager)

    void Clear();

    bool    CanEnter();

    void    Update();
    void    OnRecv(uint32 msgID, void* data);

    uint32  GetCurrTime() {return m_currTime;}                  // ºÁÃë
    bool    IsOpen() {return m_IsOpen;}
private:
    // »î¶¯¿ªÆô
    void    OnActivityStart(void* data);
    void    OnActivityEnd();
private:
    uint32  m_currTime;
    bool    m_IsOpen;
};

#endif // tower_defence_manager_h__
