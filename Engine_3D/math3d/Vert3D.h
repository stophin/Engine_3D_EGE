// Vert3D.h
// Define 3D vertex
//

#ifndef _VERT3D_H_
#define _VERT3D_H_

#include "Mat3d.h"

typedef struct Vert3D Vert3D;
struct Vert3D {

	EFTYPE x;
	EFTYPE y;
	EFTYPE z;
	EFTYPE w;

	void init() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
		this->w = 1;
	}

	Vert3D& set(const Vert3D& v) {
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
		this->w = v.w;

		return *this;
	}

	Vert3D&  set(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->w = w;

		return *this;
	}
	Vert3D&  set(EFTYPE x, EFTYPE y, EFTYPE z) {
		this->x = x;
		this->y = y;
		this->z = z;

		return *this;
	}


	Vert3D& operator *(const Mat3D& m) {
		EFTYPE x = this->x, y = this->y, z = this->z, w = this->w;
		this->x = x * m.mx.x + y * m.mx.y + z * m.mx.z + w * m.mx.w;
		this->y = x * m.my.x + y * m.my.y + z * m.my.z + w * m.my.w;
		this->z = x * m.mz.x + y * m.mz.y + z * m.mz.z + w * m.mz.w;
		this->w = x * m.mw.x + y * m.mw.y + z * m.mw.z + w * m.mw.w;

		return *this;
	}

	Vert3D& operator ^(const Mat3D& m) {
		EFTYPE x = this->x, y = this->y, z = this->z;
		this->x = x * m.mx.x + y * m.mx.y + z * m.mx.z;
		this->y = x * m.my.x + y * m.my.y + z * m.my.z;
		this->z = x * m.mz.x + y * m.mz.y + z * m.mz.z;;

		return *this;
	}

	Vert3D& operator *(const Vert3D& v) {
		EFTYPE x = this->x, y = this->y, z = this->z, w = this->w;
		this->x = y * v.z - z * v.y;
		this->y = z * v.x - x * v.z;
		this->z = x * v.y - y * v.x;

		return *this;
	}

	Vert3D& normalize() {
		EFTYPE x = this->x, y = this->y, z = this->z, w = this->w;
		//EFTYPE d = sqrt(x * x + y * y + z * z);
		//this->x /= d;
		//this->y /= d;
		//this->z /= d;
		EFTYPE d = Q_rsqrt(x * x + y * y + z * z);
		this->x *= d;
		this->y *= d;
		this->z *= d;

		return *this;
	}

	FLOAT operator &(const Vert3D& v) const{
		EFTYPE x = this->x, y = this->y, z = this->z, w = this->w;
		//EFTYPE ra = sqrt(x * x + y * y + z * z);
		//EFTYPE rb = sqrt(v.x * v.x + v.y * v.y + v.z * v.z);

		//return (x * v.x + y * v.y + z * v.z) / (ra * rb);
		EFTYPE ra = Q_rsqrt(x * x + y * y + z * z);
		EFTYPE rb = Q_rsqrt(v.x * v.x + v.y * v.y + v.z * v.z);

		return (x * v.x + y * v.y + z * v.z) * (ra * rb);
	}


	FLOAT operator ^(const Vert3D& v) const{
		EFTYPE x = this->x, y = this->y, z = this->z, w = this->w;

		return (x * v.x + y * v.y + z * v.z);
	}

	Vert3D& operator -(const Vert3D& v) {
		this->x -= v.x;
		this->y -= v.y;
		this->z -= v.z;
		this->w -= v.w;

		return *this;
	}

	Vert3D& operator +(const Vert3D& v) {
		this->x += v.x;
		this->y += v.y;
		this->z += v.z;
		this->w += v.w;

		return *this;
	}

	Vert3D& operator *(EFTYPE s) {
		this->x *= s;
		this->y *= s;
		this->z *= s;
		this->w *= s;

		return *this;
	}

	Vert3D& negative() {
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
		this->w = -this->w;

		return *this;
	}
	static bool CrossLine(EFTYPE left, EFTYPE right, EFTYPE y, EFTYPE top, EFTYPE bottom, EFTYPE x)
	{
		//判断一根横线和一根竖线是否交叉
		//横线有三个参数：left, right和y
		//竖线有三个参数：top, bottom和x
		return (top < y) && (bottom > y) && (left < x) && (right > x);
	}
	static int CrossRect(EFTYPE r1x1, EFTYPE r1x2, EFTYPE r1y1, EFTYPE r1y2,
						 EFTYPE r2x1, EFTYPE r2x2, EFTYPE r2y1, EFTYPE r2y2)
	{
		//判断两个矩形是否相交，共8种，横竖交叉
		//从一个矩形中取出一条横线，与另一矩形中的一条竖线判断是否交叉
		return CrossLine(r1x1, r1x2, r1y1, r2y1, r2y2, r2x1)
			|| CrossLine(r1x1, r1x2, r1y1, r2y1, r2y2, r2x2)
			|| CrossLine(r1x1, r1x2, r1y2, r2y1, r2y2, r2x1)
			|| CrossLine(r1x1, r1x2, r1y2, r2y1, r2y2, r2x2)
			|| CrossLine(r2x1, r2x2, r2y1, r1y1, r1y2, r1x1)
			|| CrossLine(r2x1, r2x2, r2y1, r1y1, r1y2, r1x2)
			|| CrossLine(r2x1, r2x2, r2y2, r1y1, r1y2, r1x1)
			|| CrossLine(r2x1, r2x2, r2y2, r1y1, r1y2, r1x2);
	}

	static INT get_minx(const Vert3D& p1, const Vert3D& p2, const Vert3D& p3) {
		return (INT)(p1.x > p2.x ? (p2.x > p3.x ? p3.x : p2.x) : (p1.x > p3.x ? p3.x : p1.x));
	}
	static INT get_maxx(const Vert3D& p1, const Vert3D& p2, const Vert3D& p3) {
		return (INT)(p1.x < p2.x ? (p2.x < p3.x ? p3.x : p2.x) : (p1.x < p3.x ? p3.x : p1.x));
	}
	static INT get_miny(const Vert3D& p1, const Vert3D& p2, const Vert3D& p3) {
		return (INT)(p1.y > p2.y ? (p2.y > p3.y ? p3.y : p2.y) : (p1.y > p3.y ? p3.y : p1.y));
	}
	static INT get_maxy(const Vert3D& p1, const Vert3D& p2, const Vert3D& p3) {
		return (INT)(p1.y < p2.y ? (p2.y < p3.y ? p3.y : p2.y) : (p1.y < p3.y ? p3.y : p1.y));
	}
	static EFTYPE getZ(const Vert3D& n, EFTYPE x0, EFTYPE y0, EFTYPE z0, EFTYPE x, EFTYPE y) {
		return (-n.x * (x - x0) - n.y * (y - y0)) / n.z + z0;
	}
	static EFTYPE getXFromY(const Vert3D& v0, const Vert3D& v1, EFTYPE y) {
		return ((y - v0.y) / (v1.y - v0.y)) * (v1.x - v0.x) + v0.x;
	}

	static EFTYPE cross(const Vert3D& a, const Vert3D& b, const Vert3D& p) {
		return (b.x - a.x)*(p.y - a.y) - (b.y - a.y)*(p.x - a.x);
	}

	static int IsInTriangle(const Vert3D& a, const Vert3D& b, const Vert3D& c, const Vert3D& p) {
		EFTYPE res_ab = cross(a, b, p);
		if (EP_ISZERO(res_ab)) { // in line segment ab
			return 1;
		}
		int res_bc = cross(b, c, p);
		if (EP_ISZERO(res_bc)) { // in line segment bc
			return 2;
		}
		if ((res_ab > 0) != (res_bc > 0)) {
			return 0;
		}
		int res_ca = cross(c, a, p);
		if (EP_ISZERO(res_ca)) { // in line segment ca
			return 3;
		}
		if ((res_ab > 0) != (res_ca > 0)) {
			return 0;
		}
		if (EP_ISZERO(cross(a, b, c))) { // a b c in one line
			return 0;
		}
		return -1;
	}
};

#endif