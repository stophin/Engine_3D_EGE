//OctTree.h
//Oct Treee 3D scene management

#ifndef _OCTTREE_H_
#define _OCTTREE_H_

#include "../math3d/Object3D.h"

class Rect3D {
public:
	Rect3D(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w, EFTYPE h, EFTYPE d) :
	x(x), y(y), z(z), width(w), height(h), depth(d){

	}
	Rect3D() :
	x(0), y(0), z(0), width(0), height(0), depth(0){
	}

	EFTYPE x;
	EFTYPE y;
	EFTYPE z;
	EFTYPE width;
	EFTYPE height;
	EFTYPE depth;

	void set(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w, EFTYPE h, EFTYPE d) {
		this->x = x;
		this->y = y;
		this->z = z;
		this->width = w;
		this->height = h;
		this->depth = d;
	}

	void set(const Rect3D& r) {
		this->x = r.x;
		this->y = r.y;
		this->z = r.z;
		this->width = r.width;
		this->height = r.height;
		this->depth = r.depth;
	}
};

enum OctTree_Link {
	RightTopFront = 0,
	LeftTopFront = 1,
	LeftBottomFront = 2,
	RightBottomFront = 3,
	RightTopBack = 4,
	LeftTopBack = 5,
	LeftBottomBack = 6,
	RightBottomBack = 7,
	OctTreeLinkMax = 8
};

#define MAX_QUARDANTS	8
#define MAX_OBJECTS		5
#define MAX_LEVELS		MAX_OBJ3D_LINK - 3
#define GET_LINK_INDEX(level)	(level + 3 - 1)
class OctTree {
public:
	OctTree(INT level,
		EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w, EFTYPE h, EFTYPE d,
		OctTree * parent, INT position) :
		level(level),
		position(position),
		bounds(x, y, z, w, h, d),
		maxObjects(MAX_OBJECTS),
		maxLevels(MAX_LEVELS),
		objects(GET_LINK_INDEX(level)),
		parent(parent),
		hasChild(0){
		for (int i = 0; i < OctTreeLinkMax; i++) {
			children[i] = NULL;
		}
	}
	OctTree() :
		level(0),
		position(-1),
		maxObjects(MAX_OBJECTS),
		maxLevels(MAX_LEVELS),
		objects(GET_LINK_INDEX(level)),
		parent(NULL),
		hasChild(0) {
	}
	~OctTree() {
		objects.~MultiLinkList();
	}
	void set(INT level,
		EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE w, EFTYPE h, EFTYPE d,
		OctTree * parent, INT position) {
		this->level = level;
		this->objects.linkindex = GET_LINK_INDEX(level);
		this->position = position;
		this->bounds.set(x, y, z, w, h, d);
		this->parent = parent;
	}

	INT level;
	INT position;
	Rect3D bounds;

	INT maxObjects;
	INT maxLevels;

	MultiLinkList<Obj3D> objects;

	OctTree * parent;
	INT hasChild;
	OctTree * children[OctTreeLinkMax];

	// 5 4
	// 6 7
	// 1 0
	// 2 3
	void Split() {
		EFTYPE subWidth = this->bounds.width / 2;
		EFTYPE subHeight = this->bounds.height / 2;
		EFTYPE subDepth = this->bounds.depth / 2;
		EFTYPE x = this->bounds.x;
		EFTYPE y = this->bounds.y;
		EFTYPE z = this->bounds.z;

		this->children[OctTree_Link::RightTopFront] =
			new OctTree(this->level + 1, x + subWidth, y, z, subWidth, subHeight, subDepth, this, OctTree_Link::RightTopFront);
		this->children[OctTree_Link::LeftTopFront] =
			new OctTree(this->level + 1, x, y, z, subWidth, subHeight, subDepth, this, OctTree_Link::LeftTopFront);
		this->children[OctTree_Link::LeftBottomFront] =
			new OctTree(this->level + 1, x, y + subHeight, z, subWidth, subHeight, subDepth, this, OctTree_Link::LeftBottomFront);
		this->children[OctTree_Link::RightBottomFront] =
			new OctTree(this->level + 1, x + subWidth, y + subHeight, z, subWidth, subHeight, subDepth, this, OctTree_Link::RightBottomFront);
		this->children[OctTree_Link::RightTopBack] =
			new OctTree(this->level + 1, x + subWidth, y, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::RightTopBack);
		this->children[OctTree_Link::LeftTopBack] =
			new OctTree(this->level + 1, x, y, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::LeftTopBack);
		this->children[OctTree_Link::LeftBottomBack] =
			new OctTree(this->level + 1, x, y + subHeight, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::LeftBottomBack);
		this->children[OctTree_Link::RightBottomBack] =
			new OctTree(this->level + 1, x + subWidth, y + subHeight, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::RightBottomBack);
		this->hasChild = true;
	}

