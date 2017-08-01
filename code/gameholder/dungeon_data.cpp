#include "gameholder_pch.h"
#include "dungeon_data.h"
#include "box2d_define.h"


uint32 GetDungeonClass(uint32 dungeonID)
{
    // 冒险地图
    if(dungeonID >= 2000 && dungeonID < 2500)
        return DUNGEON_CLASS_PLOT_ID;
    // 异界地图
    else if(dungeonID >= 2500 && dungeonID < 2900)
        return DUNGEON_CLASS_SOUL_ID;
    // 日常地图
    else if(dungeonID >= 2900 && dungeonID < 3000)
        return DUNGEON_CLASS_DAILY_ID;
    // 野外地图
    else if(dungeonID >= 1100 && dungeonID < 1200)
        return DUNGEON_CLASS_WORLD_ID;

    return DUNGEON_CLASS_NONE_ID;
}

// 可以飞的副本
bool IsDungeonCanFly(uint32 dungeonID)
{
    return true;
}

// 创建Box2D物理世界
b2World*    MakeBox2DWorld(const std::string& mapFileName)
{
    // 读取map文件
    TiXmlDocument m_Doc(mapFileName.c_str());

    if(!m_Doc.LoadFile())
        return NULL;

    b2World* pB2World = CnNew b2World(b2Vec2(0, 0));

    TiXmlHandle doc(&m_Doc);
    TiXmlHandle server_map = doc.FirstChild("scene_ex").FirstChild("server_map");

    b2Vec2 pFirst(0, 0);
    b2Vec2 p1(0, 0);
    b2Vec2 p2(0, 0);

    b2BodyDef bd;

    b2FixtureDef fixtureDef;
    fixtureDef.filter.categoryBits = PHY_CATEGORY_WALL;

    // 创建世界边界
    {
        b2Body* worldBody = pB2World->CreateBody(&bd);

        int32 index=0;
        TiXmlElement* worldElement = server_map.FirstChild("world").FirstChild("point").ToElement();
        for(; worldElement; worldElement = worldElement->NextSiblingElement())
        {
            worldElement->QueryFloatAttribute("x", &p2.x);
            worldElement->QueryFloatAttribute("z", &p2.y);

            if(index == 0)
                pFirst = p1 = p2;
            else
            {
                b2EdgeShape shape;
                shape.Set(p1, p2);
                fixtureDef.shape = &shape;
                fixtureDef.density = 0.0f;
                worldBody->CreateFixture(&fixtureDef);

                p1 = p2;
            }

            index++;
        }

        if(index > 0)
        {
            b2EdgeShape shape;
            shape.Set(p2, pFirst);
            fixtureDef.shape = &shape;
            fixtureDef.density = 0.0f;
            worldBody->CreateFixture(&fixtureDef);
        }
    }

    // 创建墙
    for(TiXmlNode* wallNode = server_map.FirstChild("wall").ToNode(); wallNode; wallNode = wallNode->NextSibling("wall"))
    {
        TiXmlElement* wallElement = wallNode->FirstChild("point")->ToElement();

        b2Body* wallBody = pB2World->CreateBody(&bd);

        int32 index=0;
        for(; wallElement; wallElement = wallElement->NextSiblingElement())
        {
            wallElement->QueryFloatAttribute("x", &p2.x);
            wallElement->QueryFloatAttribute("z", &p2.y);

            if(index == 0)
                pFirst = p1 = p2;
            else
            {
                b2EdgeShape shape;
                shape.Set(p1, p2);
                fixtureDef.shape = &shape;
                fixtureDef.density = 0.0f;
                wallBody->CreateFixture(&fixtureDef);

                p1 = p2;
            }

            index++;
        }

        if(index > 0)
        {
            b2EdgeShape shape;
            shape.Set(p2, pFirst);
            fixtureDef.shape = &shape;
            fixtureDef.density = 0.0f;
            wallBody->CreateFixture(&fixtureDef);
        }
    }

    // 创建洞
    for(TiXmlNode* holeNode = server_map.FirstChild("hole").ToNode(); holeNode; holeNode = holeNode->NextSibling("hole"))
    {
        TiXmlElement* holeElement = holeNode->FirstChild("point")->ToElement();

        b2Body* holeBody = pB2World->CreateBody(&bd);

        int32 index=0;
        for(; holeElement; holeElement = holeElement->NextSiblingElement())
        {
            holeElement->QueryFloatAttribute("x", &p2.x);
            holeElement->QueryFloatAttribute("z", &p2.y);

            if(index == 0)
                pFirst = p1 = p2;
            else
            {
                b2EdgeShape shape;
                shape.Set(p1, p2);
                fixtureDef.shape = &shape;
                fixtureDef.density = 0.0f;
                holeBody->CreateFixture(&fixtureDef);

                p1 = p2;
            }

            index++;
        }

        if(index > 0)
        {
            b2EdgeShape shape;
            shape.Set(p2, pFirst);
            fixtureDef.shape = &shape;
            fixtureDef.density = 0.0f;
            holeBody->CreateFixture(&fixtureDef);
        }
    }


    return pB2World;
}