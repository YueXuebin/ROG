#ifndef task_data_factory_h__
#define task_data_factory_h__


class RoleTask;
class Player;

// 创建任务对象
RoleTask* CreateTaskDataObject(const TASK_TEMPLATE* task_template, Player* player);
void DestoryTaskDataObject(RoleTask* pTask);

uint32  GetTaskObjectNum();



#endif // task_data_factory_h__
