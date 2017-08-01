#include "GridWorldPCH.h"
#include "GridWorld.h"
#include "GridObject.h"
#include "GridCell.h"

Crown::CSDObjectPool<GridCell> g_CellPool(100, 100);

enum{
    SEARCH_UP = 1,
    SEARCH_DOWN,
    SEARCH_LEFT,
    SEARCH_RIGHT
};



GridWorld::GridWorld(uint32 cellLength, int32 minX, int32 maxX, int32 minY, int32 maxY) :
m_CellLength(cellLength),
m_ObjectNum(0)
{
    m_Root = CreateGridCell(-SDU_INT32_MAX, -SDU_INT32_MAX);
}

GridWorld::~GridWorld()
{
    // 场景中的对象必须都移除
    CnAssert(m_ObjectNum == 0);
    
    // 边界链接点也全部删除
    CnAssert(m_Root->up == NULL);
    CnAssert(m_Root->down == NULL);
    CnAssert(m_Root->left == NULL);
    CnAssert(m_Root->right == NULL);

    // 删除横向边界链接点
    GridCell* here = m_Root->right;
    while(here)
    {
        GridCell* delOne = here;
        CnAssert(!delOne->up);
        CnAssert(!delOne->down);
        CnAssert(!delOne->HasObject());
        here = here->right;
        DestoryGridCell(delOne);
    }

    // 删除纵向边界链接点
    here = m_Root->down;
    while(here)
    {
        GridCell* delOne = here;
        CnAssert(!delOne->left);
        CnAssert(!delOne->right);
        CnAssert(!delOne->HasObject());
        here = here->down;
        DestoryGridCell(delOne);
    }

    DestoryGridCell(m_Root);
}

void GridWorld::AddGridObject(GridObject* pGridObject)
{
    CnAssert(pGridObject);
    m_ObjectNum++;
    CnAssert(m_ObjectNum >= 0);

    pGridObject->OnEnterGridWorld(this);

    // 查找对应坐标的链表
    int32 PosX = pGridObject->GetGridPosX();
    int32 PosY = pGridObject->GetGridPosY();

    // 找到或创建对应位置的链接点
    GridCell* posCell = CreateCell(PosX, PosY);

    // 将对象加入到链接点上
    posCell->AddObject(pGridObject);
}

GridCell* GridWorld::CreateCell(int32 x, int32 y)
{
    // 查找对应坐标的链表
    int32 PosX = x;
    int32 PosY = y;

    // 创建对应的X边界链接点
    GridCell* up = NULL;
    GridCell* down = NULL;
    GridCell* left = NULL;
    GridCell* right = NULL;
    GridCell* edgeX = FindPos4Link(PosX, -SDU_INT32_MAX, &up, &down, &left, &right);
    if(!edgeX)
    {
        CnAssert(left);
        CnAssert(!up);
        CnAssert(!down);
        edgeX = CreateGridCell(PosX, -SDU_INT32_MAX);
        if(left->right)
        {
            CnAssert(left->right == right);
            CnAssert(right->left == left);
            left->right = edgeX;
            edgeX->left = left;
            right->left = edgeX;
            edgeX->right = right;
        }
        else
        {
            CnAssert(!right);
            left->right = edgeX;
            edgeX->left = left;
        }
    }
    else
    {
        CnAssert(left);
        CnAssert(up == NULL);
    }

    // 创建对应的Y边界链接点
    up = NULL;
    down = NULL;
    left = NULL;
    right = NULL;
    GridCell* edgeY = FindPos4Link(-SDU_INT32_MAX, PosY, &up, &down, &left, &right);
    if(!edgeY)
    {
        CnAssert(up);
        CnAssert(!left);
        CnAssert(!right);
        edgeY = CreateGridCell(-SDU_INT32_MAX, PosY);
        if(up->down)
        {
            CnAssert(up->down == down);
            CnAssert(down->up == up);
            up->down= edgeY;
            edgeY->up = up;
            down->up = edgeY;
            edgeY->down = down;
        }
        else
        {
            CnAssert(!down);
            up->down = edgeY;
            edgeY->up = up;
        }
    }

    // 找到或创建对应位置的链接点
    up = NULL;
    down = NULL;
    left = NULL;
    right = NULL;
    GridCell* posCell = FindPos4Link(x, y, &up, &down, &left, &right);
    CnAssert(up);
    CnAssert(left);
    if(!posCell)
    {
        posCell = CreateGridCell(x, y);

        // 插入到四向链表中
        up->down = posCell;
        posCell->up = up;
        if(down)
        {
            down->up = posCell;
            posCell->down = down;
        }
        left->right = posCell;
        posCell->left = left;
        if(right)
        {
            right->left = posCell;
            posCell->right = right;
        }
    }
    return posCell;
}

