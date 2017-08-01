#include "navmesh_pch.h"
#include "portal.h"

Portal::Portal(const CoVec2& left, const CoVec2& right)
{
    m_left = left;
    m_right = right;
}

Portal::~Portal()
{
}
