#ifndef GridCell_h__
#define GridCell_h__


class GridObject;

/*
 *	四向链表
 */
class GridCell
{
public:
    GridCell(int32 x, int32 y) : up(NULL), down(NULL), left(NULL), right(NULL), m_x(x), m_y(y), m_ObjectLink(NULL) {}
    ~GridCell() {}

    GridCell* up;
    GridCell* down;
    GridCell* left;
    GridCell* right;

    void    AddObject(GridObject* object);
    void    RemoveObject(GridObject* object);

    bool    HasObject() {return m_ObjectLink;}

    int32   GetX() {return m_x;}
    int32   GetY() {return m_y;}

private:
    int32   m_x;
    int32   m_y;

    GridObject* m_ObjectLink;          // 格子上的对象链表
};



#endif // GridCell_h__
