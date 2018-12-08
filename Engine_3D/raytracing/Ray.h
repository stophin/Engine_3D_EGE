// Ray.h
//

#ifndef _RAY_H_
#define _RAY_H_

#include "../math3d/Vert3D.h"
#include "../common/MultiLinkList.h"

class Verts {
public:
	Verts() :
		type(0){
		initialize();
	}
	~Verts() {
	}

	EFTYPE trans;
	DWORD color;
	Vert3D v;
	Vert3D v_3;
	Vert3D v_n;
	Vert3D n_r;
	void * obj;

	//ray tracing type
	//0: normal(stop rendering)
	//1: reflection
	//2: transparent
	INT type;

	// for multilinklist
#define MAX_VERTS_LINK	2
	void initialize() {
		for (INT i = 0; i < MAX_VERTS_LINK; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
	}
	INT uniqueID;
	Verts * prev[MAX_VERTS_LINK];
	Verts * next[MAX_VERTS_LINK];
	void operator delete(void * _ptr);
	void * operator new(size_t size);
};

typedef unsigned char UMAP;
#define POOL_MAX	10240
#define MAP_SHIFT	8
#define MAP_MASK	0xFF
#define GET_MAP_SIZE(x) (x / MAP_SHIFT + 1)
class VertsPool {
public:
	VertsPool() :
		size(POOL_MAX),
		mutex(0){
		pool = new Verts[size];
		mapsize = GET_MAP_SIZE(size);
		map = new UMAP[mapsize];
		for (int i = 0; i < mapsize; i++) {
			map[i] = MAP_MASK;
		}
		used = 0;
	}
	~VertsPool() {
		if (pool) {
			delete[] pool;
			pool = NULL;
		}
		if (map) {
			delete[] map;
			map = NULL;
		}
	}
	INT mutex;
	Verts * pool;
	UMAP * map;
	INT size;
	INT mapsize;
	INT used;

	Verts * get() {
		int i, j, index;
		for (i = 0, index = 0; i < this->mapsize && index < this->size; i++, index += MAP_SHIFT) {
			if (this->map[i] & MAP_MASK) {
				for (j = 0; j < MAP_SHIFT && index < this->size; j++, index++) {
					if (this->map[i] & (0x01 << j)) {
						this->map[i] &= ~(0x01 << j);
						used++;
						return &this->pool[index];
					}
				}
			}
		}
		return NULL;
	}

	void back(Verts * o) {
		int i, j, index;
		if (o == NULL) {
			return;
		}
		for (index = 0; index < this->size; index++) {
			if (&this->pool[index] == o) {
				i = index / MAP_SHIFT;
				j = index - i * MAP_SHIFT;
				this->map[i] |= (0x01 << j);
				used--;
				memset(o, 0, sizeof(Verts));
				return;
			}
		}
		o = NULL;
		return;
	}
};

class VertsMan : public MultiLinkList<Verts> {
public:
	VertsMan(VertsPool* pool, INT index) :
		MultiLinkList<Verts>(index),
		pool(pool){
	}
	~VertsMan() {
	}

	Verts * free(Verts * _ptr) {
		if (_ptr == NULL)
		{
			return NULL;
		}
		for (INT i = 0; i < MAX_VERTS_LINK; i++)
		{
			if (((Verts*)_ptr)->prev[i] != NULL || ((Verts*)_ptr)->next[i] != NULL)
			{
				return NULL;
			}
		}
		return _ptr;
	}

	VOID clearLink() {
		if (this->link)
		{
			Verts * temp = this->link;
			do
			{
				if (removeLink(temp) == NULL)
				{
					break;
				}

				if (free(temp)) {
					pool->back(temp);
				}

				temp = this->link;
			} while (temp);
		}
	}

	VertsPool * pool;

};

class Ray {
public:
	Ray() :
	type(0){
	}
	Ray(const Vert3D& o, const Vert3D& d) :
		type(0){
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
	//ray type:
	//0: normal
	//1: reflection
	//2: refraction
	//3: shadow test ray
	INT type;

	void * obj;

	DWORD color;
	EFTYPE f;

	//r(t) = o + t * d, t >= 0
	Vert3D& getPoint(float t) {
		point.set(direction);
		point * t;
		point + original;

		return point;
	}
};

#endif