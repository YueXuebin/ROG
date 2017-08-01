#ifndef FORMULA_OBJECT_H
#define FORMULA_OBJECT_H

class FormulaObject
{
public:
    FormulaObject();
    virtual ~FormulaObject();

    virtual int32 GetHP() = 0;
    virtual int32 GetMP() = 0;
    virtual int32 GetMaxHp() = 0;
    virtual int32 GetMaxMp() = 0;
    virtual int32 GetArmor() = 0;
    virtual int32 GetDodge() = 0;

    int32   GetFunctionValue(std::string name);                 // 根据名称获得函数值

protected:
    typedef int32 (FormulaObject::*FuncPoint)();                // 定义获得属性函数指针
    void    InitFunctionMap();                                  // 初始化函数映射表
    void    RegisterFunction(std::string name, FuncPoint func); // 注册属性函数

    std::map<std::string, FuncPoint> m_funcMap;                 // 属性函数
};

#endif
