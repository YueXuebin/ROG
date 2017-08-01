/*
 *	行为控制器工厂
 */
#ifndef action_factory_h__
#define action_factory_h__


class BattleUnit;
class ActionController;

ActionController*   CreateAction(BattleUnit* pUnit);

#endif // action_factory_h__