void GridWorld::RemoveGridObject(GridObject* pGridObject)
{
    CnAssert(pGridObject);
    m_ObjectNum--;
    CnAssert(m_ObjectNum >= 0);

    CnAssert(pGridObject->m_pGridWorld == this);

    // 找到对应的cell
    GridCell* up = NULL;
    GridCell* down = NULL;
    GridCell* left = NULL;
    GridCell* right = NULL;
    GridCell* posCell = FindPos4Link(pGridObject->GetGridPosX(), pGridObject->GetGridPosY(), &up, &down, &left, &right);
    CnAssert(posCell);
    CnAssert(up);
    CnAssert(left);

    // 将对象从cell中去除
    posCell->RemoveObject(pGridObject);

    CheckDelete(posCell, up, down, left, right);

    pGridObject->OnLeaveGridWorld(this);
}

void GridWorld::CheckDelete(GridCell* cell, GridCell* up, GridCell* down, GridCell* left, GridCell* right)
{
    if(cell->HasObject())
        return;
    
    // 从四向链表中删除
    cell->up = NULL;
    cell->down = NULL;
    cell->left = NULL;
    cell->right = NULL;

    up->down = down;
    if(down)
        down->up = up;
    left->right = right;
    if(right)
        right->left = left;

    DestoryGridCell(cell);

    // 删除X边界链接点
    if(up->down == NULL && up->up == NULL)
    {
        CnAssert(up->GetY() == -SDU_INT32_MAX);
        CnAssert(up->left);

        GridCell* delEdgeX = up;
        delEdgeX->left->right = delEdgeX->right;
        if(delEdgeX->right)
            delEdgeX->right->left = delEdgeX->left;

        DestoryGridCell(delEdgeX);
    }
    // 删除Y边界链接点
    if(left->right == NULL && left->left == NULL)
    {
        CnAssert(left->GetX() == -SDU_INT32_MAX);
        CnAssert(left->up);
        
        GridCell* delEdgeY = left;
        delEdgeY->up->down = delEdgeY->down;
        if(delEdgeY->down)
            delEdgeY->down->up = delEdgeY->up;

        DestoryGridCell(delEdgeY);
    }
}

GridCell* GridWorld::FindPos4Link(int32 x, int32 y, GridCell** up, GridCell** down, GridCell** left, GridCell** right)
{
    if(up) (*up) = NULL;
    if(down) (*down) = NULL;
    if(left) (*left) = NULL;
    if(right) (*right) = NULL;


    GridCell* result1 = NULL;
    // 找上下链接点
    {
        // 先横向搜索
        GridCell* rootX = NULL;
        GridCell* hereX = m_Root;

        do
        {
            if(hereX->GetX() == x)
            {
                rootX = hereX;
                break;
            }
            else if(hereX->GetX() > x)
            {
                rootX = NULL;
                break;
            }
            else if(hereX->GetX() < x)
            {
                // 继续找
                hereX = hereX->right;
            }
            else
                CnAssert(false);
        }
        while(hereX);

        if(rootX)
        {
            // 再纵向搜索
            GridCell* last = NULL;
            GridCell* hereY = rootX;

            do 
            {
                if(hereY->GetY() == y)
                {
                    // 找到
                    result1 = hereY;
                    if(up)
                    {
                        *up = result1->up;
                        CnAssert(last == result1->up);
                    }
                    if(down)
                    {
                        *down = result1->down;
                    }
                    break;
                }
                else if(hereY->GetY() > y)
                {
                    // 过了
                    CnAssert(result1 == NULL);
                    result1 = NULL;
                    if(up)
                    {
                        *up = last;
                    }
                    if(down)
                    {
                        *down = hereY;
                    }
                    break;
                }
                else if(hereY->GetY() < y)
                {
                    // 继续找
                    last = hereY;
                    if(up)
                    {
                        *up = hereY;
                    }
                    hereY = hereY->down;
                }
            }
            while (hereY);
        }
    }


    GridCell* result2 = NULL;
    // 找左右链接点
    {
        // 先纵向搜索
        GridCell* rootY = NULL;
        GridCell* hereY = m_Root;

        do
        {
            if(hereY->GetY() == y)
            {
                rootY = hereY;
                break;
            }
            else if(hereY->GetY() > y)
            {
                rootY = NULL;
                break;
            }
            else if(hereY->GetY() < y)
            {
                // 继续找
                hereY = hereY->down;
            }
            else
                CnAssert(false);
        }
        while(hereY);

        if(rootY)
        {
            // 再横向搜索
            GridCell* last = NULL;
            GridCell* hereX = rootY;

            do 
            {
                if(hereX->GetX() == x)
                {
                    // 找到
                    CnAssert(result1);
                    result2 = hereX;
                    CnAssert(result2 == result1);
                    if(left)
                    {
                        *left = result2->left;
                        CnAssert(last == result2->left);
                    }
                    if(right)
                    {
                        *right = result2->right;
                    }
                    break;
                }
                else if(hereX->GetX() > x)
                {
                    // 过了
                    CnAssert(result2 == NULL);
                    result2 = NULL;
                    if(left)
                    {
                        *left = last;
                    }
                    if(right)
                    {
                        *right = hereX;
                    }
                    break;
                }
                else if(hereX->GetX() < x)
                {
                    // 继续找
                    last = hereX;
                    if(left)
                    {
                        *left = hereX;
                    }
                    hereX = hereX->right;
                }
            }
            while (hereX);
        }
    }
    
    CnAssert(result1 == result2);

    // 结果校验
#ifdef DEBUG
    if(result1)
    {
        if(*up)
        {
            CnAssert(result1->up == *up);
            CnAssert((*up)->down == result1);
        }
        else
        {
            CnAssert(!result1->up);
        }

        if(*down)
        {
            CnAssert(result1->down == *down);
            CnAssert((*down)->up == result1);
        }
        else
        {
            CnAssert(!result1->down);
        }

        if(*left)
        {
            CnAssert(result1->left == *left);
            CnAssert((*left)->right == result1);
        }
        else
        {
            CnAssert(!result1->left);
        }

        if(*right)
        {
            CnAssert(result1->right == *right);
            CnAssert((*right)->left == result1);
        }
        else
        {
            CnAssert(!result1->right);
        }
    }
    else
    {
        if(*up)
        {
            if(*down)
            {
                CnAssert((*up)->down == (*down));
                CnAssert((*down)->up == (*up));
            }
            else
            {
                CnAssert(!(*up)->down);
            }
        }
        else
        {
            CnAssert(!(*down));
        }

        if(*left)
        {
            if(*right)
            {
                CnAssert((*left)->right == (*right));
                CnAssert((*right)->left == (*left));
            }
            else
            {
                CnAssert(!(*left)->right);
            }
        }
        else
        {
            CnAssert(!(*right));
        }

    }

#endif

    return result1;
}

