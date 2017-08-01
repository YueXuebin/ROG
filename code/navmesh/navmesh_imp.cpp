#include "navmesh_pch.h"
#include "navmesh_imp.h"
#include "triangle_node.h"
#include "newfunnel.h"
#include "edge.h"
#include "pool_manager.h"
#include "CoWorld.h"

bool SortOpenList(TriangleNode* node1, TriangleNode* node2) 
{
    if(!node1 || !node2)
        return false;
    return node1->GetF_Value() < node2->GetF_Value();
}

NavMesh* CreateNavMesh(const std::vector<CoPolygon*>& triangles, CoWorld* pCoWorld)
{
    return CnNew NavMeshImp(triangles, pCoWorld);
}

void DestroyNavMesh(NavMesh* pNavMesh)
{
    CnDelete pNavMesh;
}

NavMeshImp::NavMeshImp(const std::vector<CoPolygon*>& triangles, CoWorld* pCoWorld):NavMesh(triangles),pWorld(pCoWorld)
{
    Init(triangles);
}

NavMeshImp::~NavMeshImp()
{
    Destory();
}

void NavMeshImp::Init(const std::vector<CoPolygon*>& triangles)
{
    std::vector<CoPolygon*>::const_iterator polygoniter;
    for (polygoniter = triangles.begin(); polygoniter != triangles.end(); polygoniter++)
    {
        TriangleNode* curnode = g_PoolManager.CreateTriangleNode(*polygoniter);
        if(!curnode) continue;

        curnode->SetPolygonOfTriangleNode(curnode);
        m_AStartPolygonList.push_back(curnode);
    }

    std::vector<TriangleNode*>::iterator neigerber;
    for(neigerber = m_AStartPolygonList.begin(); neigerber != m_AStartPolygonList.end(); neigerber++)
    {
        GetNeighborsTriangles((*neigerber));
    }
}

void NavMeshImp::Destory()
{
    std::vector<TriangleNode*>::iterator iter;
    for(iter = m_AStartPolygonList.begin(); iter != m_AStartPolygonList.end(); iter++)
    {
        if((*iter) != NULL)
        {
            g_PoolManager.DestoryTriangleNode(*iter);
        }
    }
}
void NavMeshImp::GetNeighborsTriangles(TriangleNode* polygon)
{
    if(!polygon)  return;

    std::vector<TriangleNode*>::iterator neigerber;
    for(neigerber = m_AStartPolygonList.begin(); neigerber != m_AStartPolygonList.end(); neigerber++)
    {
        if(polygon->m_neighbors.size() >= 3) 
            break;
        if(IsEqualsTriangle(polygon->GetTriangle(), (*neigerber)->GetTriangle())) 
            continue;
        if(IsCommonEdge(polygon->GetTriangle(), (*neigerber)->GetTriangle()))
        {
            polygon->m_neighbors.push_back(*neigerber);
        }
    }
    while(polygon->m_neighbors.size() < 3)
    {
        polygon->m_neighbors.push_back(NULL);
    }
}

//判断两个三角形是否有公共边
bool NavMeshImp::IsCommonEdge(CoPolygon* triangle1 , CoPolygon*triangle2)
{
    int32 num = 0; //记录相同点的个数
    std::vector<CoVec2>::const_iterator iter;
    for (iter = triangle1->Points().begin(); iter != triangle1->Points().end(); iter++)
    {
        if(IsEqual(*iter, triangle2->Points()[0]) || IsEqual(*iter, triangle2->Points()[1]) || IsEqual(*iter, triangle2->Points()[2]))
            num++;
    }

    if(num >= 2)
        return true;

    return false;
}

//得到二个三角形的公共边
Edge* NavMeshImp::GetCommonEdge(CoPolygon* triangle1 , CoPolygon*triangle2)
{
    std::vector<CoVec2> CommIndex;
    int32 num = 0;
    std::vector<CoVec2>::const_iterator iter;
    for (iter = triangle1->Points().begin(); iter != triangle1->Points().end(); iter++)
    {
        if(IsEqual(*iter, triangle2->Points()[0]) || IsEqual(*iter, triangle2->Points()[1]) || IsEqual(*iter, triangle2->Points()[2]))
        {
            CommIndex.push_back(*iter);
            num++;
        }
    }

    if(num != 2) 
    {
        CnDbg("Triangles are not contiguous");
        return NULL;
    }

    return g_PoolManager.CreateEdge(CommIndex[0], CommIndex[1]);//  CnNew Edge(CommIndex[0], CommIndex[1]);
}

