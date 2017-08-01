#ifndef gameworld_h__
#define gameworld_h__

class Player;

/*
*	游戏世界
*/
class GameWorld
{
public:
    GameWorld();
    ~GameWorld();

    bool Init();
    void Uninit();

    void Update(uint32 frame_count);

private:
    void OnPlayerMsg(uint32 sessionID, uint32 msgID, void* data);

    void SendEnterGameAck(uint32 sid, uint16 errCode);


};


#endif // gameworld_h__