	INT GetIndex(Rect3D& rect) {
		INT index = -1;
		EFTYPE xMidPoint = this->bounds.x + this->bounds.width / 2;
		EFTYPE yMidPoint = this->bounds.y + this->bounds.height / 2;
		EFTYPE zMidPoint = this->bounds.z + this->bounds.depth / 2;

		EFTYPE topQuad = (rect.y > this->bounds.y && rect.y + rect.height < yMidPoint);
		EFTYPE bottomQuad = (rect.y > yMidPoint && rect.y + rect.height < this->bounds.y + this->bounds.height);
		EFTYPE frontQuad = (rect.z > this->bounds.z && rect.z + rect.depth < zMidPoint);
		EFTYPE backQuad = (rect.z > zMidPoint && rect.z + rect.depth < this->bounds.z + this->bounds.depth);

		if (rect.x > this->bounds.x && rect.x + rect.width < xMidPoint) {//leftQuad
			if (topQuad) {
				if (frontQuad) {
					index = 1;//leftTopFront
				}
				else if (backQuad) {
					index = 5;//leftTopBack
				}
			}
			else if (bottomQuad) {
				if (frontQuad) {
					index = 2;//leftBottomFront
				}
				else if (backQuad) {
					index = 6;//leftBottomBack
				}
			}
		}
		else if (rect.x > xMidPoint && rect.x + rect.width < this->bounds.x + this->bounds.width) {//rightQuad
			if (topQuad) {
				if (frontQuad) {
					index = 0;//rightTopFront
				}
				else if (backQuad) {
					index = 4;//rightTopBack
				}
			}
			else if (bottomQuad) {
				if (frontQuad) {
					index = 3;//rightBottmFront
				}
				else if (backQuad) {
					index = 7;//rightBottomBack
				}
			}
		}
		return index;
	}

	Rect3D rect;
	void Insert(Obj3D * obj) {
		if (NULL == obj) {
			return;
		}
		if (this->hasChild) {
			rect.set(obj->leftTopBack.x, obj->leftTopBack.y, obj->leftTopBack.z, 
				obj->rightBottomFront.x - obj->leftTopBack.x, obj->rightBottomFront.y - obj->leftTopBack.y, obj->rightBottomFront.z - obj->leftTopBack.z);
			INT index = this->GetIndex(rect);
			if (index != -1) {
				this->children[index]->Insert(obj);
			}
			else {
				this->objects.insertLink(obj);
				obj->octTree = this;
			}
			return;
		}
		this->objects.insertLink(obj);
		obj->octTree = this;
		if (this->objects.linkcount > this->maxObjects && this->level < this->maxLevels) {
			this->Split();
			Obj3D * _obj = this->objects.link, *_next;
			if (_obj) {
				do {
					_next = this->objects.next(_obj);

					rect.set(_obj->leftTopBack.x, _obj->leftTopBack.y, _obj->leftTopBack.z,
						_obj->rightBottomFront.x - _obj->leftTopBack.x, _obj->rightBottomFront.y - _obj->leftTopBack.y, _obj->rightBottomFront.z - _obj->leftTopBack.z);
					INT index = this->GetIndex(rect);
					if (index != -1) {
						this->objects.removeLink(_obj);
						this->children[index]->Insert(_obj);
					}

					_obj = _next;
				} while (_obj && _obj != this->objects.link);
			}
		}
	}

