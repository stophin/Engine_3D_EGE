// Light3D.h
//

#ifndef _LIGHT3D_H_
#define _LIGHT3D_H_

#include "Object3D.h"

typedef class Light3D Light3D;
class Light3D {
public:
	Light3D() :
		_M(&M, &M_1, 1),
		cam(NULL) , mode(1) {
		pos.init();
		pos_r.init();

	}
	~Light3D() {

	}

	DWORD color;
	Vert3D pos;
	Vert3D pos_r;

	int mode;

	Camera3D * cam;

	void render_normalize() {
		if (this->cam == NULL) {
			return;
		}
		pos_r.set(pos);
		pos_r * M * this->cam->M;
	}

	Matrix3D _M;
	Mat3D M;
	Mat3D M_1;
	Light3D& move(EFTYPE dx, EFTYPE dy, EFTYPE dz) {
		this->_M.move(dx, dy, dz);

		this->render_normalize();

		return *this;
	}
	Light3D& rotate(EFTYPE dx, EFTYPE dy, EFTYPE dz) {
		this->_M.rotate(dx, dy, dz);

		this->render_normalize();

		return *this;
	}

	Vert3D n1;
	Vert3D r;
	EFTYPE getFactor(const Vert3D& n, const Vert3D& n0) {
		// amibent :  Ka * GlobalAmibent
		EFTYPE f = 0, _f = 0, __f = 0;
		// diffuse :  Kd * LightColor * dot(L, N)
		n1.set(this->pos_r);
		n1 - n0;
		_f = (n & n1);
		if (this->mode > 0) {
			// specular : Ks * LightColor * (dot(V, R))^ shininess
			// reflect R: 2 * (dot(L, N))N - L
			r.set(n);
			r * (2 * _f) + n1;
			__f = (r.negative() & n0);
			__f *= __f;
			//__f = pow(__f, (FLOAT)5);
		}
		//if (_f < 0) _f = -_f;
		f += _f + __f;

		return f;
	}


	static DWORD add(DWORD rgba, DWORD rgba_r, EFTYPE s) {
		BYTE r = EGEGET_R(rgba), g = EGEGET_G(rgba), b = EGEGET_B(rgba), a = EGEGET_A(rgba);
		BYTE r_r = EGEGET_R(rgba_r), g_r = EGEGET_G(rgba_r), b_r = EGEGET_B(rgba_r), a_r = EGEGET_A(rgba_r);

		s /= 2;
		r = max(min(r * s + r_r * (1 - s), 255), 0);
		g = max(min(g * s + g_r * (1 - s), 255), 0);
		b = max(min(b * s + b_r * (1 - s), 255), 0);
		a = max(min(a * s + a_r * (1 - s), 255), 0);

		return EGERGBA(r, g, b, a);

	}
	static DWORD multi(DWORD rgba, EFTYPE s) {
		BYTE r = EGEGET_R(rgba), g = EGEGET_G(rgba), b = EGEGET_B(rgba), a = EGEGET_A(rgba);

		r = max(min(r * s, 255), 0);
		g = max(min(g * s, 255), 0);
		b = max(min(b * s, 255), 0);
		a = max(min(a * s, 255), 0);

		return EGERGBA(r, g, b, a);

	}

};
#endif