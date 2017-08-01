#ifndef ai_condition_finish_line_h__
#define ai_condition_finish_line_h__

#include "ai_condition_base.h"

/*
 *	AI条件：到达路径终点
 */
class AIConditionFinishPath : public AIConditionBase
{
public:
    AIConditionFinishPath();
    virtual ~AIConditionFinishPath();

    virtual bool IsSatisfy();

};

#endif // ai_condition_finish_line_h__
