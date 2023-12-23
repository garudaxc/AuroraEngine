#ifndef GATYPES_H
#define GATYPES_H
#include <limits>
#include <string>

namespace Aurora
{

    typedef unsigned int uint;

    typedef char int8;
    typedef unsigned char uint8;
    typedef short int16;
    typedef unsigned short uint16;
    typedef int int32;
    typedef unsigned int uint32;
    typedef __int64 int64;
    typedef unsigned __int64 uint64;

    typedef uint32 bitfield;

    typedef std::string ResourceID;

    typedef uint32 FourCC;

    typedef int32 Handle;

    // template<typename T>
    // T MaxValueOfType()
    // {
    //     return std::numeric_limits<T>::max();
    // }

    struct RectSize
    {
        RectSize(int32 InWidth, int32 InHeight): Width(InWidth), Height(InHeight)
        {
        }

        RectSize()
        {
        }

        int32 Width = 0;
        int32 Height = 0;
    };



}





#endif