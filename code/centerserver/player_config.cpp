#include "centerserver_pch.h"
#include "player_config.h"
#include "config_loader.h"
#include "dungeon_manager.h"


IMPLEMENT_SINGLETON(PlayerConfig)

PlayerConfig::PlayerConfig()
{

}

PlayerConfig::~PlayerConfig()
{

}

bool PlayerConfig::LoadConfig( )
{
     return true;
}

void PlayerConfig::FillCreateProperyInfo( PROPERTY_INFO& info )
{
    // ������Ҵ��Ǻ�ĳ�����
    info.unionmap_id = UNION_ID(0, CITY_FIRST_MAP, 1);
    
    int32 diffx = rand()%100;           // ��һ����Χ�ڣ��������ĳ�����
    int32 diffy = rand()%100;

    if(rand()%1) diffx = -diffx;
    if(rand()%1) diffy = -diffy;

    info.x = -600 + diffx;
    info.y = 0 + diffy;
    info.angle = 180;

    // ��ʼ����
    info.game_point     = 0;
    info.money_point    = 0;
    info.vigor          = PLAYER_VIGOR_MAX;  // ����

    // ��ʼ��Ѫ��ħ
    info.hp = 999999;
    info.mp = 999999;


}