void GridWorld::OnMoveGridObject(GridObject* pGridObject, int32 oldCellX, int32 oldCellY, int32 newCellX, int32 newCellY)
{
    // 找到原格子
    GridCell* up = NULL;
    GridCell* down = NULL;
    GridCell* left = NULL;
    GridCell* right = NULL;
    GridCell* oldCell = FindPos4Link(oldCellX, oldCellY, &up, &down, &left, &right);
    CnAssert(oldCell);

    // 从原格子中移除
    oldCell->RemoveObject(pGridObject);

    CheckDelete(oldCell, up, down, left, right);

    // 找到或创建新格子
    GridCell* newCell = CreateCell(newCellX, newCellY);
    CnAssert(newCell);

    // 加入到新格子中
    newCell->AddObject(pGridObject);
}

GridCell* GridWorld::CreateGridCell(int32 x, int32 y)
{
    GridCell* pNew = g_CellPool.Alloc(x, y);
    return pNew;
}

void GridWorld::DestoryGridCell(GridCell* cell)
{
    g_CellPool.Free(cell);
}

// 获取所有空位置
uint32 GridWorld::FindEmptyPosInRect(int32 cellMinX, int32 cellMinY, int32 cellMaxX, int32 cellMaxY, Crown::SDPoint* result, uint32 resultLen)
{
    uint32 emptyNum = 0;
   
    // 横向搜索
    GridCell* hereX = m_Root;
    
    for(int32 x=cellMinX; x<=cellMaxX; x++)
    {
        while(hereX && hereX->GetX() < x)
            hereX = hereX->right;

        GridCell* hereY = NULL;
        if(hereX && hereX->GetX() == x)
            hereY = hereX;
        else
            hereY = NULL;

        // 纵向搜索
        for(int32 y=cellMinY; y<=cellMaxY; y++)
        {
            while(hereY && hereY->GetY() < y)
                hereY = hereY->down;

            if(hereY && hereY->GetY() == y)
            {   // 非空

            }
            else
            {
                emptyNum++;
                if(result && emptyNum <= resultLen)
                {
                    result[emptyNum-1].x = x;
                    result[emptyNum-1].y = y;
                }
            }
        }
    }

    return emptyNum;

}
// 获取所有非空位置
uint32 GridWorld::FindCellPosInRect(int32 cellMinX, int32 cellMinY, int32 cellMaxX, int32 cellMaxY, Crown::SDPoint* result, uint32 resultLen)
{
    uint32 findNum = 0;
    // 横向搜索
    GridCell* hereX = m_Root->right;
    while(hereX)
    {
        if(hereX->GetX() >= cellMinX && hereX->GetX() <= cellMaxX)
        {   // 纵向搜索
            GridCell* hereY = hereX->down;
            while(hereY)
            {
                if(hereY->GetY() >= cellMinY && hereY->GetY() <= cellMaxY)
                {
                    findNum++;
                    if(result && findNum <= resultLen)
                    {
                        result[findNum-1].x = hereY->GetX();
                        result[findNum-1].y = hereY->GetY();
                    }
                }
                else if(hereY->GetY() > cellMaxY)
                    break;

                hereY = hereY->down;
            }
        }
        else if(hereX->GetX() > cellMaxX)
            break;

        hereX = hereX->right;
    }
    return findNum;
}

bool GridWorld::HasCellAtPos(int32 cellX, int32 cellY)
{
    return FindCellPosInRect(cellX, cellY, cellX, cellY, NULL, 0) > 0;
}
