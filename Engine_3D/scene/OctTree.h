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
#define MAX_LEVELS		(MAX_OBJ3D_LINK - MAX_OBJ3D_END) / (MAX_QUARDANTS + 1)
#define GET_LINK_INDEX(level, position)	(level * MAX_QUARDANTS + position + MAX_OBJ3D_END)
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
		objects(GET_LINK_INDEX(level, position)),
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
		objects(GET_LINK_INDEX(level, -1)),
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
		this->objects.linkindex = GET_LINK_INDEX(level, position);
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
			new OctTree(this->level + 1, x, y + subHeight, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::RightTopFront);
		this->children[OctTree_Link::LeftTopFront] =
			new OctTree(this->level + 1, x + subWidth, y + subHeight, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::LeftTopFront);
		this->children[OctTree_Link::LeftBottomFront] =
			new OctTree(this->level + 1, x + subWidth, y, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::LeftBottomFront);
		this->children[OctTree_Link::RightBottomFront] =
			new OctTree(this->level + 1, x, y, z + subDepth, subWidth, subHeight, subDepth, this, OctTree_Link::RightBottomFront);
		this->children[OctTree_Link::RightTopBack] =
			new OctTree(this->level + 1, x, y + subHeight, z, subWidth, subHeight, subDepth, this, OctTree_Link::RightTopBack);
		this->children[OctTree_Link::LeftTopBack] =
			new OctTree(this->level + 1, x + subWidth, y + subHeight, z, subWidth, subHeight, subDepth, this, OctTree_Link::LeftTopBack);
		this->children[OctTree_Link::LeftBottomBack] =
			new OctTree(this->level + 1, x + subWidth, y, z, subWidth, subHeight, subDepth, this, OctTree_Link::LeftBottomBack);
		this->children[OctTree_Link::RightBottomBack] =
			new OctTree(this->level + 1, x, y, z, subWidth, subHeight, subDepth, this, OctTree_Link::RightBottomBack);
		this->hasChild = true;
	}

	INT GetIndex(Rect3D& rect) {
		INT index = -1;
		EFTYPE xMidPoint = this->bounds.x + this->bounds.width / 2;
		EFTYPE yMidPoint = this->bounds.y + this->bounds.height / 2;
		EFTYPE zMidPoint = this->bounds.z + this->bounds.depth / 2;

		EFTYPE bottomQuad = (EP_GEZERO(rect.y - this->bounds.y) && EP_LEZERO(rect.y + rect.height - yMidPoint));
		EFTYPE topQuad = (EP_GEZERO(rect.y - yMidPoint) && EP_LEZERO(rect.y + rect.height -( this->bounds.y + this->bounds.height)));
		EFTYPE backQuad = (EP_GEZERO(rect.z - this->bounds.z) && EP_LEZERO(rect.z + rect.depth - zMidPoint));
		EFTYPE frontQuad = (EP_GEZERO(rect.z - zMidPoint) && EP_LEZERO(rect.z + rect.depth - ( this->bounds.z + this->bounds.depth)));

		if (EP_GEZERO(rect.x - xMidPoint) && EP_LEZERO(rect.x + rect.width - ( this->bounds.x + this->bounds.width))) {//leftQuad
			if (topQuad) {
				if (frontQuad) {
					index = OctTree_Link::LeftTopFront;//leftTopFront
				}
				else if (backQuad) {
					index = OctTree_Link::LeftTopBack;//leftTopBack
				}
			}
			else if (bottomQuad) {
				if (frontQuad) {
					index = OctTree_Link::LeftBottomFront;//leftBottomFront
				}
				else if (backQuad) {
					index = OctTree_Link::LeftBottomBack;//leftBottomBack
				}
			}
		}
		else if (EP_GEZERO(rect.x - this->bounds.x) && EP_LEZERO( rect.x + rect.width - xMidPoint)) {//rightQuad
			if (topQuad) {
				if (frontQuad) {
					index = OctTree_Link::RightTopFront;//rightTopFront
				}
				else if (backQuad) {
					index = OctTree_Link::RightTopBack;//rightTopBack
				}
			}
			else if (bottomQuad) {
				if (frontQuad) {
					index = OctTree_Link::RightBottomFront;//rightBottmFront
				}
				else if (backQuad) {
					index = OctTree_Link::RightBottomBack;//rightBottomBack
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
				obj->octTree = this;
			}
		}
		this->objects.insertLink(obj);
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
	INT Collision(Vert3D& vo, Vert3D& vd, Camera3D *  cam, Obj3D * obj) {
		if (NULL == cam) {
			return 0;
		}
		if (NULL == obj) {
			return 0;
		}
		Obj3D * _obj = obj;

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
		Vert3D v0, v1, v2;
		Vert3D n0, n1, n, p;
		for (int i = 0; i < 12; i++) {
			v0.set(_obj->aabb_r[indice[i][0]]);
			v1.set(_obj->aabb_r[indice[i][1]]);
			v2.set(_obj->aabb_r[indice[i][2]]);

			n0.set(v1) - v0;
			n1.set(v2) - v0;
			n.set(n0) * n1;
			//back face culling
			EFTYPE cross = vd  & n;
			if (cross * _obj->anti < 0) {
				continue;
			}
			EFTYPE trans = Vert3D::GetLineIntersectPointWithTriangle(v0, v1, v2, vo, vd, 10000, p);
			if (EP_GTZERO(trans)) {
				intersect = 1;
				break;
			}
		}
		if (intersect) {
			return 1;
		}
		return 0;
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