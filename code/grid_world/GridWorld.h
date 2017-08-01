#ifndef GridWorld_h__
#define GridWorld_h__


class GridCell;
class GridObject;

/*
 *	四向链表实现的网格化场景管理
 *  说明:格子数量太多会导致内存消耗巨大,采用四向链表实现场景管理可以有效避免场景格的内存消耗
 *  *--------------*--------------------*-------------
 *  |              |                    |
 *  |              |                    |
 *  |              |                    |
 *  *--------------+--------------------*
 *  |              |
 *  *--------------*
 *  |
 */
class GridWorld
{
    friend class GridObject;
public:
    GridWorld(uint32 cellLength, int32 minX, int32 maxX, int32 minY, int32 maxY);
    ~GridWorld();

    void AddGridObject(GridObject* pGridObject);
    void RemoveGridObject(GridObject* pGridObject);

    uint32  GetCellLength() {return m_CellLength;}

    GridCell*   GetRoot() {return m_Root;}

    uint32 FindCellPosInRect(int32 cellMinX, int32 cellMinY, int32 cellMaxX, int32 cellMaxY, Crown::SDPoint* result, uint32 resultLen);         // 获取所有非空位置
    uint32 FindEmptyPosInRect(int32 cellMinX, int32 cellMinY, int32 cellMaxX, int32 cellMaxY, Crown::SDPoint* result, uint32 resultLen);        // 获取所有空位置
    bool HasCellAtPos(int32 cellX, int32 cellY);

private:
    GridCell*   CreateGridCell(int32 x, int32 y);
    void        DestoryGridCell(GridCell* cell);

    void        OnMoveGridObject(GridObject* pGridObject, int32 oldCellX, int32 oldCellY, int32 newCellX, int32 newCellY);

    // 查找某格子4方向上相邻的链表对象
    GridCell*   FindPos4Link(int32 x, int32 y, GridCell** up, GridCell** down, GridCell** left, GridCell** right);

    GridCell*   CreateCell(int32 x, int32 y);

    void        CheckDelete(GridCell* cell, GridCell* up, GridCell* down, GridCell* left, GridCell* right);
    

private:
    GridCell*   m_Root;

    int32       m_ObjectNum;            // 加入到场景中的对象数
    uint32      m_CellLength;

    
};


#endif // GridWorld_h__
