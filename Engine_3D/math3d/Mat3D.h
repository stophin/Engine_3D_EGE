// Mat3D.h
// Matrix for vertex transformation

#ifndef _MAT3D_H_
#define _MAT3D_H_

#include "../common/EPoint.h"

typedef class Mat Mat;
class Mat {
public:
	Mat() :x(0), y(0), z(0), w(1) {

	}
	Mat(EFTYPE x, EFTYPE y, EFTYPE z) :x(x), y(y), z(z), w(1) {

	}
	Mat(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w) :x(x), y(y), z(z), w(w) {

	}
	Mat(const Mat & v) : x(v.x), y(v.y), z(v.z), w(v.w) {

	}
	~Mat(){

	}

	EFTYPE x;
	EFTYPE y;
	EFTYPE z;
	EFTYPE w;

	void set(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;
	}
	void set(EFTYPE x, EFTYPE y, EFTYPE z) {
		this->x = x;
		this->y = y;
		this->z = z;
	}
	Mat& set(const Mat& m) {
		this->x = m.x;
		this->y = m.y;
		this->z = m.z;
		this->w = m.w;

		return *this;
	}

	Mat& operator *(const Mat& m) {
		EFTYPE x = this->x, y = this->y, z = this->z;
		this->x = y * m.z - z * m.y;
		this->y = z * m.x - x * m.z;
		this->z = x * m.y - y * m.x;

		return *this;
	}
	Mat& operator *(float s) {
		this->x *= s;
		this->y *= s;
		this->z *= s;

		return *this;
	}
	Mat& operator =(const Mat& m) {
		this->x = m.x;
		this->y = m.y;
		this->z = m.z;
		this->w = m.w;

		return *this;
	}
	Mat& operator +=(const Mat& m) {
		this->x += m.x;
		this->y += m.y;
		this->z += m.z;
		this->w += m.w;

		return *this;
	}
	Mat& operator -=(const Mat& m) {
		this->x -= m.x;
		this->y -= m.y;
		this->z -= m.z;
		this->w -= m.w;

		return *this;
	}
};

typedef class Mat3D Mat3D;
class Mat3D {
public:
	Mat3D() : mx(1, 0, 0, 0), my(0, 1, 0, 0), mz(0, 0, 1, 0), mw(0, 0, 0, 1) {

	}
	Mat3D(const Mat& mx, const Mat& my, const Mat& mz, const Mat& mw) :
		mx(mx), my(my), mz(mz), mw(mw)	{

	}
	~Mat3D() {

	}

#ifdef USING_SIMD_INTRINSIC
	union {
		struct {
			Mat mx;
			Mat my;
			Mat mz;
			Mat mw;
		};
		EFTYPE _mm[16];
	};
#else
	Mat mx;
	Mat my;
	Mat mz;
	Mat mw;
#endif

	Mat3D& set(const Mat3D& m) {
		this->mx.set(m.mx);
		this->my.set(m.my);
		this->mz.set(m.mz);
		this->mw.set(m.mw);

		return *this;
	}

	Mat3D& setI() {
		this->mx.set(1, 0, 0, 0);
		this->my.set(0, 1, 0, 0);
		this->mz.set(0, 0, 1, 0);
		this->mw.set(0, 0, 0, 1);

		return *this;
	}

