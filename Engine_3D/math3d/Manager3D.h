// Manager3D.h
// Manager for Camera and Object
//

#ifndef _MANAGER3D_H_
#define _MANAGER3D_H_

#include "Object3D.h"
#include "Light3D.h"

#include "../scene/OctTree.h"

typedef class Manager3D Manager3D;
class Manager3D {
public:
	Manager3D() : cams(0), objs(0), lgts(0), tras(0), refl(0), grps(0), octs(1),
		group(NULL){

	}
	~Manager3D() {
		this->cams.~MultiLinkList();
		//NOTE: octs links contains parts of objs and tras and refl, 
		//so we muse deconstruct it first
		this->octs.~MultiLinkList();
		this->objs.~MultiLinkList();
		this->tras.~MultiLinkList();
		this->refl.~MultiLinkList();
		this->grps.~MultiLinkList();
	}

	MultiLinkList<Cam3D> cams;
	MultiLinkList<Obj3D> objs;
	MultiLinkList<Obj3D> tras;
	MultiLinkList<Obj3D> refl;
	MultiLinkList<Lgt3D> lgts;
	MultiLinkList<Group3D> grps;
	//oct-tree
	MultiLinkList<Obj3D> octs;

	Rect3D rect;
	OctTree octTree;
	void createOctTree() {
		//get scnene aabb
		Obj3D* _obj = this->objs.link;
		INT render_state = 0;
		this->rect.set(EP_MAX, EP_MAX, EP_MAX, 0, 0, 0);
		if (_obj) {
			do {
				Vert3D &v0 = _obj->leftTopBack;
				Vert3D &v1 = _obj->rightBottomFront;
				if (this->rect.x > v0.x) this->rect.x = v0.x;
				if (this->rect.y > v0.y) this->rect.y = v0.y;
				if (this->rect.z > v0.z) this->rect.z = v0.z;
				if (this->rect.width < v1.x) this->rect.width = v1.x;
				if (this->rect.height < v1.y) this->rect.height = v1.y;
				if (this->rect.depth < v1.z) this->rect.depth = v1.z;

				//first do objects till end
				//then do reflection and then transparent object
				if (render_state == 0) {
					_obj = this->objs.next(_obj);
					if (!(_obj && _obj != this->objs.link)) {
						_obj = this->refl.link;
						render_state = 1;
						if (!_obj) {
							//or render reflection points
							_obj = this->tras.link;
							render_state = 2;
						}
					}
				}
				else if (render_state == 1) {
					_obj = this->refl.next(_obj);
					if (!(_obj && _obj != this->refl.link)) {
						_obj = this->tras.link;
						render_state = 2;
					}
				}
				else {
					_obj = this->tras.next(_obj);
					if (!(_obj && _obj != this->tras.link)) {
						break;
					}
				}
			} while (_obj);
		}
		this->rect.width = this->rect.width - this->rect.x;
		this->rect.height = this->rect.height - this->rect.y;
		this->rect.depth = this->rect.depth - this->rect.z;
		//create oct-tree
		this->octTree.bounds.set(this->rect);
		_obj = this->objs.link;
		render_state = 0;
		if (_obj) {
			do {
				this->octTree.Insert(_obj);

				//first do objects till end
				//then do reflection and then transparent object
				if (render_state == 0) {
					_obj = this->objs.next(_obj);
					if (!(_obj && _obj != this->objs.link)) {
						_obj = this->refl.link;
						render_state = 1;
						if (!_obj) {
							//or render reflection points
							_obj = this->tras.link;
							render_state = 2;
						}
					}
				}
				else if (render_state == 1) {
					_obj = this->refl.next(_obj);
					if (!(_obj && _obj != this->refl.link)) {
						_obj = this->tras.link;
						render_state = 2;
					}
				}
				else {
					_obj = this->tras.next(_obj);
					if (!(_obj && _obj != this->tras.link)) {
						break;
					}
				}
			} while (_obj);
		}
	}

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
	Camera3D& addShadowCamera() {
		Cam3D * cam = new Cam3D();
		cam->type = 1;
		this->cams.insertLink(cam);

		return *((Camera3D*)cam);
	}
	Camera3D& addReflectionCamera() {
		Cam3D * cam = new Cam3D();
		cam->type = 2;
		this->cams.insertLink(cam);

		return *((Camera3D*)cam);
	}

