#ifndef OBJECTWITHID_H
#define OBJECTWITHID_H


class ObjectWithId
{
public:
    ObjectWithId(uint32 type);
    virtual ~ObjectWithId();
    uint32 GetId() const { return m_Id; }

    uint32 GetObjectType() const { return m_Type; }

private:
    uint32 m_Id;
    uint32 m_Type;
};

#endif // OBJECTWITHID_H
