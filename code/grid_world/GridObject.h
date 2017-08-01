#ifndef IGridObject_h__
#define IGridObject_h__

class GridWorld;
class GridCell;

class GirdObjectOwner
{


};


/*
 *	场景对象接口类
 */
class GridObject
{
    friend class GridWorld;
    friend class GridCell;

public:
    GridObject(GirdObjectOwner* owner);
    virtual ~GridObject();

    int32   GetGridPosX() {return m_GridPosX;}
    int32   GetGridPosY() {return m_GridPosY;}

    void    SetPosition(real x, real y);

private:
    void    OnEnterGridWorld(GridWorld* world);
    void    OnLeaveGridWorld(GridWorld* world);

private:
    int32   m_GridPosX;
    int32   m_GridPosY;

    real    m_x;
    real    m_y;

    GridWorld*  m_pGridWorld;
    GridObject* m_Next;

    GirdObjectOwner* m_Owner;
};



#endif // IGridObject_h__