	Camera3D * getCamera(int type) {
		Cam3D * cam = this->cams.link;
		if (cam) {
			do {
				if (cam->type == type) {
					break;
				}

				cam = this->cams.next(cam);
			} while (cam && cam != this->cams.link);
		}
		if (cam->type != type) {
			return NULL;
		}
		return (Camera3D*)cam;
	}

	Group3D& addGroup() {
		Group3D * gp = new Group3D();

		this->grps.insertLink(gp);

		return *gp;
	}

	Group3D * group;
	Manager3D& startGroup(INT uniqueID) {
		this->group = this->grps.getLink(uniqueID);

		return *this;
	}

	Manager3D& endGroup() {
		this->group = NULL;

		return *this;
	}

	Object3D& addObject(INT anti = 1) {
		Obj3D * obj = new Obj3D();

		obj->cam = this->cams.link;

		this->objs.insertLink(obj);
		if (this->group) {
			this->group->objs.insertLink(obj);
		}

		((Object3D *)obj)->anti = anti;
		//TODO
		((Object3D *)obj)->render_aabb = 1;

		return *((Object3D *)obj);
	}

	Object3D& addReflectionObject(EFTYPE t){
		Obj3D * obj = new Obj3D();

		obj->cam = this->cams.link;

		obj->reflection = t;

		this->refl.insertLink(obj);
		if (this->group) {
			this->group->objs.insertLink(obj);
		}
		((Object3D *)obj)->anti = 1;
		//TODO
		((Object3D *)obj)->render_aabb = 1;

		return *((Object3D *)obj);

	}


	Object3D& addTransparentObject(EFTYPE t){
		Obj3D * obj = new Obj3D();

		obj->cam = this->cams.link;

		obj->transparent = t;

		this->tras.insertLink(obj);
		if (this->group) {
			this->group->objs.insertLink(obj);
		}
		((Object3D *)obj)->anti = 1;
		//TODO
		((Object3D *)obj)->render_aabb = 1;

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

			this->refresh(NULL);
		}
	}

	void rotateCamera(EFTYPE ax, EFTYPE ay, EFTYPE az) {
		if (this->cams.link) {
			this->cams.link->rotate(ax, ay, az);

			this->refresh(NULL);
		}
	}

	void nextCamera() {

		do {
			if (this->cams.link) {
				Cam3D * cam = this->cams.removeLink(this->cams.link);
				this->cams.insertLink(cam);
			}
		} while (this->cams.link->type != 0);

		this->refresh(NULL);
	}

	void refresh(Camera3D * cam) {
		Obj3D * obj = this->objs.link;
		if (obj) {
			do {
				if (cam) {
					obj->cam = cam;
				}
				else {
					obj->cam = this->cams.link;
				}
				obj->render_normalize(2);

				obj = this->objs.next(obj);
			} while (obj && obj != this->objs.link);
		}
		obj = this->tras.link;
		if (obj) {
			do {
				if (cam) {
					obj->cam = cam;
				}
				else {
					obj->cam = this->cams.link;
				}
				obj->render_normalize(2);

				obj = this->tras.next(obj);
			} while (obj && obj != this->tras.link);
		}
		obj = this->refl.link;
		if (obj) {
			do {
				if (cam) {
					obj->cam = cam;
				}
				else {
					obj->cam = this->cams.link;
				}
				obj->render_normalize(2);

				obj = this->refl.next(obj);
			} while (obj && obj != this->refl.link);
		}
		Lgt3D * lgt = this->lgts.link;
		if (lgt) {
			do {
				if (cam) {
					lgt->cam = cam;
				}
				else {
					lgt->cam = this->cams.link;
				}
				lgt->render_normalize(2);

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