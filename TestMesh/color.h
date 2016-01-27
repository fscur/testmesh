#ifndef _COLOR_H_
#define _COLOR_H_
#include <algorithm>
struct color
{
public:
    float R;
    float G;
    float B;
    float A;

public:
    color(float r = 0.0f, float g = 0.0f, float b = 0.0f, float a = 1.0f)
	{
		R = r < 0.0f ? 0.0f : r > 1.0f ? 1.0f : r;
		G = g < 0.0f ? 0.0f : g > 1.0f ? 1.0f : g;
		B = b < 0.0f ? 0.0f : b > 1.0f ? 1.0f : b;
		A = a < 0.0f ? 0.0f : a > 1.0f ? 1.0f : a;
	}

    static color fromRGBA(long rgba)
    {
        int rgbaInt = (int)rgba;

        unsigned char r = rgbaInt & 255;
        rgbaInt = rgbaInt >> 8;
        unsigned  g = rgbaInt & 255;
        rgbaInt = rgbaInt >> 8;
        unsigned  b = rgbaInt & 255;
        rgbaInt = rgbaInt >> 8;
        unsigned  a = rgbaInt & 255;

        auto rf = std::max<float>(std::min<float>(std::abs((float)r / 255.0f), 1.0f), 0.0f);
        auto gf = std::max<float>(std::min<float>(std::abs((float)g / 255.0f), 1.0f), 0.0f);
        auto bf = std::max<float>(std::min<float>(std::abs((float)b / 255.0f), 1.0f), 0.0f);
        auto af = std::max<float>(std::min<float>(std::abs((float)a / 255.0f), 1.0f), 0.0f);

        return color(rf, gf, bf, af);
    }
};

#endif