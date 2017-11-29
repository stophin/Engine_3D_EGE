// Ray.h
//

#ifndef _RAY_H_
#define _RAY_H_

#include "../math3d/Vert3D.h"

class Verts{
public:
	Verts() {

	}
	~Verts() {

	}

	EFTYPE trans;
	DWORD color;
	Vert3D v;
	Vert3D v_n;

	// for multilinklist
#define MAX_VERTS_LINK	2
	INT uniqueID;
	Verts * prev[MAX_VERTS_LINK];
	Verts * next[MAX_VERTS_LINK];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < MAX_VERTS_LINK; i++)
		{
			if (((Verts*)_ptr)->prev[i] != NULL || ((Verts*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
	}
};

class Ray {
public:
	Ray(){
	}
	Ray(const Vert3D& o, const Vert3D& d){
		this->set(o, d);
	}
	~Ray() {
	}
	
	void set(const Vert3D& o, const Vert3D& d){
		original.set(o);
		direction.set(d);
	}

	Vert3D original;
	Vert3D direction;
	Vert3D point;

	//r(t) = o + t * d, t >= 0
	Vert3D& getPoint(float t) {
		point.set(direction);
		point * t;
		point + original;

		return point;
	}
};

#endif