/*
 *	物理空间的对象基类
 */
#ifndef CoObject_h__
#define CoObject_h__

#include "CoMath.h"
#include "CoVec2DInt.h"

class CoWorld;
class CoDynamicObject;
struct CoSegment;

class CoObjectOwner
{
	CoDynamicObject* m_pCoDynObject;

public:
	CoObjectOwner();

	virtual ~CoObjectOwner() {}

	CoDynamicObject* CreateCoObject(real x, real y, real radius);


	CoDynamicObject* GetCoObject() { return m_pCoDynObject; }
	const CoDynamicObject* GetCoObject() const { return m_pCoDynObject; }

	virtual bool MayCollidedWith(const CoObjectOwner&) const { return true; }
	virtual bool MayCollidedWith(const CoSegment&) const { return true; }

    virtual void OnNotify(uint32 /*msgId*/, void* /*pData*/) {}
    virtual void OnObjectDisappeared(CoObjectOwner* obj) {}
    virtual void OnObjectAppeared(CoObjectOwner* obj) {}
};

class CoObject
{
protected:
	CoObject(CoObjectOwner* pOwner);
	virtual ~CoObject();

	virtual void UpdatePos() = 0;

public:
	int	GetCellPosX() const { return m_CellPos.X; }
	int	GetCellPosY() const { return m_CellPos.Y; }

	bool	CollisionTestWithCell();

    uint32  GetLayer() {return m_layer;}
    void    SetLayer(uint32 layer) {m_layer = layer;}

	CoObjectOwner* GetOwner() {return m_pOwner;}
	const CoObjectOwner* GetOwner() const {return m_pOwner;}

    virtual void    onEnterWorld(CoWorld* pWorld);
    virtual void    onLeaveWorld(CoWorld* pWorld);

protected:
	CoWorld*	m_pOwnerWorld;
	CoObjectOwner*	m_pOwner;
	CoVec2DInt	m_CellPos;      // 该Object所占cell位置
    bool m_CellPosInited;       // true表示m_CellPos被赋过合法的值
    uint32      m_layer;
};

#endif // CoObject_h__
