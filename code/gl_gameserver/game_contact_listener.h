#ifndef game_contact_listener_h__
#define game_contact_listener_h__

class GlGameServer;
struct Settings;

const int32 k_maxContactPoints = 2048;          // �����ʾ����ײ����

// ��ʾ��ײ�������ݽṹ
struct ContactPoint
{
    b2Fixture* fixtureA;
    b2Fixture* fixtureB;
    b2Vec2 normal;
    b2Vec2 position;
    b2PointState state;         // ��ײ��״̬
};

class GameContactListener : public b2ContactListener
{
public:
    GameContactListener(GlGameServer* gs);
    virtual ~GameContactListener() {}

    // 
    virtual void BeginContact(b2Contact* contact);
    //
    virtual void EndContact(b2Contact* contact);
    //
    virtual void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
    //
    virtual void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);

    void Reset();

    // ����ײ��
    void DrawContactPoints(Settings* settings);

private:
    ContactPoint m_points[k_maxContactPoints];
    int32 m_pointCount;

    GlGameServer*   m_GameServer;
};


#endif // game_contact_listener_h__
