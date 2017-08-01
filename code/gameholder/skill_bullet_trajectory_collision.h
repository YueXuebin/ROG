/*
 *	碰撞型弹道
 */
#ifndef skill_shoot_node_collision_h__
#define skill_shoot_node_collision_h__


const uint32 SHOOT_MAX_COLLISION = 15;          // 最大碰撞记录

class SkillBulletTrajectory_Collision
{
public:
    SkillBulletTrajectory_Collision();
    virtual ~SkillBulletTrajectory_Collision();

    virtual void Init(SkillLogicBullet* pBullet);

    virtual void Update();

    const CoVec2& GetPosition() {return m_BulletPosition;}
    int32   GetAngle() {return m_BulletAngle;}

private:
    
    void UpdateCollision();         // 碰撞型

    bool CheckCollisionID(uint32 object_id);
    bool FillCollisionID(uint32 object_id);

    
private:
    SkillLogicBullet*   m_Bullet;
    const SkillBulletData* m_BulletData;

    int32   m_ShootTime;                // 弹道时间
    CoVec2  m_ShootSpeed;               // 弹道速度

    CoVec2  m_BulletStartPosition;      // 子弹的起始位置
    CoVec2  m_BulletPosition;           // 子弹位置
    int32   m_BulletAngle;              // 子弹方向

    int32   m_PassTime;                 // 已经经过的时间(ms)

    int32   m_CollisionCounter;             // 碰撞次数计数

    int32   m_CollisionIDs[SHOOT_MAX_COLLISION];        // 碰撞对象ID记录
    int32   m_CollisionTimes[SHOOT_MAX_COLLISION];      // 碰撞时间记录
    

};


#endif // skill_shoot_node_collision_h__
