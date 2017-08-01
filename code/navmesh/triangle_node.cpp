#include "navmesh_pch.h"
#include "triangle_node.h"

TriangleNode::TriangleNode(CoPolygon* polygon)
{
    InitNode(polygon);
}


void TriangleNode::InitNode(CoPolygon* polygon)
{
    if(!polygon) return;
    m_g = 0;
    m_h = 0;
    m_x = ((polygon->Points()[0].x + polygon->Points()[1].x +polygon->Points()[2].x) / 3);
    m_y = ((polygon->Points()[0].y + polygon->Points()[1].y + polygon->Points()[2].y) / 3);
    m_triangles = (polygon);
    b_close = false;
    m_parentNode = NULL;
}

CoPolygon* TriangleNode::GetTriangle()
{
    if(m_triangles)
        return m_triangles;

    return NULL;
}

void TriangleNode::SetPolygonOfTriangleNode(TriangleNode* node)
{
    if(!node) return;
    m_triangles->m_UserObject = (TriangleNode*)node;
}

//得到三角形的左开口
const CoVec2& TriangleNode::pointCW(const CoVec2& curpoint)
{
    const int32 CCW_OFFSET = -1;
    int32 index = 0;
    index = getPointIndexOffset(curpoint, CCW_OFFSET);
    return (m_triangles->Points()[index]);
}
//得到三角形的右开口
const CoVec2& TriangleNode::pointCCW(const CoVec2& curpoint)
{
    const int32 CW_OFFSET = +1;

    int32 index = 0;
    index = getPointIndexOffset(curpoint, CW_OFFSET);
    return (m_triangles->Points()[index]);
}

int32 TriangleNode::getPointIndexOffset(const CoVec2& p, int32 offset)
{
    int32 no = offset;
    for (int32 n = 0; n < 3; n++, no++)
    {
        while(no < 0)
        {
            no +=3;
        }

        while(no >2)
        {
            no -=3;
        }

        if(IsEqual(p, m_triangles->Points()[n])) return no;
    }

    CnDbg("Point not in triangle");
    return 0;
}

void TriangleNode::NeighborReset()
{
    std::vector<TriangleNode*>::iterator iter;
    for(iter = m_neighbors.begin(); iter != m_neighbors.end(); iter++)
    {
        if((*iter))
        {
            (*iter)->m_parentNode =NULL;
            (*iter)->Set_G(0);
            (*iter)->Set_H(0);
            (*iter)->SetCloseState(false);
        }
    }
}

//重置 g,h值
void TriangleNode::Reset()
{
    m_g = 0;
    m_h = 0;
    b_close = false;
    m_parentNode = NULL;
    NeighborReset();
}

//当前三角形是否包含该点
bool TriangleNode::ContaiansPoint(const CoVec2& curpoint)
{
    return ( IsEqual(m_triangles->Points()[0], curpoint) || IsEqual(m_triangles->Points()[1], curpoint)|| IsEqual(m_triangles->Points()[2], curpoint));
}