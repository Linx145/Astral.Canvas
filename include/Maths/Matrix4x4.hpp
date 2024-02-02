#pragma once
#include <math.h>
#include "Maths/Vec4.hpp"
#include "Maths/simd.h"
#include "Maths/Util.hpp"
#include "Maths/Quaternion.hpp"
#include "option.hpp"

namespace Maths
{
	struct Matrix4x4
	{
		//Row 1
		union
		{
			struct
			{
				float M11;
				float M12;
				float M13;
				float M14;
				//Row 2
				float M21;
				float M22;
				float M23;
				float M24;
				//Row 3
				float M31;
				float M32;
				float M33;
				float M34;
				//Row 4
				float M41;
				float M42;
				float M43;
				float M44;
			};
#ifdef USE_SSE
			struct
			{
				__m128 row1;
				__m128 row2;
				__m128 row3;
				__m128 row4;
			};
#else
			struct
			{
				Vec4 row1;
				Vec4 row2;
				Vec4 row3;
				Vec4 row4;
			};
#endif
		};

		Matrix4x4()
		{

		}
		Matrix4x4(float fill)
		{
#ifdef USE_SSE
			__m128 asM128 = _mm_set_ps1(fill);
			row1 = asM128;
			row2 = asM128;
			row3 = asM128;
			row4 = asM128;
#else
			M11 = M12 = M13 = M14 = fill;
			M21 = M22 = M23 = M24 = fill;
			M31 = M32 = M33 = M34 = fill;
			M41 = M42 = M43 = M44 = fill;
#endif
		}
		Matrix4x4(float* m)
		{
#ifdef USE_SSE
			row1 = _mm_load_ps(m);//_mm_set_ps(m[0], m[1], m[2], m[3]);
			row2 = _mm_load_ps(m + 4);//_mm_set_ps(m[4], m[5], m[6], m[7]);
			row3 = _mm_load_ps(m + 8);//_mm_set_ps(m[8], m[9], m[10], m[11]);
			row4 = _mm_load_ps(m + 12);//_mm_set_ps(m[12], m[13], m[14], m[15]);
#else
			M11 = m[0];
			M12 = m[1];
			M13 = m[2];
			M14 = m[3];

			M21 = m[4];
			M22 = m[5];
			M23 = m[6];
			M24 = m[7];

			M31 = m[8];
			M32 = m[9];
			M33 = m[10];
			M34 = m[11];

			M41 = m[12];
			M42 = m[13];
			M43 = m[14];
			M44 = m[15];
#endif
		}

