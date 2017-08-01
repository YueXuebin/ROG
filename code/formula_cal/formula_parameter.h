#ifndef FORMULA_PARAMETER_H
#define FORMULA_PARAMETER_H

class FormulaTemplate;
class FormulaObject;

class CalParameter
{
public:
    std::string key;
    real      val;
};

class FormulaParameter
{
public:
    bool Init(const FormulaTemplate* formulaTmpl, FormulaObject* aObject=NULL, FormulaObject* bObject=NULL);

    void Clear();

    bool GetParam(const std::string& key, real* val);

    // 添加参数
    void AddParam(const std::string& key, int8 val);
    void AddParam(const std::string& key, uint8 val);
    void AddParam(const std::string& key, int16 val);
    void AddParam(const std::string& key, uint16 val);
    void AddParam(const std::string& key, int32 val);
    void AddParam(const std::string& key, uint32 val);
    //void AddParam(const std::string& key, int64 val);
    //void AddParam(const std::string& key, uint64 val);
    void AddParam(const std::string& key, real val);
    //void AddParam(const std::string& key, const std::string& val);

    typedef std::map<std::string, CalParameter> CalParameterList;
    CalParameterList   m_params;     // <key, obj>
};

#endif
