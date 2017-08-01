/*
 * checkpoint
 * pin路点,无任何逻辑功能,仅表示位置
 */
#ifndef checkpoint_h__
#define checkpoint_h__

#include "sceneobject.h"

class Checkpoint : public SceneObject
{
	CnDeclareRTTI
public:
	Checkpoint();
	virtual ~Checkpoint();

	virtual void onEnterScene(Scene* pScene);
	virtual void onLeaveScene(Scene* pScene);

	virtual void Update(uint32 curr_time);
    virtual void OnDestory() {}
public:
    uint32 GetCheckpointId() {return m_checkpointId;}
    void SetCheckpointId(uint32 id) {m_checkpointId = id;}

private:
    uint32 m_checkpointId;
};

#endif // checkpoint_h__