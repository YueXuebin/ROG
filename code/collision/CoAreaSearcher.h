/*
 *	范围搜索器
 */
#ifndef COAREASEARCHER_H_
#define COAREASEARCHER_H_

class CoWorld;
class CoDynamicObject;


class CoAreaSearcher
{
public:
	CoAreaSearcher(CoWorld* pWorld);
	virtual ~CoAreaSearcher();

	//void SearchInRoundness(
	//	std::vector<CoDynamicObject*>& container, real centreX, real centreY, real radius) const;


	//void SearchInAngle(
	//	std::vector<CoDynamicObject*>& container, real centreX, real centreY,
	//	real angleCenter, real angleSeperation, real range);
private:
	CoWorld* m_pWorld;
	/*
	 * 遍历可能涉及到的方格中，搜索中心在圆内的所有对象。
	 * 通过计算对象中心和圆心的距离来判断是否在圆内，所以称为Slow版本。
	 */
	void SearchInRoundnessSlow(
		std::vector<CoDynamicObject*>& container, real centreX, real centreY, real radius) const;

};

#endif /* COAREASEARCHER_H_ */
