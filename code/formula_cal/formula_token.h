#ifndef FORMULA_TOKEN_H
#define FORMULA_TOKEN_H

class FormulaToken
{
public:
    void Clear();

    void SetErrCode(int32 val);     ///< ���ô�����

    void SetPriority(int32 val);    ///< �������ȼ���ֻ�Բ�������Ч��

    void PushChar(int8 val);        ///< ѹ���ַ�

    enum
    {
        kTOKEN_OK               = 0,
        kTOKEN_ERR              = 1,
        // ====================
        kTOKEN_TYPE_ID          = 1,
        kTOKEN_TYPE_DIGIT       = 2,
        kTOKEN_TYPE_OPERATOR    = 3
    };

    int32       m_errcode;          ///< ������
    int32       m_priority;         ///< ���ȼ���ֻ�Բ�������Ч��
    int32       m_type;             ///< ����
    real        m_digit;            ///< ����
    bool        m_positive;         ///< �Ƿ�Ϊ������ֻ��ID��Ч��
    std::string m_token;            ///< ��
};

#endif
