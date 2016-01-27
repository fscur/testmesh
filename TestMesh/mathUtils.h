#ifndef _MATH_UTILS_H_
#define _MATH_UTILS_H_

#include <glm/glm.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <vector>

class mathUtils
{
public:
		
	static float max(float a, float b)
	{
		if (a > b)
			return a;
		else
			return b;
	}

	static float round(float value, int decimals)
	{
		float exp = pow(10.0f, (float)decimals);
		value *= exp;

		int istack = (int)floor(value);
		float out = value - istack;

		if (out < 0.5)
		{
			value = floor(value);
			value /= exp;
			return value;
		}

		if (out > 0.4) 
		{
			value = ceil(value);
			value /= exp;
			return value;
		}
	}

    static float distance(glm::vec3 v0, glm::vec3 v1);
    static std::vector<glm::vec3> rotateAboutAxis(std::vector<glm::vec3>* points, glm::vec3 origin, glm::vec3 axis, float angle);
    static std::vector<glm::vec3> rotateAboutAxis(std::vector<glm::vec3>* points, glm::vec3 axis, float angle);
    static glm::vec3 rotateAboutAxis(glm::vec3 point, glm::vec3 origin, glm::vec3 axis, float angle);
    static glm::vec3 rotateAboutAxis(glm::vec3 point, glm::vec3 axis, float angle);
    static glm::mat4 getRotationMatrixAboutAnArbitraryAxis(glm::vec3 origin, glm::vec3 axis, float angle);
};

#endif
