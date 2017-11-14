// Matrix3D.h
//

#ifndef _MATRIX_3D_H_
#define _MATRIX_3D_H_

#include "Vert3d.h"


typedef class Matrix3D Matrix3D;
class Matrix3D {
public:
	Matrix3D(Mat3D * M, Mat3D * M_1, int m) : M(M), M_1(M_1), mode(m) {

	}
	~Matrix3D() {

	}
	
	// mode: 1, scale
	// mode: 0, no scale
	int mode;

	Mat3D *M;
	Mat3D *M_1;

	Mat3D Mm;
	Mat3D Ms;
	Mat3D Mrx;
	Mat3D Mry;
	Mat3D Mrz;
	Mat3D Mm_1;
	Mat3D Ms_1;
	Mat3D Mrx_1;
	Mat3D Mry_1;
	Mat3D Mrz_1;
	Mat3D mm;

	void scale(EFTYPE sx, EFTYPE sy, EFTYPE sz) {
		this->Ms.mx.x *= sx;
		this->Ms.my.y *= sy;
		this->Ms.mz.z *= sz;

		refresh();
		if (M_1) {
			this->Ms_1.mx.x /= sx;
			this->Ms_1.my.y /= sy;
			this->Ms_1.mz.z /= sz;

			refresh_1();
		}
	}

	void move(EFTYPE dx, EFTYPE dy, EFTYPE dz) {
		this->Mm.mx.w += dx;
		this->Mm.my.w += dy;
		this->Mm.mz.w += dz;

		/*
		this->mm.setI();
		this->mm.mx.w = dx;
		this->mm.my.w = dy;
		this->mm.mz.w = dz;

		this->M * this->mm;
		*/

		refresh();

		if (M_1) {
			/*
			this->Mm_1.mx.w -= dx;
			this->Mm_1.my.w -= dy;
			this->Mm_1.mz.w -= dz;

			this->mm.set(this->M_1);
			this->M_1.mx.w = -dx;
			this->M_1.my.w = -dy;
			this->M_1.mz.w = -dz;

			this->M_1 * this->mm;
			*/

			/*
			this->mm.mx.w = -dx;
			this->mm.my.w = -dy;
			this->mm.mz.w = -dz;

			this->M_1 * this->mm;
			*/
			this->Mm_1.mx.w -= dx;
			this->Mm_1.my.w -= dy;
			this->Mm_1.mz.w -= dz;

			refresh_1();
		}
	}

	void rotate(EFTYPE ax, EFTYPE ay, EFTYPE az) {

		/*
		if (!EP_ISZERO(ax)) {
		ax = ax * PI / 180.0;
		EFTYPE sina = sin(ax);
		EFTYPE cosa = cos(ax);

		this->mm.setI();
		this->mm.my.set(0, cosa, -sina);
		this->mm.mz.set(0, sina, cosa);
		this->Mr * this->mm;
		}
		if (!EP_ISZERO(ay)) {
		ay = ay * PI / 180.0;
		EFTYPE sina = sin(ay);
		EFTYPE cosa = cos(ay);

		this->mm.setI();
		this->mm.mx.set(cosa, 0, sina);
		this->mm.mz.set(-sina, 0, cosa);
		this->Mr * this->mm;
		}
		if (!EP_ISZERO(az)) {
		az = az * PI / 180.0;
		EFTYPE sina = sin(az);
		EFTYPE cosa = cos(az);;

		this->mm.setI();
		this->mm.mx.set(cosa, sina, 0);
		this->mm.my.set(sina, cosa, 0);
		this->Mr * this->mm;
		}

		this->M.set(this->Mr) * this->Mm;
		*/

		if (!EP_ISZERO(ax)) {
			ax = ax * PI / 180.0;
			EFTYPE sinq = Mrx.mz.y;
			EFTYPE cosq = Mrx.my.y;
			EFTYPE sina = sin(ax);
			EFTYPE cosa = cos(ax);
			Mrx.my.y = cosq * cosa - sinq * sina;
			Mrx.my.z = -(sinq * cosa + cosq * sina);
			Mrx.mz.y = sinq * cosa + cosq * sina;
			Mrx.mz.z = cosq * cosa - sinq * sina;

			if (M_1) {

				sinq = Mrx_1.mz.y;
				cosq = Mrx_1.my.y;
				Mrx_1.my.y = cosq * cosa + sinq * sina;
				Mrx_1.my.z = -(sinq * cosa - cosq * sina);
				Mrx_1.mz.y = sinq * cosa - cosq * sina;
				Mrx_1.mz.z = cosq * cosa + sinq * sina;
			}
		}
		if (!EP_ISZERO(ay)) {
			ay = ay * PI / 180.0;
			EFTYPE sinq = Mry.mx.z;
			EFTYPE cosq = Mry.mx.x;
			EFTYPE sina = sin(ay);
			EFTYPE cosa = cos(ay);
			Mry.mx.x = cosq * cosa - sinq * sina;
			Mry.mx.z = sinq * cosa + cosq * sina;
			Mry.mz.x = -(sinq * cosa + cosq * sina);
			Mry.mz.z = cosq * cosa - sinq * sina;

			if (M_1) {

				sinq = Mry_1.mx.z;
				cosq = Mry_1.mx.x;
				Mry_1.mx.x = cosq * cosa + sinq * sina;
				Mry_1.mx.z = sinq * cosa - cosq * sina;
				Mry_1.mz.x = -(sinq * cosa - cosq * sina);
				Mry_1.mz.z = cosq * cosa + sinq * sina;
			}
		}
		if (!EP_ISZERO(az)) {
			az = az * PI / 180.0;
			EFTYPE sinq = Mrz.my.x;
			EFTYPE cosq = Mrz.mx.x;
			EFTYPE sina = sin(az);
			EFTYPE cosa = cos(az);
			Mrz.mx.x = cosq * cosa - sinq * sina;
			Mrz.mx.y = -(sinq * cosa + cosq * sina);
			Mrz.my.x = sinq * cosa + cosq * sina;
			Mrz.my.y = cosq * cosa - sinq * sina;

			if (M_1) {

				sinq = Mrz_1.my.x;
				cosq = Mrz_1.mx.x;
				Mrz_1.mx.x = cosq * cosa + sinq * sina;
				Mrz_1.mx.y = -(sinq * cosa - cosq * sina);
				Mrz_1.my.x = sinq * cosa - cosq * sina;
				Mrz_1.my.y = cosq * cosa + sinq * sina;
			}
		}


		refresh();
		if (M_1) {
			refresh_1();
		}
	}


	void refresh() {
		if (mode) {
			(*M).set(Ms) * Mrx * Mry * Mrz * Mm;
		}
		else {
			(*M).set(Mrx) * Mry * Mrz * Mm;
		}
	}
	void refresh_1() {
		if (mode) {
			(*M_1).set(Mm_1) * Mrz_1 * Mry_1 * Mrx_1 * Ms_1;
		}
		else {
			(*M_1).set(Mm_1) * Mrz_1 * Mry_1 * Mrx_1;
		}
	}
};

#endif