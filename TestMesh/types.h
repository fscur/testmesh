#pragma once
#include <vector>

using uint = unsigned int;
using byte = unsigned char;

template<typename T>
bool contains(std::vector<T> vector, T value)
{
    auto it = std::find(vector.begin(), vector.end(), value);

    return it != vector.end();
}