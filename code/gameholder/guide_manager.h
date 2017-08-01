#ifndef guide_manager_h__
#define guide_manager_h__


class Player;

/*
 *	Òýµ¼
 */
class GuideManager
{
public:
    GuideManager(Player* player);
    ~GuideManager();

    void LoadGuideInfo(const GUIDE_INFO& info);
    void FillGuideInfo(GUIDE_INFO& info) const;

    void SetGuideInfo(const std::vector<uint32>& guide_info);

private:
    Player*     m_player;

    GUIDE_INFO  m_guide_info;

};


#endif // guide_manager_h__
