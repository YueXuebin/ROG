#pragma once

template <class T, typename P1>
struct LambdaNew1
{
    P1 p1;

    LambdaNew1(P1 p1):
        p1(p1)
    {}

    void operator ()() const
    {
        new T(p1);
    }
};

template <class T, typename P1>
LambdaNew1<T, P1> MakeLambdaNew(P1 p1)
{
    return LambdaNew1<T, P1>(p1);
}

template <class T, typename P1, typename P2>
struct LambdaNew2
{
    P1 p1; P2 p2;

    LambdaNew2(P1 p1, P2 p2):
        p1(p1), p2(p2)
    {}

    void operator ()() const
    {
        new T(p1, p2);
    }
};

template <class T, typename P1, typename P2>
LambdaNew2<T, P1, P2> MakeLambdaNew(P1 p1, P2 p2)
{
    return LambdaNew2<T, P1, P2>(p1, p2);
}

template <class T, typename P1, typename P2, typename P3>
struct LambdaNew3
{
    P1 p1; P2 p2; P3 p3;

    LambdaNew3(P1 p1, P2 p2, P3 p3):
        p1(p1), p2(p2), p3(p3)
    {}

    void operator ()() const
    {
        new T(p1, p2, p3);
    }
};

template <class T, typename P1, typename P2, typename P3>
LambdaNew3<T, P1, P2, P3> MakeLambdaNew(P1 p1, P2 p2, P3 p3)
{
    return LambdaNew3<T, P1, P2, P3>(p1, p2, p3);
}

template <class T, typename P1, typename P2, typename P3, typename P4>
struct LambdaNew4
{
    P1 p1; P2 p2; P3 p3; P4 p4;

    LambdaNew4(P1 p1, P2 p2, P3 p3, P4 p4):
        p1(p1), p2(p2), p3(p3), p4(p4)
    {}

    void operator ()() const
    {
        new T(p1, p2, p3, p4);
    }
};

template <class T, typename P1, typename P2, typename P3, typename P4>
LambdaNew4<T, P1, P2, P3, P4> MakeLambdaNew(P1 p1, P2 p2, P3 p3, P4 p4)
{
    return LambdaNew4<T, P1, P2, P3, P4>(p1, p2, p3, p4);
}