bool NavMeshImp::IsEqualsTriangle(CoPolygon* triangle1 , CoPolygon*triangle2)
{
    int32 num = 0; //记录相同点的个数
    std::vector<CoVec2>::const_iterator iter;
    for (iter = triangle1->Points().begin(); iter != triangle1->Points().end(); iter++)
    {
        if(IsEqual(*iter, triangle2->Points()[0]) || IsEqual(*iter, triangle2->Points()[1]) || IsEqual(*iter, triangle2->Points()[2]))
            num++;
    }

    if(num >= 3)
        return true;

    return false;
}


//重心法
//所以对于平面内任意一点，都可以由如下方程来表示
//P = A +  u * (C – A) + v * (B - A) // 方程1
//如果系数u或v为负值，那么相当于朝相反的方向移动，即BA或CA方向。那么如果想让P位于三角形ABC内部，u和v必须满足什么条件呢？有如下三个条件
//u >= 0
//v >= 0
//u + v <= 1
// Determine whether point P in triangle ABC
bool NavMeshImp::JudgePointInTriangle(CoPolygon* triangle , const CoVec2& pointP)
{
    if(triangle->Points().size()!= 3)
        return false;

    CoVec2 pointA = triangle->Points()[0];
    CoVec2 pointB= triangle->Points()[1];
    CoVec2 pointC =triangle->Points()[2];

    CoVec2 v0 = pointC - pointA; 
    CoVec2 v1 = pointB - pointA;
    CoVec2 v2 = pointP - pointA; 

    real  dot00 = Vec2Dot(v0, v0);
    real  dot01 = Vec2Dot(v0, v1);
    real  dot02 = Vec2Dot(v0, v2);
    real  dot11 = Vec2Dot(v1, v1);
    real  dot12 = Vec2Dot(v1, v2);

    real inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

    real u = (dot11* dot02 - dot01 * dot12) * inverDeno; // if u out of range, return directly
    if(u <0 || u >1)
    {
        return false;
    }

    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno ;
    if (v < 0 || v > 1) // if v out of range, return directly
    {
        return false ;
    }

    return u+v <= 1;
}

//从点里面找出属的三角形
TriangleNode* NavMeshImp::GetTriangeFromPoint(const CoVec2& start)
{
    // 1 得到cell
    // 2 得到cell里面的poly;
    // 3 通过poly查找triangelNode
    CoWorld::Cell* cell = pWorld->GetCellGridByCoord(start.x, start.y);
    if(!cell) 
        return NULL;

    std::vector<CoPolygon*> polygonList =  cell->GetNaviTriangleList();
    std::vector<CoPolygon*>::iterator iter;
    for(iter = polygonList.begin(); iter != polygonList.end(); iter++)
    {
        if(JudgePointInTriangle((*iter), start))
        {
            TriangleNode* curnode = (TriangleNode*)(*iter)->m_UserObject;
            if(curnode) 
                return  curnode;
            else
                return NULL;
        }
    }

    return NULL;
}

bool NavMeshImp::bFindOpenList(TriangleNode* node)
{
    std::vector<TriangleNode*>::iterator iter;
    for (iter = m_OpenList.begin(); iter != m_OpenList.end(); iter++)
    { 
        if(node == (*iter))
            return true;
    }

    return false;
}

void NavMeshImp::AddCloseList(TriangleNode* curNode)
{
    if(!curNode->GetCloseState())
    {
        curNode->SetCloseState(true);        
        m_CloseList.push_back(curNode);
    }
}

bool  NavMeshImp::IsFindCloseList(TriangleNode* triangelNode)
{
    std::vector<TriangleNode*>::iterator iter;
    for(iter = m_CloseList.begin(); iter!= m_CloseList.end(); iter++)
    {
        if(IsEqualsTriangle((*iter)->GetTriangle(), triangelNode->GetTriangle()))
            return true;
    }


    return false;
}
//添加到开器表
void NavMeshImp::AddOpenList(TriangleNode* node)
{
    if(!node) return;
    m_OpenList.push_back(node);
}
//得到当前三角和邻居三角形之间的距离
real NavMeshImp::GetDistance(TriangleNode* triangle, TriangleNode* nexttriangle)
{
    if(!triangle) return 0;
    if(!nexttriangle) return 0;

    real distance = (Crown::Distance((real)triangle->Get_X(), (real)triangle->Get_Y(),(real)nexttriangle->Get_X(),(real)nexttriangle->Get_Y()));

    return distance;
}

