// Camera3D.h
// Camera for 3D
//

#ifndef _CAMERA3D_H_
#define _CAMERA3D_H_

#include "Matrix3D.h"

typedef class Camera3D Camera3D;
class Camera3D {
public:
	Camera3D(EFTYPE width, EFTYPE height, EFTYPE znear, EFTYPE zfar, EFTYPE angle_width, EFTYPE angle_height) :
		_M(&M, &M_1, 0),
	width(width), height(height), znear(znear), zfar(zfar){
		lookat.set(0, 0, -1, 1);

		lb.set(-width / 2, -height / 2, 0, 1);
		rt.set(width / 2, height / 2, 0, 1);

		initialize(2);

		tanwidth = tan(angle_width * PI / 180.0);
		tanheight = tan(angle_height * PI / 180.0);
	}

	EFTYPE znear;
	EFTYPE zfar;
	EFTYPE width;
	EFTYPE height;

	EFTYPE tanwidth;
	EFTYPE tanheight;

	Vert3D lb;
	Vert3D rt;
	Vert3D lookat;

	//type:
	//1: Orthographic
	//2: Oblique
	INT type;
	void initialize(int type) {
		this->type = type;

		if (type == 1) {
			//TODO
			//use oblique
			proj.mx.set(2 * znear / width, 0, -(rt.x + lb.x) / width, 0);
			proj.my.set(0, 2 * znear / height, -(rt.y + lb.y) / height, 0);
			proj.mz.set(0, 0, zfar / (zfar - znear), -zfar * znear / (zfar - znear));
			proj.mw.set(0, 0, 1, 0);

			proj_1.mx.set(1 / (2 * znear / width), 0, -(rt.x + lb.x) / width, 0);
			proj_1.my.set(0, 1 / (2 * znear / height), -(rt.y + lb.y) / height, 0);
			proj_1.mz.set(0, 0, 1 / (zfar / (zfar - znear)), (-zfar * znear / (zfar - znear)) / (zfar / (zfar - znear)));
			proj_1.mw.set(0, 0, 0, 1);
		}
		else if (type == 2) {
			proj.mx.set(2 * znear / width, 0, -(rt.x + lb.x) / width, 0);
			proj.my.set(0, 2 * znear / height, -(rt.y + lb.y) / height, 0);
			proj.mz.set(0, 0, (zfar + znear) / (zfar - znear), -2 * zfar * znear / (zfar - znear));
			proj.mw.set(0, 0, 1, 0);

			proj_1.mx.set(1 / (2 * znear / width), 0, -(rt.x + lb.x) / width, 0);
			proj_1.my.set(0, 1 / (2 * znear / height), -(rt.y + lb.y) / height, 0);
			proj_1.mz.set(0, 0, 1 / ((zfar + znear) / (zfar - znear)), -(-2 * zfar * znear / (zfar - znear)) / ((zfar + znear) / (zfar - znear)));
			proj_1.mw.set(0, 0, 0, 1);
		}

	}

	int normalize(Vert3D& v) {
		if (this->lookat.z > 0 && (EP_GTZERO(v.z - zfar) || EP_GTZERO(v.z - znear))) {
			return -2;
		}
		//if (this->lookat.z > 0) {
		//	if (EP_GTZERO(v.z - zfar)) {
		//		v.z = zfar;
		//	}
		//	else if (EP_GTZERO(v.z - znear)) {
		//		v.z = znear;
		//	}
		//	//return -2;
		//}
		else if (this->lookat.z < 0 && (EP_GTZERO(v.z + znear) || EP_LTZERO(v.z + zfar))) {
			return -3;
		}
		//else if (this->lookat.z < 0) {
		//	if (EP_GTZERO(v.z + znear)) {
		//		v.z = -znear;
		//	}
		//	else if (EP_LTZERO(v.z + zfar)) {
		//		v.z = -zfar;
		//	}
		//	//return -3;
		//}
		EFTYPE w = tanwidth * v.z, h = tanheight * v.z;
		if (w < 0) w = -w;
		if (h < 0) h = -h;
		if (v.x > w || v.x < -w) {
			return -4;
		}
		else if (v.y > h || v.y < -h) {
			return -5;
		}
		//if (v.x > w) {
		//	v.x = w;
		//}
		//else if (v.x < -w) {
		//	v.x = -w;
		//}
		//else if (v.y > h) {
		//	v.y = h;
		//}
		//else if (v.y < -h) {
		//	v.y = -h;
		//}
		if (EP_ISZERO(v.z)) {
			return -1;
		}
		else if (!EP_ISZERO(v.w - EP_MAX)){
			project(v);
		}
		return 0;
	}

	Mat3D proj;
	void project(Vert3D &v) {
		EFTYPE z = 1.0 / v.z;
		v * proj;
		v * z;
	}

	Mat3D proj_1;

	int anti_normalize(Vert3D& v, EFTYPE z) {

		v * z;

		v.w = 1;

		v * proj_1;

		v.w = 1;

		return 0;
	}

	union {
		Vert3D g;
		struct {
			EFTYPE offset_w;
			EFTYPE offset_h;
			EFTYPE scale_w;
			EFTYPE scale_h;
		};
	};

	void setRange(EFTYPE o_w, EFTYPE o_h, EFTYPE s_w, EFTYPE s_h) {
		this->g.set(o_w, o_h, s_w, s_h);
	}

	Matrix3D _M;
	Mat3D M;
	Mat3D M_1;
	Camera3D& move(EFTYPE dx, EFTYPE dy, EFTYPE dz) {

		this->_M.move(dx, dy, dz);

		return *this;
	}

	Camera3D& rotate(EFTYPE ax, EFTYPE ay, EFTYPE az) {

		this->_M.rotate(ax, ay, az);

		return *this;
	}
};


typedef class Cam3D Cam3D;
class Cam3D : public Camera3D {
public:
	Cam3D() :
		Camera3D(50, 50, 50, 1000, 90, 90), type(0) {
		initialize();
	}
	Cam3D(EFTYPE width, EFTYPE height, EFTYPE znear, EFTYPE zfar, EFTYPE angle_width, EFTYPE angle_height) :
		Camera3D(width, height, znear, zfar, angle_width, angle_height), type(0) {
		initialize();
	}
	void initialize() {
		for (INT i = 0; i < 1; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
	}
	// camera type
	// 0 : normal camera
	// 1 : shadow camera
	// 2 : reflection camera
	int type;

#define MAX_CAM3D_LINK	1
	INT uniqueID;
	Cam3D * prev[MAX_CAM3D_LINK];
	Cam3D * next[MAX_CAM3D_LINK];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < MAX_CAM3D_LINK; i++)
		{
			if (((Cam3D*)_ptr)->prev[i] != NULL || ((Cam3D*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
	}
};


#endif