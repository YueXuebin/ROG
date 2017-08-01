#ifndef SDPOINT_H
#define SDPOINT_H


namespace Crown
{
    class SDPoint
    {
    public:
        SDPoint() : x(0), y(0) {}
        ~SDPoint() {}

        int32 x;
        int32 y;
    };
}

#endif // SDPOINT_H