void NavMeshImp::SortRemoveTriangle()
{
    int32 num = 0;
    std::vector<TriangleNode*>::iterator iter;
    for(iter = m_OpenList.begin(); iter != m_OpenList.end();)
    {
        if(num == 0)
        {
            iter = m_OpenList.erase(iter);
        }
        else
        {
            iter++;
        }
        num++;
    }
}

void NavMeshImp::UpdateOpenList(TriangleNode* curNode)
{
    if(!curNode) return;

    std::vector<TriangleNode*>::iterator iterlist;
    for(iterlist = m_OpenList.begin(); iterlist != m_OpenList.end(); iterlist++)
    {
        if((*iterlist) == curNode)
        {
            (*iterlist)->Set_G(curNode->Get_G());
            (*iterlist)->Set_H(curNode->Get_H());
            break;
        }
    }
}
bool NavMeshImp::FindPath(const CoVec2& start, const CoVec2& end, std::vector<CoVec2>& pPath)
{
    pPath.clear();

    CoVec2 startpoint = start;
    CoVec2 endpoint = end;
    std::vector<TriangleNode*> triangelist;
    if(FindTriangleList(startpoint, endpoint, triangelist))
    {
        ChannelToPortals(startpoint, endpoint, triangelist, pPath);
    }

    if(pPath.size() >=2)
        return true;

    pPath.push_back(startpoint);
    pPath.push_back(endpoint);

    return true;
}

//利用拐点算法，遍历三角形列表，求出最近路径
void NavMeshImp::ChannelToPortals(CoVec2& startpoint, CoVec2& endpoint, std::vector<TriangleNode*> triangelist, std::vector<CoVec2>& pPath)
{
    real fatScale = 2.5;
    real fatRadius = 50;

    fatRadius *= fatScale;

    NewFunnel* portals = g_PoolManager.CreateFunnel();
    portals->PushPortal(&startpoint);

    CoVec2 tmpPoint;
    CoVec2 tmpPoint1;
    CoVec2 tmpPoint2;

    bool isFatToBlock = false;

    if(triangelist.size()>= 2)
    {
        TriangleNode* first = triangelist[0];
        TriangleNode* sceond = triangelist[1];

        CoVec2 starVertex; //开始点
        starVertex = GetNotCommonVerTex(first, sceond);

        //寻找开始点到第一个三角形的拐点
        CoVec2 vertexCW0 = starVertex;
        CoVec2 vertexCCW0 = starVertex;

        for(uint32 n = 0; n < triangelist.size() - 1; n++)
        {
            TriangleNode* curTriangle = triangelist[n + 0];
            TriangleNode* nextTriangle = triangelist[n + 1];

            if(curTriangle && nextTriangle)
            {
                Edge* curgEdge = GetCommonEdge(curTriangle->GetTriangle(), nextTriangle->GetTriangle());

                if(!curgEdge) continue;

                CoVec2 vertexCW1 = curTriangle->pointCW(vertexCW0);
                CoVec2 vertexCCW1  = curTriangle->pointCCW(vertexCCW0);

                if(!curgEdge->HavePoint(vertexCW0))
                    vertexCW0 = vertexCW1;

                if(!curgEdge->HavePoint(vertexCCW0))
                    vertexCCW0 = vertexCCW1;

                g_PoolManager.DestoryEdge(curgEdge);        //CnDelete curgEdge;

                if( ((vertexCW0.x - vertexCCW0.x)* (vertexCW0.x - vertexCCW0.x)+ (vertexCW0.y - vertexCCW0.y)*(vertexCW0.y - vertexCCW0.y)) <(fatRadius*fatRadius*4))
                {
                    isFatToBlock = true;
                    break;
                }
                else
                {
                    tmpPoint.Set(vertexCCW0.x, vertexCCW0.y);
                    tmpPoint = tmpPoint - vertexCW0;

                    tmpPoint.Normalize();

                    tmpPoint = tmpPoint* fatRadius;

                    tmpPoint1.Set(vertexCW0.x, vertexCW0.y);
                    tmpPoint1 = tmpPoint1 + tmpPoint;


                    tmpPoint.neg(); ///// Negate this point.
                    tmpPoint2.Set(vertexCCW0.x, vertexCCW0.y);
                    tmpPoint2 = tmpPoint2 + tmpPoint;
                }

                portals->PushPortal(&tmpPoint1, &tmpPoint2);
            }
        }// end for
    }// end if 

    if(!isFatToBlock)
        portals->PushPortal(&endpoint);

    portals->GetPathPoint(pPath);

    //CnDelete portals;
    g_PoolManager.DestoryFunnel(portals);
}

