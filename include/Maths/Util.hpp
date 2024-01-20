#pragma once
#include <math.h>
#include <Maths/Vec2.hpp>
#include <Linxc.h>

#define PI 3.1415927f
#define DOUBLEPI 3.14159265358979323846
constexpr float Degree2Radian = 3.1415927f / 180.0f;
constexpr float Radian2Degree = 180.0f / 3.1415927f;

namespace Maths
{
	float inline LengthdirX(float len, float dirInRadians)
	{
		return cosf(dirInRadians) * len;
	}
	float inline LengthdirY(float len, float dirInRadians)
	{
		return sinf(dirInRadians) * len;
	}
	Vec2 inline Lengthdir(float len, float dirInRadians)
	{
		return Vec2(cosf(dirInRadians) * len, sinf(dirInRadians) * len);
	}
	float inline DirectionTowards(Vec2 A, Vec2 B)
	{
		return atan2f(B.Y - A.Y, B.X - A.X);
	}
	float inline DirectionTowards(float AX, float AY, float BX, float BY)
	{
		return atan2f(BY - AY, BX - AX);
	}
	i32 inline Sign(float A)
	{
		if (A < 0.0f)
		{
			return -1;
		}
		else if (A > 0.0f)
		{
			return 1;
		}
		return 0;
	}
	i32 inline Sign(i32 A)
	{
		if (A < 0)
		{
			return -1;
		}
		else if (A > 0)
		{
			return 1;
		}
		return 0;
	}
	float inline QuadraticStep(float A, float B, float amount)
	{
		float equation = Sign(B - A) * amount * amount + (B < A ? 1.0f : 0.0f);
		return A + (B - A) * equation;
	}
	float inline BarycentricCoords(float A, float B, float C, float amount1, float amount2)
	{
		return A + (B - A) * amount1 + (C - A) * amount2;
	}
	float inline Clamp(float min, float max, float value)
	{
		if (value < min)
		{
			return min;
		}
		else if (value > max)
		{
			return max;
		}
		return value;
	}
	float inline Distance(float A, float B)
	{
		return fabsf(B - A);
	}
	float inline Hermite(float v1, float t1, float v2, float t2, float amount)
	{
		double result;
		double amtCubed = amount * amount * amount;
		double amtSquared = amount * amount;

		if (amount == 0.0f)
		{
			result = v1;
		}
		else if (amount == 1.0f)
		{
			result = v2;
		}
		else
		{
			result = (2 * v1 - 2 * v2 + t2 + t1) * amtCubed +
				(3 * v2 - 3 * v1 - 2 * t1 - t2) * amtSquared +
				t1 * amount +
				v1;
		}
		return (float)result;
	}
	float inline Lerp(float A, float B, float amount)
	{
		return A + (B - A) * amount;
	}
	float inline ModuloFloat(float A, float B)
	{
		i32 n = floorl(A / B);
		return A - (n * B);
	}
	float inline WrapAngle(float angle)
	{
		if ((angle > -PI) && (angle <= PI))
			return angle;
		angle = ModuloFloat(angle, PI * 2.0f);
		if (angle <= -PI)
			return angle + PI * 2.0f;
		if (angle > PI)
			return angle - PI * 2.0f;
		return angle;
	}
	float inline LerpAngle(float current, float designated, float amt)
	{
		if (designated < current)
		{
			if (designated + (180.0f * Degree2Radian) < current)
			{
				float distance = (2 * PI) - fabsf(designated - current);
				return WrapAngle(current + (amt * distance));
			}
			else
			{
				float distance = current - designated;
				return WrapAngle(current - (distance * amt));
			}
		}
		else if (current < designated)
		{
			if (designated - (180.0f * Degree2Radian) > current)
			{
				float distance = (2 * PI) - fabsf(designated - current);
				return WrapAngle(current - (amt * distance));
			}
			else
			{
				float distance = designated - current;
				return WrapAngle(current + (distance * amt));
			}
		}
		else return current;
	}
	constexpr float Approach(float value, float toApproach, float amount)
	{
		return value + (toApproach - value) * amount;
	}
	inline float ApproachAngle(float curAngle, float targetAngle, float maxChange)
	{
		curAngle = WrapAngle(curAngle);
		targetAngle = WrapAngle(targetAngle);
		if (curAngle < targetAngle)
		{
			if (targetAngle - curAngle > PI)
			{
				curAngle += PI * 2.0f;
			}
		}
		else if (curAngle - targetAngle > PI)
		{
			curAngle -= PI * 2.0f;
		}
		curAngle += Clamp(targetAngle - curAngle, -maxChange, maxChange);
		return WrapAngle(curAngle);
	}
}