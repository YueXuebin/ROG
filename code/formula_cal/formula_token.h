#ifndef FORMULA_TOKEN_H
#define FORMULA_TOKEN_H

class FormulaToken
{
public:
    void Clear();

    void SetErrCode(int32 val);     ///< 设置错误码

    void SetPriority(int32 val);    ///< 设置优先级（只对操作符有效）

    void PushChar(int8 val);        ///< 压入字符

    enum
    {
        kTOKEN_OK               = 0,
        kTOKEN_ERR              = 1,
        // ====================
        kTOKEN_TYPE_ID          = 1,
        kTOKEN_TYPE_DIGIT       = 2,
        kTOKEN_TYPE_OPERATOR    = 3
    };

    int32       m_errcode;          ///< 错误码
    int32       m_priority;         ///< 优先级（只对操作符有效）
    int32       m_type;             ///< 类型
    real        m_digit;            ///< 数字
    bool        m_positive;         ///< 是否为正数（只对ID有效）
    std::string m_token;            ///< 词
};

#endif
