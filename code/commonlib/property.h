#ifndef property_h__
#define property_h__

// 属性 类型表  用于为角色添加属性
class Property
{
public:
    Property(int32 t, int32 v)
    {
        type = t;
        value = v;
    }

    Property()
    {
        Clear();
    }

    void Clear()
    {
        type = 0;
        value = 0;
    }

    Property& operator=(const Property& c)
    {
        type = c.type;
        value = c.value;

        return *this;
    }

    int32 type;         // 属性类型
    int32 value;        // 属性值

};

typedef std::vector<Property> PropertyList;

// 属性集
class PropertySet
{
public:
    PropertySet();
    PropertySet(const PropertyList& list);
    ~PropertySet();

    void Clear();

    void Add(const PropertySet& propSet);
    void Add(const PropertyList& list);
    void Add(const Property& prop);
    void Add(int32 type, int32 value);

    void Set(int32 type, int32 value);

    void Remove(int32 type);
    
    int32 GetValue(int32 type) const;

    bool HasType(int32 type) const;

    PropertySet& operator=(const PropertySet& c);

    PropertyList m_list;
};

bool operator == (const PropertySet& a, const PropertySet& b);

void FuseSameProp(PropertyList& propList);

PropertyList GetPropListByString(const std::string& propStr);  // XML等读出的string转属性列

#endif // property_h__