//两个三角形是否有公共顶点的序列号
int32 NavMeshImp::GetNotCommonVerTexNum(TriangleNode* triangle1, TriangleNode* triangle2)
{
    if(!triangle1) return 0;
    if(!triangle2) return 0;

    int32 num = 0;
    bool b_exe = false;
    if(!triangle2->ContaiansPoint(triangle1->GetTriangle()->Points()[0])) 
    {
        num = 0; 
        b_exe = true;
    }

    if(!triangle2->ContaiansPoint(triangle1->GetTriangle()->Points()[1])) 
    {
        num = 1; 
        b_exe = true;
    }

    if(!triangle2->ContaiansPoint(triangle1->GetTriangle()->Points()[2])) 
    {
        num = 2; 
        b_exe = true;
    }

    if(!b_exe)
        CnDbg("no commeon vertex in triangle");

    return num;
}
//查找两个三角形公共顶点
const CoVec2& NavMeshImp::GetNotCommonVerTex(TriangleNode* triangle1, TriangleNode* triangle2)
{
    int32 num = GetNotCommonVerTexNum(triangle1, triangle2);
    return triangle1->GetTriangle()->Points()[num];
}

// A*查找最近的三角形带
bool NavMeshImp::FindTriangleList(const CoVec2& start, const CoVec2& end, std::vector<TriangleNode*>& trilist)
{
    real fatScale = 2.5;
    real fatRadius = 50;

    fatRadius *= fatScale;

    std::vector<TriangleNode*> ::iterator itertest;
    for(itertest = m_AStartPolygonList.begin(); itertest != m_AStartPolygonList.end(); itertest++)
    {   
        (*itertest)->Reset();
    }

    std::vector<TriangleNode*> triangelist;

    TriangleNode* starNode = GetTriangeFromPoint(start);           //1: 求出点start所在的三角形A
    TriangleNode* endNode = GetTriangeFromPoint(end);              //2: 求出点end所在的三角形 B

    if(!endNode || !starNode)
        return false;

    // clear
    m_OpenList.clear();
    m_CloseList.clear();

    //3: 求出三角形A到B之间最短的距离 三角形列表ListC
    m_OpenList.push_back(starNode);

    TriangleNode* curNode = starNode;

    while((curNode != endNode) && (m_OpenList.size() != 0))
    {
        //排序中出最小的三角形距离
        std::sort(m_OpenList.begin() , m_OpenList.end(), SortOpenList);
        if(m_OpenList.size() > 0)
        {
            curNode = m_OpenList[0];
            m_OpenList.erase(m_OpenList.begin());
        }

        AddCloseList(curNode);
        if(IsEqualsTriangle(curNode->GetTriangle() , endNode->GetTriangle()))
            break;

        std::vector<TriangleNode*>::iterator iter_neighbor;
        for (iter_neighbor = curNode->m_neighbors.begin(); iter_neighbor!= curNode->m_neighbors.end(); iter_neighbor++)
        {
            if(!(*iter_neighbor)) continue;

            if(IsFindCloseList(*iter_neighbor)) continue;

            int32 cur_G = curNode->Get_G() + (int32)GetDistance(curNode, *iter_neighbor);
            //公共边距离问题
            //没有在开启类表里面
            if(!bFindOpenList(*iter_neighbor))
            {
                AddOpenList(*iter_neighbor);
                (*iter_neighbor)->Set_G(cur_G);

                int32 value_h = (int32)GetDistance((*iter_neighbor), endNode);

                (*iter_neighbor)->Set_H(value_h);
                (*iter_neighbor)->m_parentNode = curNode;
                UpdateOpenList(*iter_neighbor);

            }
            else if(cur_G < (*iter_neighbor)->Get_G()) //在开启表里面
            {
                (*iter_neighbor)->Set_G(cur_G);
                (*iter_neighbor)->m_parentNode = curNode;
                UpdateOpenList( (*iter_neighbor));
            }
        }

    }

    while (curNode->GetTriangle()!= starNode->GetTriangle()) 
    {
        triangelist.push_back(curNode);
        curNode = curNode->m_parentNode;
    }

    triangelist.push_back(starNode);

    std::vector<TriangleNode*>::reverse_iterator curlist;
    for(curlist = triangelist.rbegin(); curlist != triangelist.rend(); curlist++)
    {
        trilist.push_back(*curlist);
    }

    return true;
}