		inline static Matrix4x4 Identity()
		{
			float m[16] = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline bool IsIdentity()
		{
			return M11 == 1.0f && M22 == 1.0f && M33 == 1.0f && M44 == 1.0f &&
				M12 == 0.0f && M13 == 0.0f && M14 == 0.0f &&
				M21 == 0.0f && M23 == 0.0f && M24 == 0.0f &&
				M31 == 0.0f && M32 == 0.0f && M34 == 0.0f &&
				M41 == 0.0f && M42 == 0.0f && M43 == 0.0f;
		}
		inline static Matrix4x4 CreateTranslation(float X, float Y, float Z)
		{
			float m[16] = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					X, Y, Z, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateTranslation(Vec3 pos)
		{
			float m[16] = {
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					pos.X, pos.Y, pos.Z, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateScale(float X, float Y, float Z)
		{
			float m[16] = {
					X, 0.0f, 0.0f, 0.0f,
					0.0f, Y, 0.0f, 0.0f,
					0.0f, 0.0f, Z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateScale(Vec3 scale)
		{
			float m[16] = {
					scale.X, 0.0f, 0.0f, 0.0f,
					0.0f, scale.Y, 0.0f, 0.0f,
					0.0f, 0.0f, scale.Z, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateScale(float scale)
		{
			return CreateScale(scale, scale, scale);
		}
		inline static Matrix4x4 CreateRotationZ(float s, float c)
		{
			float m[16] = {
				c, s, 0.0f, 0.0f,
				-s, c, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreateRotationZ(float angleRadians)
		{
			return CreateRotationZ(sinf(angleRadians), cosf(angleRadians));
		}
		inline static Matrix4x4 CreateFromAxisAngle(Vec3 axis, float angle)
		{
			float x = axis.X, y = axis.Y, z = axis.Z;
			float sa = sinf(angle);
			float ca = cosf(angle);
			float xx = x * x;
			float yy = y * y;
			float zz = z * z;
			float xy = x * y;
			float xz = x * z;
			float yz = y * z;

			float m[16] = {
				xx + ca * (1.0f - xx), xy - ca * xy + sa * z, xz - ca * xz - sa * y, 0.0f,
				xy - ca * xy - sa * z, yy + ca * (1.0f - yy), yz - ca * yz + sa * x, 0.0f,
				xz - ca * xz + sa * y, yz - ca * yz - sa * x, zz + ca * (1.0f - zz), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(&m[0]);
		}
		inline static Matrix4x4 CreatePerspectiveFOV(float FOV, float aspectRatio, float nearPlaneDistance, float farPlaneDistance)
		{
#if DEBUG
			if (FOV <= 0.0f || FOV >= PI || nearPlaneDistance <= 0.0f || farPlaneDistance <= 0.0f || farPlaneDistance < nearPlaneDistance)
			{
				return Identity();
			}
#endif
			float yScale = 1.0f / tanf(FOV * 0.5f);
			float xScale = yScale / aspectRatio;
			float m[16] = {
				xScale, 0.0f, 0.0f, 0.0f,
				0.0f, yScale, 0.0f, 0.0f,
				0.0f, 0.0f, farPlaneDistance / (nearPlaneDistance - farPlaneDistance), -1.0f,
				0.0f, 0.0f, nearPlaneDistance* farPlaneDistance / (nearPlaneDistance - farPlaneDistance), 0.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateOrthographic(float width, float height, float nearZ, float farZ)
		{
			float m[16] = {
				2.0f / width, 0.0f, 0.0f, 0.0f,
				0.0f, 2.0f / height, 0.0f, 0.0f,
				0.0f, 0.0f, 1.0f / (nearZ - farZ), 0.0f,
				0.0f, 0.0f, nearZ / (nearZ - farZ), 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateLookAt(Vec3 cameraPosition, Vec3 cameraTarget, Vec3 up)
		{
			Vec3 zAxis = (cameraPosition - cameraTarget).Normalized();
			Vec3 xAxis = Vec3::Cross(up, zAxis).Normalized();
			Vec3 yAxis = Vec3::Cross(zAxis, xAxis);

			float m[16] = {
				xAxis.X, yAxis.X, zAxis.X, 0.0f,
				xAxis.Y, yAxis.Y, zAxis.Y, 0.0f,
				xAxis.Z, yAxis.Z, zAxis.Z, 0.0f,
				-Vec3::Dot(xAxis, cameraPosition), -Vec3::Dot(yAxis, cameraPosition), -Vec3::Dot(zAxis, cameraPosition), 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateWorld(Vec3 position, Vec3 forward, Vec3 up)
		{
			Vec3 zAxis = (-forward).Normalized();
			Vec3 xAxis = Vec3::Cross(up, zAxis).Normalized();
			Vec3 yAxis = Vec3::Cross(zAxis, xAxis);

			float m[16] = {
				xAxis.X, yAxis.X, zAxis.X, 0.0f,
				xAxis.Y, yAxis.Y, zAxis.Y, 0.0f,
				xAxis.Z, yAxis.Z, zAxis.Z, 0.0f,
				position.X, position.Y, position.Z, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateFromQuaternion(Quaternion quaternion)
		{
			float xx = quaternion.X * quaternion.X;
			float yy = quaternion.Y * quaternion.Y;
			float zz = quaternion.Z * quaternion.Z;

			float xy = quaternion.X * quaternion.Y;
			float wz = quaternion.Z * quaternion.W;
			float xz = quaternion.Z * quaternion.X;
			float wy = quaternion.Y * quaternion.W;
			float yz = quaternion.Y * quaternion.Z;
			float wx = quaternion.X * quaternion.W;

			float m[16] = {
				1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f,
				2.0f * (xy - wz), 1.0f - 2.0f * (zz + xx), 2.0f * (yz + wx), 0.0f,
				2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (yy + xx), 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f
			};
			return Matrix4x4(m);
		}
		inline static Matrix4x4 CreateFromYawPitchRoll(float yaw, float pitch, float roll)
		{
			return Matrix4x4::CreateFromQuaternion(Quaternion::FromYawPitchRoll(yaw, pitch, roll));
		}
		option<Matrix4x4> Invert()
		{
			float a = M11, b = M12, c = M13, d = M14;
			float e = M21, f = M22, g = M23, h = M24;
			float i = M31, j = M32, k = M33, l = M34;
			float m = M41, n = M42, o = M43, p = M44;

			float kp_lo = k * p - l * o;
			float jp_ln = j * p - l * n;
			float jo_kn = j * o - k * n;
			float ip_lm = i * p - l * m;
			float io_km = i * o - k * m;
			float in_jm = i * n - j * m;

			float a11 = +(f * kp_lo - g * jp_ln + h * jo_kn);
			float a12 = -(e * kp_lo - g * ip_lm + h * io_km);
			float a13 = +(e * jp_ln - f * ip_lm + h * in_jm);
			float a14 = -(e * jo_kn - f * io_km + g * in_jm);

			float det = a * a11 + b * a12 + c * a13 + d * a14;

			if (fabsf(det) <= 0.0f)
			{
				return option<Matrix4x4>();
			}

			float invDet = 1.0f / det;

			Matrix4x4 result;
			result.M11 = a11 * invDet;
			result.M21 = a12 * invDet;
			result.M31 = a13 * invDet;
			result.M41 = a14 * invDet;

			result.M12 = -(b * kp_lo - c * jp_ln + d * jo_kn) * invDet;
			result.M22 = +(a * kp_lo - c * ip_lm + d * io_km) * invDet;
			result.M32 = -(a * jp_ln - b * ip_lm + d * in_jm) * invDet;
			result.M42 = +(a * jo_kn - b * io_km + c * in_jm) * invDet;

			float gp_ho = g * p - h * o;
			float fp_hn = f * p - h * n;
			float fo_gn = f * o - g * n;
			float ep_hm = e * p - h * m;
			float eo_gm = e * o - g * m;
			float en_fm = e * n - f * m;

			result.M13 = +(b * gp_ho - c * fp_hn + d * fo_gn) * invDet;
			result.M23 = -(a * gp_ho - c * ep_hm + d * eo_gm) * invDet;
			result.M33 = +(a * fp_hn - b * ep_hm + d * en_fm) * invDet;
			result.M43 = -(a * fo_gn - b * eo_gm + c * en_fm) * invDet;

			float gl_hk = g * l - h * k;
			float fl_hj = f * l - h * j;
			float fk_gj = f * k - g * j;
			float el_hi = e * l - h * i;
			float ek_gi = e * k - g * i;
			float ej_fi = e * j - f * i;

			result.M14 = -(b * gl_hk - c * fl_hj + d * fk_gj) * invDet;
			result.M24 = +(a * gl_hk - c * el_hi + d * ek_gi) * invDet;
			result.M34 = -(a * fl_hj - b * el_hi + d * ej_fi) * invDet;
			result.M44 = +(a * fk_gj - b * ek_gi + c * ej_fi) * invDet;

			return option<Matrix4x4>(result);
		}

		Matrix4x4 operator*(const Matrix4x4 other)
		{
#ifdef USE_SSE
			Matrix4x4 m;

			{
				__m128 e0 = _mm_shuffle_ps(other.row1, other.row1, _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(other.row1, other.row1, _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(other.row1, other.row1, _MM_SHUFFLE(2, 2, 2, 2));
				__m128 e3 = _mm_shuffle_ps(other.row1, other.row1, _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(row1, e0);
				__m128 m1 = _mm_mul_ps(row2, e1);
				__m128 m2 = _mm_mul_ps(row3, e2);
				__m128 m3 = _mm_mul_ps(row4, e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, a1);

				m.row1 = a2;
			}

			{
				__m128 e0 = _mm_shuffle_ps(other.row2, other.row2, _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(other.row2, other.row2, _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(other.row2, other.row2, _MM_SHUFFLE(2, 2, 2, 2));
				__m128 e3 = _mm_shuffle_ps(other.row2, other.row2, _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(row1, e0);
				__m128 m1 = _mm_mul_ps(row2, e1);
				__m128 m2 = _mm_mul_ps(row3, e2);
				__m128 m3 = _mm_mul_ps(row4, e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, a1);

				m.row2 = a2;
			}

			{
				__m128 e0 = _mm_shuffle_ps(other.row3, other.row3, _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(other.row3, other.row3, _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(other.row3, other.row3, _MM_SHUFFLE(2, 2, 2, 2));
				__m128 e3 = _mm_shuffle_ps(other.row3, other.row3, _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(row1, e0);
				__m128 m1 = _mm_mul_ps(row2, e1);
				__m128 m2 = _mm_mul_ps(row3, e2);
				__m128 m3 = _mm_mul_ps(row4, e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, a1);

				m.row3 = a2;
			}

			{
				__m128 e0 = _mm_shuffle_ps(other.row4, other.row4, _MM_SHUFFLE(0, 0, 0, 0));
				__m128 e1 = _mm_shuffle_ps(other.row4, other.row4, _MM_SHUFFLE(1, 1, 1, 1));
				__m128 e2 = _mm_shuffle_ps(other.row4, other.row4, _MM_SHUFFLE(2, 2, 2, 2));
				__m128 e3 = _mm_shuffle_ps(other.row4, other.row4, _MM_SHUFFLE(3, 3, 3, 3));

				__m128 m0 = _mm_mul_ps(row1, e0);
				__m128 m1 = _mm_mul_ps(row2, e1);
				__m128 m2 = _mm_mul_ps(row3, e2);
				__m128 m3 = _mm_mul_ps(row4, e3);

				__m128 a0 = _mm_add_ps(m0, m1);
				__m128 a1 = _mm_add_ps(m2, m3);
				__m128 a2 = _mm_add_ps(a0, a1);

				m.row4 = a2;
			}

			return m;
#else
			Matrix4x4 m;

			// First row
			m.M11 = M11 * other.M11 + M12 * other.M21 + M13 * other.M31 + M14 * other.M41;
			m.M12 = M11 * other.M12 + M12 * other.M22 + M13 * other.M32 + M14 * other.M42;
			m.M13 = M11 * other.M13 + M12 * other.M23 + M13 * other.M33 + M14 * other.M43;
			m.M14 = M11 * other.M14 + M12 * other.M24 + M13 * other.M34 + M14 * other.M44;

			// Second row
			m.M21 = M21 * other.M11 + M22 * other.M21 + M23 * other.M31 + M24 * other.M41;
			m.M22 = M21 * other.M12 + M22 * other.M22 + M23 * other.M32 + M24 * other.M42;
			m.M23 = M21 * other.M13 + M22 * other.M23 + M23 * other.M33 + M24 * other.M43;
			m.M24 = M21 * other.M14 + M22 * other.M24 + M23 * other.M34 + M24 * other.M44;

			// Third row
			m.M31 = M31 * other.M11 + M32 * other.M21 + M33 * other.M31 + M34 * other.M41;
			m.M32 = M31 * other.M12 + M32 * other.M22 + M33 * other.M32 + M34 * other.M42;
			m.M33 = M31 * other.M13 + M32 * other.M23 + M33 * other.M33 + M34 * other.M43;
			m.M34 = M31 * other.M14 + M32 * other.M24 + M33 * other.M34 + M34 * other.M44;

			// Fourth row
			m.M41 = M41 * other.M11 + M42 * other.M21 + M43 * other.M31 + M44 * other.M41;
			m.M42 = M41 * other.M12 + M42 * other.M22 + M43 * other.M32 + M44 * other.M42;
			m.M43 = M41 * other.M13 + M42 * other.M23 + M43 * other.M33 + M44 * other.M43;
			m.M44 = M41 * other.M14 + M42 * other.M24 + M43 * other.M34 + M44 * other.M44;

			return m;
#endif
		}
		inline Matrix4x4 operator*=(const Matrix4x4 other)
		{
			*this = *this * other;
		}

		inline bool operator==(const Matrix4x4 other)
		{
			return M11 == other.M11 && M22 == other.M22 && M33 == other.M33 && M44 == other.M44 &&
				M12 == other.M12 && M13 == other.M13 && M14 == other.M14 &&
				M21 == other.M21 && M23 == other.M23 && M24 == other.M24 &&
				M31 == other.M31 && M32 == other.M32 && M34 == other.M34 &&
				M41 == other.M41 && M42 == other.M42 && M43 == other.M43;
		}
		inline bool operator!=(const Matrix4x4 other)
		{
			return (M11 != other.M11 || M12 != other.M12 || M13 != other.M13 || M14 != other.M14 ||
				M21 != other.M21 || M22 != other.M22 || M23 != other.M23 || M24 != other.M24 ||
				M31 != other.M31 || M32 != other.M32 || M33 != other.M33 || M34 != other.M34 ||
				M41 != other.M41 || M42 != other.M42 || M43 != other.M43 || M44 != other.M44);
		}

		inline static Matrix4x4 Lerp(const Matrix4x4 A, const Matrix4x4 B, float amount)
		{
#ifdef USE_SSE
			__m128 amountAsM128 = _mm_set_ps1(amount);
			Matrix4x4 m;
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.row1, sub);
				m.row1 = _mm_add_ps(_mm_mul_ps(B.row1, amountAsM128), mul);
			}
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.row2, sub);
				m.row2 = _mm_add_ps(_mm_mul_ps(B.row2, amountAsM128), mul);
			}
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.row3, sub);
				m.row3 = _mm_add_ps(_mm_mul_ps(B.row3, amountAsM128), mul);
			}
			{
				__m128 sub = _mm_sub_ps(_mm_set1_ps(1.0f), amountAsM128);
				__m128 mul = _mm_mul_ps(A.row4, sub);
				m.row4 = _mm_add_ps(_mm_mul_ps(B.row4, amountAsM128), mul);
			}
			return m;
#else
			Matrix4x4 m;

			m.row1 = Vec4::Lerp(A.row1, B.row1, amount);
			m.row2 = Vec4::Lerp(A.row2, B.row2, amount);
			m.row3 = Vec4::Lerp(A.row3, B.row3, amount);
			m.row4 = Vec4::Lerp(A.row4, B.row4, amount);

			return m;
#endif
		}
	};
}