	void Collision(Obj3D * obj, MultiLinkList<Obj3D> * link) {
		if (NULL == obj) {
			return;
		}
		if (NULL == link) {
			return;
		}
		if (this->hasChild) {
			rect.set(obj->leftTopBack.x, obj->leftTopBack.y, obj->leftTopBack.z,
				obj->rightBottomFront.x - obj->leftTopBack.x, obj->rightBottomFront.y - obj->leftTopBack.y, obj->rightBottomFront.z - obj->leftTopBack.z);
			INT index = this->GetIndex(rect);
			if (index != -1) {
				this->children[index]->Collision(obj, link);
			}
		}
		if (NULL == this->objects.link) {
			return;
		}
		Obj3D * _obj = this->objects.link;
		if (_obj) {
			do {

				if (_obj != obj) {
					link->insertLink(_obj);
				}

				_obj = this->objects.next(_obj);
			} while (_obj && _obj != this->objects.link);
		}
	}

	Vert3D v[8];
	Vert3D v0, v1, v2;
	Vert3D n0, n1, n, p;
	void Collision(Vert3D& vo, Vert3D& vd, Camera3D * cam, MultiLinkList<Obj3D> * link) {
		if (NULL == link) {
			return;
		}
		if (NULL == cam) {
			return;
		}
		if (this->hasChild) {
			for (int i = 0; i < MAX_QUARDANTS; i++) {
				if (this->children[i]) {
					this->children[i]->Collision(vo, vd, cam, link);
				}
			}
		}
		if (NULL == this->objects.link) {
			return;
		}
		v[0].set(this->bounds.x, this->bounds.y, this->bounds.z);
		v[1].set(this->bounds.x, this->bounds.y + this->bounds.height, this->bounds.z);
		v[2].set(this->bounds.x + this->bounds.width, this->bounds.y + this->bounds.height, this->bounds.z);
		v[3].set(this->bounds.x + this->bounds.width, this->bounds.y, this->bounds.z);
		v[4].set(this->bounds.x, this->bounds.y, this->bounds.z + this->bounds.depth);
		v[5].set(this->bounds.x, this->bounds.y + this->bounds.height, this->bounds.z + this->bounds.depth);
		v[6].set(this->bounds.x + this->bounds.width, this->bounds.y + this->bounds.height, this->bounds.z + this->bounds.depth);
		v[7].set(this->bounds.x + this->bounds.width, this->bounds.y, this->bounds.z + this->bounds.depth);
		//to camera coordinate
		for (int i = 0; i < 8; i++) {
			v[i] * cam->M;
		}
		//012 023 326 367 034 437 045 051 475 576 152 256
		static INT indice[12][3] = {
			{ 0, 1, 2 },
			{ 0, 2, 3 },
			{ 3, 2, 6 },
			{ 3, 6, 7 },
			{ 0, 3, 4 },
			{ 4, 3, 7 },
			{ 0, 4, 5 },
			{ 0, 5, 1 },
			{ 4, 7, 5 },
			{ 5, 7, 6 },
			{ 1, 5, 2 },
			{ 2, 5, 6 }
		};
		INT intersect = 0;
		for (int i = 0; i < 12; i++) {
			v0.set(v[indice[i][0]]);
			v1.set(v[indice[i][1]]);
			v2.set(v[indice[i][2]]);

			n0.set(v1) - v0;
			n1.set(v2) - v0;
			n = n0 * n1;
			//back face culling
			EFTYPE cross = vd  & n;
			if (cross < 0) {
				continue;
			}
			EFTYPE trans = Vert3D::GetLineIntersectPointWithTriangle(v0, v1, v2, vo, vd, 10000, p);
			if (EP_GTZERO(trans)) {
				intersect = 1;
				break;
			}
		}
		if (intersect) {
			Obj3D * _obj = this->objects.link;
			if (_obj) {
				do {

					link->insertLink(_obj);

					_obj = this->objects.next(_obj);
				} while (_obj && _obj != this->objects.link);
			}
		}
	}

	void change(Obj3D * obj) {
		if (NULL == obj) {
			return;
		}
		if (NULL == obj->octTree) {
			this->Insert(obj);
		}
		else
		{
			((OctTree*)obj->octTree)->objects.removeLink(obj);
			if (((OctTree*)obj->octTree)->parent) {
				((OctTree*)obj->octTree)->parent->Insert(obj);
			}
			else {
				this->Insert(obj);
			}
		}
	}
};


#endif