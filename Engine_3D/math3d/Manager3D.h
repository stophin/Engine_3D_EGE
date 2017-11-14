// Manager3D.h
// Manager for Camera and Object
//

#ifndef _MANAGER3D_H_
#define _MANAGER3D_H_

#include "Object3D.h"
#include "Light3D.h"

typedef class Cam3D Cam3D;
class Cam3D : public Camera3D {
public:
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
	int type;

	INT uniqueID;
	Cam3D * prev[1];
	Cam3D * next[1];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < 1; i++)
		{
			if (((Cam3D*)_ptr)->prev[i] != NULL || ((Cam3D*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
	}
};

typedef class Obj3D Obj3D;
class Obj3D : public Object3D {
public:
	Obj3D() : Object3D() {
		initialize();
	}
	void initialize() {
		for (INT i = 0; i < 1; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
	}

	INT uniqueID;
	Obj3D * prev[1];
	Obj3D * next[1];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < 1; i++)
		{
			if (((Obj3D*)_ptr)->prev[i] != NULL || ((Obj3D*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
	}
};

typedef class Lgt3D Lgt3D;
class Lgt3D : public Light3D {
public:
	Lgt3D() : Light3D() {
		initialize();
	}
	void initialize() {
		for (INT i = 0; i < 1; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
	}

	INT uniqueID;
	Lgt3D * prev[1];
	Lgt3D * next[1];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < 1; i++)
		{
			if (((Lgt3D*)_ptr)->prev[i] != NULL || ((Lgt3D*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
	}
};


typedef class Manager3D Manager3D;
class Manager3D {
public:
	Manager3D() : cams(0), objs(0), lgts(0), tras(0), refl(0){

	}
	~Manager3D() {
		this->cams.~MultiLinkList();
		this->objs.~MultiLinkList();
		this->tras.~MultiLinkList();
		this->refl.~MultiLinkList();
	}

	MultiLinkList<Cam3D> cams;
	MultiLinkList<Obj3D> objs;
	MultiLinkList<Obj3D> tras;
	MultiLinkList<Obj3D> refl;
	MultiLinkList<Lgt3D> lgts;

	void setCameraRange(EFTYPE o_w, EFTYPE o_h, EFTYPE s_w, EFTYPE s_h) {
		Cam3D * cam = this->cams.link;
		if (cam){
			do {
				cam->setRange(o_w, o_h, s_w, s_h);

				cam = this->cams.next(cam);
			} while (cam && cam != this->cams.link);
		}

		this->refresh(0);
	}

	Camera3D& addCamera(EFTYPE width, EFTYPE height, EFTYPE znear, EFTYPE zfar, EFTYPE angle_width, EFTYPE angle_height) {
		Cam3D * cam = new Cam3D(width, height, znear, zfar, angle_width, angle_height);
		this->cams.insertLink(cam);

		return *((Camera3D*)cam);
	}
	Camera3D& addShadowCamera(EFTYPE width, EFTYPE height, EFTYPE znear, EFTYPE zfar, EFTYPE angle_width, EFTYPE angle_height) {
		Cam3D * cam = new Cam3D(width, height, znear, zfar, angle_width, angle_height);
		cam->type = 1;
		this->cams.insertLink(cam);

		return *((Camera3D*)cam);
	}

	Object3D& addObject() {
		Obj3D * obj = new Obj3D();

		obj->cam = this->cams.link;

		this->objs.insertLink(obj);

		return *((Object3D *)obj);
	}

	Object3D& addReflectionObject(EFTYPE t){
		Obj3D * obj = new Obj3D();

		obj->cam = this->cams.link;

		obj->reflection = t;

		this->refl.insertLink(obj);

		return *((Object3D *)obj);

	}


	Object3D& addTransparentObject(EFTYPE t){
		Obj3D * obj = new Obj3D();

		obj->cam = this->cams.link;

		obj->transparent = t;

		this->tras.insertLink(obj);

		return *((Object3D *)obj);

	}

	Light3D& addLight(EFTYPE x, EFTYPE y, EFTYPE z) {
		Lgt3D * lgt = new Lgt3D();

		lgt->cam = this->cams.link;

		lgt->move(x, y, z);

		this->lgts.insertLink(lgt);

		return *((Light3D*)lgt);
	}

	void moveCamera(EFTYPE x, EFTYPE y, EFTYPE z) {
		if (this->cams.link) {
			this->cams.link->move(x, y, z);

			this->refresh(0);
		}
	}

	void rotateCamera(EFTYPE ax, EFTYPE ay, EFTYPE az) {
		if (this->cams.link) {
			this->cams.link->rotate(ax, ay, az);

			this->refresh(0);
		}
	}

	void nextCamera() {

		if (this->cams.link) {
			Cam3D * cam = this->cams.removeLink(this->cams.link);
			this->cams.insertLink(cam);

			this->refresh(1);
		}
	}

	void refresh(int mode) {
		Obj3D * obj = this->objs.link;
		if (obj) {
			do {
				if (mode) {
					obj->cam = this->cams.link;
				}
				obj->render_normalize();

				obj = this->objs.next(obj);
			} while (obj && obj != this->objs.link);
		}
		obj = this->tras.link;
		if (obj) {
			do {
				if (mode) {
					obj->cam = this->cams.link;
				}
				obj->render_normalize();

				obj = this->tras.next(obj);
			} while (obj && obj != this->tras.link);
		}
		obj = this->refl.link;
		if (obj) {
			do {
				if (mode) {
					obj->cam = this->cams.link;
				}
				obj->render_normalize();

				obj = this->refl.next(obj);
			} while (obj && obj != this->refl.link);
		}
		Lgt3D * lgt = this->lgts.link;
		if (lgt) {
			do {
				if (mode) {
					lgt->cam = this->cams.link;
				}
				lgt->render_normalize();

				lgt = this->lgts.next(lgt);
			} while (lgt && lgt != this->lgts.link);
		}
	}

	void moveLight(EFTYPE x, EFTYPE y, EFTYPE z) {
		if (this->lgts.link) {
			this->lgts.link->move(x, y, z);
		}
	}
	void rotateLight(EFTYPE x, EFTYPE y, EFTYPE z) {
		if (this->lgts.link) {
			this->lgts.link->rotate(x, y, z);
		}
	}

	void nextLight() {
		if (this->lgts.link) {
			Lgt3D * lgt = this->lgts.removeLink(this->lgts.link);
			this->lgts.insertLink(lgt);
		}
	}
};

#endif