	void cross(Mat& t, const Mat& m) {
		t.x = this->mx.x * m.x + this->my.x * m.y + this->mz.x * m.z + this->mw.x * m.w;
		t.y = this->mx.y * m.x + this->my.y * m.y + this->mz.y * m.z + this->mw.y * m.w;
		t.z = this->mx.z * m.x + this->my.z * m.y + this->mz.z * m.z + this->mw.z * m.w;
		t.w = this->mx.w * m.x + this->my.w * m.y + this->mz.w * m.z + this->mw.w * m.w;
	}

#ifdef USING_SIMD_INTRINSIC
	Mat3D& operator * (const Mat3D& m) {
		__declspec(align(16)) __m256 temp;
		__declspec(align(16)) __m256 a12, a34;
		__declspec(align(16)) __m256 b11, b22, b33, b44;
		a12 = _mm256_i32gather_ps(this->_mm, gatherA12, sizeof(float));
		a34 = _mm256_i32gather_ps(this->_mm, gatherA34, sizeof(float));

		b11 = _mm256_i32gather_ps(m._mm, gatherB11, sizeof(float));
		b22 = _mm256_i32gather_ps(m._mm, gatherB22, sizeof(float));
		b33 = _mm256_i32gather_ps(m._mm, gatherB33, sizeof(float));
		b44 = _mm256_i32gather_ps(m._mm, gatherB44, sizeof(float));

		temp = _mm256_dp_ps(a12, b11, 0b11110001);
		this->_mm[0] = temp.m256_f32[0];
		this->_mm[1] = temp.m256_f32[4];
		temp = _mm256_dp_ps(a34, b11, 0b11110001);
		this->_mm[2] = temp.m256_f32[0];
		this->_mm[3] = temp.m256_f32[4];

		temp = _mm256_dp_ps(a12, b22, 0b11110001);
		this->_mm[4] = temp.m256_f32[0];
		this->_mm[5] = temp.m256_f32[4];
		temp = _mm256_dp_ps(a34, b22, 0b11110001);
		this->_mm[6] = temp.m256_f32[0];
		this->_mm[7] = temp.m256_f32[4];

		temp = _mm256_dp_ps(a12, b33, 0b11110001);
		this->_mm[8] = temp.m256_f32[0];
		this->_mm[9] = temp.m256_f32[4];
		temp = _mm256_dp_ps(a34, b33, 0b11110001);
		this->_mm[10] = temp.m256_f32[0];
		this->_mm[11] = temp.m256_f32[4];

		temp = _mm256_dp_ps(a12, b44, 0b11110001);
		this->_mm[12] = temp.m256_f32[0];
		this->_mm[13] = temp.m256_f32[4];
		temp = _mm256_dp_ps(a34, b44, 0b11110001);
		this->_mm[14] = temp.m256_f32[0];
		this->_mm[15] = temp.m256_f32[4];

		return *this;
	}
#else

	Mat3D& operator *(const Mat3D& m) {
		//Mat mx = this->mx, my = this->my, mz = this->mz, mw = this->mw;
		Mat mx, my, mz, mw;
		this->cross(mx, m.mx);
		this->cross(my, m.my);
		this->cross(mz, m.mz);
		this->cross(mw, m.mw);

		this->mx = mx;
		this->my = my;
		this->mz = mz;
		this->mw = mw;

		return *this;
	}
#endif

	Mat3D& operator = (const Mat3D& m) {
		this->mx = m.mx;
		this->my = m.my;
		this->mz = m.mz;
		this->mw = m.mw;

		return *this;
	}
	Mat3D& operator + (const Mat3D& m) {
		this->mx += m.mx;
		this->my += m.my;
		this->mz += m.mz;
		this->mw += m.mw;

		return *this;
	}
	Mat3D& operator - (const Mat3D& m) {
		this->mx -= m.mx;
		this->my -= m.my;
		this->mz -= m.mz;
		this->mw -= m.mw;

		return *this;
	}

	Mat3D& operator * (float s) {
		this->mx * s;
		this->my * s;
		this->mz * s;
		this->mw * s;

		return *this;
	}

	//M-T
	Mat3D& transpose() {
		Mat mx = this->mx, my = this->my, mz = this->mz, mw = this->mw;
		this->mx.y = this->my.x;
		this->mx.z = this->mz.x;
		this->mx.w = this->mw.x;

		this->my.x = this->mx.y;
		this->my.z = this->mz.y;
		this->my.w = this->mw.y;
		
		this->mz.x = this->mx.z;
		this->mz.y = this->my.z;
		this->mz.w = this->mw.z;

		this->mw.x = this->mx.w;
		this->mw.y = this->my.w;
		this->mw.z = this->mz.w;

		return *this;
	}
};

#endif