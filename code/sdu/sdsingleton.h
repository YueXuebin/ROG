#pragma once

#ifndef SDSINGLETON_H
#define SDSINGLETON_H

#include "buildconfig.h"
#include "sdtype.h"


namespace   Crown
{

    enum ConstructorStrategy
    {
        WITH_DEFAULT_CONSTRUCTOR,
        WITHOUT_DEFAULT_CONSTRUCTOR
    };

    template <typename T>
    class _Singleton
    {
    private:
        _Singleton(const _Singleton<T>&) {}
        _Singleton& operator =(const _Singleton<T>&) {}

    protected:
        // You SHOULD initialize the instance yourself.
        static T* m_pInstance;

        _Singleton() {}
    };

    template <typename T>
    T* _Singleton<T>::m_pInstance = nullptr;

    template <typename T, enum Crown::ConstructorStrategy>
    class Singleton;

    template <typename T>
    class Singleton<T, WITH_DEFAULT_CONSTRUCTOR> : public _Singleton<T>
    {
        struct Initializer
        {
            Initializer(T** ppInstance)
            {
                if (*ppInstance == nullptr)
                    *ppInstance = CnNew T;
            }
        };

    public:
        static T& GetInstance()
        {
            static Initializer s_Instance(&_Singleton<T>::m_pInstance);
            return *_Singleton<T>::m_pInstance;
        }

        static T* GetInstancePtr()
        {
            static Initializer s_Instance(&_Singleton<T>::m_pInstance);
            return _Singleton<T>::m_pInstance;
        }
    };

    // 请自行初始化 m_pInstance 对象.
    // WARNING: Initialize the instance yourself.
    template <typename T>
    class Singleton<T, WITHOUT_DEFAULT_CONSTRUCTOR> : public _Singleton<T>
    {
    public:
        static T& GetInstance()
        {
            return *_Singleton<T>::m_pInstance;
        }

        static T* GetInstancePtr()
        {
            return _Singleton<T>::m_pInstance;
        }
    };

    // WARNING: The following code IS CRAP. NOT TO USE THEM IN NEW CODE.

    // 新增代码不要使用下面的部分.
    // FIXME: 删除这部分代码的依赖,使用上面定义的模板.

    /**
    * @defgroup groupsingleton 单件类封装
    * @ingroup  Crown
    * @{
    */

    /**
    如何使用
    1. 避免随意使用单件对象，你应当声明构造函数和析构函数为私有
    2. 避免随意使用单例对象，你应当声明拷贝构造函数和operator ＝ 为私有
    3. 添加DECLARE_SINGLETON(cls)至你的类的声明处, cls为你类的名称
    4. 添加IMPLEMENT_SINGLETON(cls)至你的类的定义处

    for example:
    //Sample.h
    class CSample
    {
    CSample();
    ~CSample();
    DECLARE_SINGLETON(CSample)
    }

    //Sample.cpp
    IMPLEMENT_SINGLETON(CSample)
    */

#define DECLARE_SINGLETON(cls)\
private:\
    static cls* m_poInstance;\
public:\
    static bool CreateInstance()\
    {\
    if(nullptr == m_poInstance)\
    m_poInstance = CnNew cls;\
    return m_poInstance != nullptr;\
    }\
    static cls* Instance(){ return m_poInstance; }\
    static void DestroyInstance()\
    {\
    if(m_poInstance != nullptr)\
    {\
    CnDelete m_poInstance;\
    m_poInstance = nullptr;\
    }\
    }

#define IMPLEMENT_SINGLETON(cls) \
    cls* cls::m_poInstance = nullptr;


    //The second way to declare a singleton class
#define  SINGLETON_FRIEND_CLASS_REGISTER(T)  friend  class  CSingleton<T>

    /**
    @brief 单件模板类
    *
    *@remarks
    *1.你的单件类应当以publicly的方式继承自此类并且构造函数，析构函数应当设置为私有的。
    *2.避免随意使用单例对象，你应当声明拷贝构造函数和operator ＝ 为私有
    *3.添加宏 SINGLETON_FRIEND_CLASS_REGISTER(T) 在类的声明处以注册本类为一个friend类
    */
    template<class  T>
    class   CSingleton
    {
    public:

        /**
        * @brief
        * 初始化创建单件类
        * @return false means failed
        */
        static bool CreateInstance()
        {
            if(nullptr == m_poInstance)
            {
                m_poInstance = CnNew T;
            }
            return m_poInstance != nullptr;
        }

        /**
        * @brief
        * 获取单件类指针
        * @return 单件类指针，如果单件类实例初始化失败，返回NULL
        */
        static T* Instance(void)
        {
            return m_poInstance;
        }

        /**
        * @brief
        * 销毁单件类实例
        * @return void
        */
        static void DestroyInstance(void)
        {
            CnDelete  m_poInstance;
            m_poInstance = nullptr;
        }

    protected:
        CSingleton(void) {}
        virtual ~CSingleton(void) {}

    private:
        static T*   m_poInstance;
    };

    template<class T>
    T* CSingleton<T>::m_poInstance = nullptr;



    // 单件模式的另一种实现,用户只需定义 typedef CSDSingleton<Type> TypeSingleton;

    template <class T>
    class CSDSingleton : private T
    {
    private:
        CSDSingleton();
        ~CSDSingleton();

    public:
        static T& Instance();
    };


    template <class T>
    inline CSDSingleton<T>::CSDSingleton()
    {
    }

    template <class T>
    inline CSDSingleton<T>::~CSDSingleton()
    {
    }

    template <class T>
    /*static*/ T &CSDSingleton<T>::Instance()
    {
        // function-local static to force this to work correctly at static
        // initialization time.
        static CSDSingleton<T> s_instance;
        return(s_instance);
    }

}



#endif
