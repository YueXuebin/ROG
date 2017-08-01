#include "CollisionPCH.h"
#include "CoWorld.h"

CoWorld* MakeCollisionWorld(const char* mapFileName)
{
    TiXmlDocument m_Doc(mapFileName);
    
    if(!m_Doc.LoadFile())
        return NULL;

    TiXmlHandle doc(&m_Doc);
    TiXmlHandle server_map = doc.FirstChild("scene_ex").FirstChild("server_map");

    // 世界包围框
    TiXmlElement* borderElement = server_map.FirstChild("scenebound").ToElement();    
    real minX, maxX, minY, maxY;
    borderElement->QueryFloatAttribute("minX", &minX);
    borderElement->QueryFloatAttribute("maxX", &maxX);
    borderElement->QueryFloatAttribute("minY", &minY);
    borderElement->QueryFloatAttribute("maxY", &maxY);

    CoPolygon* pWorldBorder = CoPolygon::Rect(CoVec2(minX, minY), CoVec2(maxX, maxY));
    CoWorld* pNewWorld = CnNew CoWorld(pWorldBorder, 500);
     
    // world三角
    TiXmlElement* worldElement = server_map.FirstChild("worldtri").FirstChild("tri").ToElement();
    for(; worldElement; worldElement = worldElement->NextSiblingElement())
    {
        CoPolygon* pWorldWall = CnNew CoPolygon;

        CoVec2 tempVec2;

        real v0x, v0y, v1x, v1y, v2x, v2y;
        worldElement->QueryFloatAttribute("v0x", &v0x);
        worldElement->QueryFloatAttribute("v0y", &v0y);
        worldElement->QueryFloatAttribute("v1x", &v1x);
        worldElement->QueryFloatAttribute("v1y", &v1y);
        worldElement->QueryFloatAttribute("v2x", &v2x);
        worldElement->QueryFloatAttribute("v2y", &v2y);

        tempVec2.x = v0x;
        tempVec2.y = v0y;
        pWorldWall->append(tempVec2);
        tempVec2.x = v1x;
        tempVec2.y = v1y;
        pWorldWall->append(tempVec2);
        tempVec2.x = v2x;
        tempVec2.y = v2y;
        pWorldWall->append(tempVec2);

        pNewWorld->AddTriangle(pWorldWall);     
    }


    // 墙体三角
    for(TiXmlNode* wallNode = server_map.FirstChild("walltri").ToNode();
        wallNode; wallNode = wallNode->NextSibling("walltri"))
    {
        TiXmlElement* wallElement = wallNode->FirstChild("tri")->ToElement();

        for(; wallElement; wallElement = wallElement->NextSiblingElement())
        {
            CoPolygon* pWall = CnNew CoPolygon;

            CoVec2 tempVec2;

            real v0x, v0y, v1x, v1y, v2x, v2y;
            wallElement->QueryFloatAttribute("v0x", &v0x);
            wallElement->QueryFloatAttribute("v0y", &v0y);
            wallElement->QueryFloatAttribute("v1x", &v1x);
            wallElement->QueryFloatAttribute("v1y", &v1y);
            wallElement->QueryFloatAttribute("v2x", &v2x);
            wallElement->QueryFloatAttribute("v2y", &v2y);

            tempVec2.x = v0x;
            tempVec2.y = v0y;
            pWall->append(tempVec2);
            tempVec2.x = v1x;
            tempVec2.y = v1y;
            pWall->append(tempVec2);
            tempVec2.x = v2x;
            tempVec2.y = v2y;
            pWall->append(tempVec2);

            pNewWorld->AddTriangle(pWall); 
        }
    }
    // 洞三角
    for(TiXmlNode* holeNode = server_map.FirstChild("holetri").ToNode();
        holeNode; holeNode = holeNode->NextSibling("holetri"))
    {
        TiXmlElement* holeElement = holeNode->FirstChild("tri")->ToElement();

        for(; holeElement; holeElement = holeElement->NextSiblingElement())
        {
            CoPolygon* pHole = CnNew CoPolygon;
            CoVec2 tempVec2;

            real v0x, v0y, v1x, v1y, v2x, v2y;
            holeElement->QueryFloatAttribute("v0x", &v0x);
            holeElement->QueryFloatAttribute("v0y", &v0y);
            holeElement->QueryFloatAttribute("v1x", &v1x);
            holeElement->QueryFloatAttribute("v1y", &v1y);
            holeElement->QueryFloatAttribute("v2x", &v2x);
            holeElement->QueryFloatAttribute("v2y", &v2y);

            tempVec2.x = v0x;
            tempVec2.y = v0y;
            pHole->append(tempVec2);
            tempVec2.x = v1x;
            tempVec2.y = v1y;
            pHole->append(tempVec2);
            tempVec2.x = v2x;
            tempVec2.y = v2y;
            pHole->append(tempVec2);

            pNewWorld->AddTriangle(pHole); 
        }
    }

    // 导航网格
    for(TiXmlNode* holeNode = server_map.FirstChild("navigation").ToNode();
        holeNode; holeNode = holeNode->NextSibling("navigation"))
    {
        TiXmlElement* holeElement = holeNode->FirstChild("tri")->ToElement();

        for(; holeElement; holeElement = holeElement->NextSiblingElement())
        {
            CoPolygon* pNav = CnNew CoPolygon;
            CoVec2 tempVec2;

            real v0x, v0y, v1x, v1y, v2x, v2y;
            holeElement->QueryFloatAttribute("v0x", &v0x);
            holeElement->QueryFloatAttribute("v0y", &v0y);
            holeElement->QueryFloatAttribute("v1x", &v1x);
            holeElement->QueryFloatAttribute("v1y", &v1y);
            holeElement->QueryFloatAttribute("v2x", &v2x);
            holeElement->QueryFloatAttribute("v2y", &v2y);

            tempVec2.x = v0x;
            tempVec2.y = v0y;
            pNav->append(tempVec2);
            tempVec2.x = v1x;
            tempVec2.y = v1y;
            pNav->append(tempVec2);
            tempVec2.x = v2x;
            tempVec2.y = v2y;
            pNav->append(tempVec2);

            pNewWorld->AddNavigation(pNav); 
        }
    }

    pNewWorld->MakeBlockCell();

    return pNewWorld;
}
