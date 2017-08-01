/*
 *	指向型弹道
 */
#ifndef skill_shoot_node_direct_h__
#define skill_shoot_node_direct_h__

class SkillLogicBullet;
class SkillBulletData;

class SkillBulletTrajectory_Direct
{
public:
    SkillBulletTrajectory_Direct();
    ~SkillBulletTrajectory_Direct();

    void Init(SkillLogicBullet* pBullet);

    void Update();

    const CoVec2& GetPosition() {return m_BulletPosition;}
    int32   GetAngle() {return m_BulletAngle;}

private:
    void UpdatePosition();

    void ShootEnd();


private:
    SkillLogicBullet*   m_Bullet;
    const SkillBulletData*  m_BulletData;

    int32   m_ShootTime;        // 弹道时间
    int32   m_PassTime;         // 已经经过的时间(ms)

    CoVec2  m_BulletStartPosition;  // 子弹的起始位置
    CoVec2  m_BulletPosition;       // 子弹位置
    CoVec2  m_BulletEndPostion;     // 子弹的终点位置

    int32   m_BulletAngle;          // 子弹方向

    
};


#endif // skill_shoot_node_direct_h__