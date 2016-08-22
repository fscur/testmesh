#pragma once

#include <unordered_map>

enum parameterSemantic
{
    NONE,
    LOCAL,
    MODEL,
    PROJECTION,
    MODELVIEW,
    MODELVIEWPROJECTION,
    MODELINVERSE,
    VIEWINVERSE,
    PROJECTIONINVERSE,
    MODELVIEWINVERSE,
    MODELVIEWPROJECTIONINVERSE,
    MODELINVERSETRANSPOSE,
    MODELVIEWINVERSETRANSPOSE,
    VIEWPORT,
    JOINTMATRIX,
    POSITION,
    NORMAL,
    TEXCOORD,
    COLOR,
    JOINT,
    WEIGHT
};

namespace std
{
    template<>
    struct hash< ::parameterSemantic>
    {
        typedef ::parameterSemantic argument_type;
        typedef std::underlying_type< argument_type >::type underlying_type;
        typedef std::hash< underlying_type >::result_type result_type;
        result_type operator()(const argument_type& arg) const
        {
            std::hash< underlying_type > hasher;
            return hasher(static_cast< underlying_type >(arg));
        }
    };
}

