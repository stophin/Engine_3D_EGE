// Object3D.h
// Object for 3D
//

#ifndef _OBJECT3D_H_
#define _OBJECT3D_H_

#include "Camera3D.h"

#include "../common/MultiLinkList.h"
#include "../math3d/Texture.h"

typedef class VObj VObj;
class VObj {
public:
	VObj(EFTYPE x, EFTYPE y, EFTYPE z) {
		this->v.set(x, y, z, 1);

		this->v_c.init();
		this->v_r.init();
		this->n.init();
		this->n_r.init();
		this->n_d.init();
		this->n_z.init();
		this->v_w.init();
		this->n_w.init();
		this->n_1_z.init();

		initialize();
	}

	void initialize() {
		for (INT i = 0; i < 3; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
		aabb[0].set(-EP_MAX, -EP_MAX, -EP_MAX, 1);
		aabb[1].set(EP_MAX, EP_MAX, EP_MAX, 1);
	}
	Vert3D v;
	Vert3D v_c;
	union {
		Vert3D v_r;
		struct {
			EFTYPE x;
			EFTYPE y;
			EFTYPE z;
			EFTYPE w;
		};
	};
	union {
		Vert3D v_s;
		struct {
			EFTYPE x0;
			EFTYPE y0;
			EFTYPE z0;
			EFTYPE w0;
		};
	};
	Vert3D v_w;
	Vert3D n;
	Vert3D n_w;
	Vert3D n_r;
	Vert3D n_d;
	Vert3D n_z;
	Vert3D n_1_z;

	EFTYPE zz;

	INT ys;
	INT ye;
	INT xs;
	INT xe;

	Mat3D R;
	Mat3D R_r;

	EFTYPE backface;

	Vert3D aabb[2];

	// for multilinklist
#define MAX_VOBJ_LINK	4
	INT uniqueID;
	VObj * prev[MAX_VOBJ_LINK];
	VObj * next[MAX_VOBJ_LINK];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < MAX_VOBJ_LINK; i++)
		{
			if (((VObj*)_ptr)->prev[i] != NULL || ((VObj*)_ptr)->next[i] != NULL)
			{
				return;
			}
		}
		delete(_ptr);
	}
};

typedef class Object3D Object3D;
class Object3D {
public:
	Object3D() : 
		_M(&M, &M_1, 1), texture(NULL),
		cam(NULL), indice(3), verts(0), verts_r(1), verts_f(2), transparent(0), reflection(0), v0(NULL), v1(NULL), render_aabb(0),
		texture_type(0), vertex_type(0){
		center.init();
		center_r.init();
		center_w.init();

		aabb[0].set(-EP_MAX, -EP_MAX, -EP_MAX, 1);
		aabb[6].set(EP_MAX, EP_MAX, EP_MAX, 1);
	}
	~Object3D() {
		this->indice.~MultiLinkList();
		this->verts.~MultiLinkList();
		this->verts_r.~MultiLinkList();
		this->verts_f.~MultiLinkList();
	}

	DWORD *texture;
	INT t_w;
	INT t_h;
	// texture type
	// 0: normal texture
	// 1: SphereMap(environment reflection)
	// 2: SphereMap(object texture)
	// 3: SphereMap(world texture)
	// 4: CubeMap
	INT texture_type;
	INT u;
	INT v;

	Object3D& setUV(INT u, INT v) {
		this->u = u;
		this->v = v;

		return *this;
	}

	Object3D&   setTexture(TextureManage& tman, INT uID, INT t = 0) {
		Texture * ptexture = tman.textures.getLink(uID);

		if (NULL == ptexture) {
			return *this;
		}

		t_w = ptexture->width;
		t_h = ptexture->height;
		texture = ptexture->texture;
		texture_type = t;

		this->setUV(0, 0);

		return *this;
	}

	DWORD getTexture(EFTYPE x, EFTYPE y) {
		if (NULL == texture) {
			return this->color;
		}
		INT _u = x *t_w, _v = y * t_h;
		_u += this->u;// -t_w / 2.0;
		_v += this->v;// -t_h / 2.0;
		_u %= t_w;
		_v %= t_h;
		if (_u < 0) {
			_u = t_w + _u;
		}
		if (_v < 0) {
			_v = t_h + _v;
		}
		return texture[_u + _v * t_w];
	}

	//n0: camera coordinate
	//n1: word coordinate
	//n2: temp vector
	//n3: temp vector
	//v: vertex
	//v_n: get the normal vector(camera coordinate)
	DWORD getTextureColor(Vert3D& n0, Vert3D& n1, Vert3D& n2, Vert3D& n3, VObj * v, Vert3D* v_n = NULL) {
		if (NULL == this->cam) {
			return this->color;
		}
		Object3D * obj = this;
		DWORD color;
		DWORD * __image = &color;
		if (obj->texture_type == 0) {
			//set texture 
			n2.set(n1)*obj->M_1;

			if (v_n) {
				v_n->set(v->n_r);
			}

			//*__image = obj->getTexture(n2.y * obj->t_w, n2.z * obj->t_h);
			//get the max projection plat£º xy or yz or xz?
			EFTYPE sxy = n3.set(0, 0, 1) ^ (v->n), syz = n3.set(1, 0, 0) ^ (v->n), sxz = n3.set(0, 1, 0) ^ (v->n);
			//get geometry size
			EFTYPE mx = v->aabb[0].x - v->aabb[1].x, my = v->aabb[0].y - v->aabb[1].y, mz = v->aabb[0].z - v->aabb[1].z;
			//EFTYPE mx = obj->aabb[0].x - obj->aabb[6].x, my = obj->aabb[0].y - obj->aabb[6].y, mz = obj->aabb[0].z - obj->aabb[6].z;
			//EFTYPE sxy = mx * my, syz = my * mz, sxz = mx * mz;
			if (sxy < 0) sxy = -sxy;
			if (syz < 0) syz = -syz;
			if (sxz < 0) sxz = -sxz;
			if (mx < 0) mx = -mx;
			if (my < 0) my = -my;
			if (mz < 0) mz = -mz;
			if (sxy > sxz) {
				if (sxy > syz) {
					*__image = obj->getTexture(n2.x / mx, n2.y / my);
				}
				else {
					*__image = obj->getTexture(n2.y / my, n2.z / mz);
				}
			}
			else {
				if (sxz > syz) {
					*__image = obj->getTexture(n2.x / mx, n2.z / mz);
				}
				else {
					*__image = obj->getTexture(n2.y / my, n2.z / mz);
				}
			}
		}
		else if (obj->texture_type == 1) {
			//sphere map(reflection)
			// reflection vector
			// R = I -  N * ( dot(I , N)* 2 )
			//get n3 = N
			n2.set(0, 0, 0, 1);
			n2 * obj->M * cam->M;
			n3.set(n0);
			n3 - n2;

			if (v_n) {
				v_n->set(n3);
			}

			//get n2 = I
			n2.set(n0);
			//get n2 = R
			EFTYPE cross = n2 ^ n3;
			n3 * (cross * 2);
			n2 - n3;
			// transition vector
			// m = r + cam(0, 0, 1)
			n3.set(cam->lookat);
			n2 + n3;
			n2.normalize();

			n2.x = n2.x * 0.5 + 0.5;
			n2.y = n2.y * 0.5 + 0.5;

			*__image = obj->getTexture(n2.x, n2.y);
		}
		else if (obj->texture_type == 2) {
			//sphere map(object texture)
			// reflection vector
			// R = I -  N * ( dot(I , N)* 2 )
			//get n3 = N
			n2.set(0, 0, 0, 1);
			//n2 * obj->M;
			n3.set(n1) * obj->M_1;
			n3 - n2;

			if (v_n) {
				v_n->set(n3)* obj->M * cam->M;
			}

			//get n2 = I
			n2.set(n1) * obj->M_1;
			//get n2 = R
			EFTYPE cross = n2 ^ n3;
			n3 * (cross * 2);
			n2 - n3;
			// transition vector
			// m = r + cam(0, 0, 1)
			n3.set(cam->lookat);
			n2 + n3;
			n2.normalize();

			n2.x = n2.x * 0.5 + 0.5;
			n2.y = n2.y * 0.5 + 0.5;

			*__image = obj->getTexture(n2.x, n2.y);

		}
		else if (obj->texture_type == 3) {
			//sphere map(world texture)
			// reflection vector
			// R = I -  N * ( dot(I , N)* 2 )
			//get n3 = N
			n2.set(0, 0, 0, 1);
			n2 * obj->M;
			n3.set(n1);
			n3 - n2;

			if (v_n) {
				v_n->set(n3) *cam->M;
			}

			//get n2 = I
			n2.set(n1);
			//get n2 = R
			EFTYPE cross = n2 ^ n3;
			n3 * (cross * 2);
			n2 - n3;
			// transition vector
			// m = r + cam(0, 0, 1)
			n3.set(cam->lookat);
			n2 + n3;
			n2.normalize();

			n2.x = n2.x * 0.5 + 0.5;
			n2.y = n2.y * 0.5 + 0.5;

			*__image = obj->getTexture(n2.x, n2.y);

		}
		else if (obj->texture_type == 4) {
			//sphere map(world texture)
			// reflection vector
			// R = I -  N * ( dot(I , N)* 2 )
			//get n3 = N
			n2.set(0, 0, 0, 1);
			n2 * obj->M;
			n3.set(n1);
			n3 - n2;

			if (v_n) {
				v_n->set(n3) *cam->M;
			}

			EFTYPE sxy = n2.set(0, 0, 1) ^ n3, syz = n2.set(1, 0, 0) ^ n3, sxz = n2.set(0, 1, 0) ^ n3;
			//get n2 = I
			n2.set(n1);
			//get n2 = R
			EFTYPE cross = n2 ^ n3;
			n3 * (cross * 2);
			n2 - n3;
			// transition vector
			// m = r + cam(0, 0, 1)
			n3.set(cam->lookat);
			n2 + n3;
			n2.normalize();

			n2.x = n2.x * 0.5 + 0.5;
			n2.y = n2.y * 0.5 + 0.5;
			n2.z = n2.z * 0.5 + 0.5;

			//*__image = obj->getTexture(n2.y * obj->t_w, n2.z * obj->t_h);
			EFTYPE _sxy = sxy, _syz = syz, _sxz = sxz;
			if (sxy < 0) sxy = -sxy;
			if (syz < 0) syz = -syz;
			if (sxz < 0) sxz = -sxz;
			EFTYPE dw = 1.0 / 4.0, dh = 1.0 / 3.0;
			EFTYPE _dw = dw, _dh = dh;
			if (sxy > sxz) {
				if (sxy > syz) {
					if (_sxy < 0) {
						//-z
						*__image = obj->getTexture(n2.x * _dw + 0 * dw, n2.y * _dh + 1 * dh);
					}
					else {
						//+z
						*__image = obj->getTexture(n2.x * _dw + 2 * dw, n2.y * _dh + 1 * dh);
					}
				}
				else {
					if (_syz < 0) {
						//-x
						*__image = obj->getTexture(n2.y * _dw + 1 * dw, n2.z * _dh + 1 * dh);
					}
					else {
						//+x
						*__image = obj->getTexture(n2.y * _dw + 3 * dw, n2.z * _dh + 1 * dh);
					}
				}
			}
			else {
				if (sxz > syz) {
					if (_sxz < 0) {
						//-y
						*__image = obj->getTexture(n2.x * _dw + 2 * dw, n2.z * _dh + 0 * dh);
					}
					else {
						//+y
						*__image = obj->getTexture(n2.x * _dw + 2 * dw, n2.z * _dh + 2 * dh);
					}
				}
				else {
					if (_syz < 0) {
						//-x
						*__image = obj->getTexture(n2.y * _dw + 1 * dw, n2.z * _dh + 1 * dh);
					}
					else {
						//+x
						*__image = obj->getTexture(n2.y * _dw + 3 * dw, n2.z * _dh + 1 * dh);
					}
				}
			}
		}

		return color;
	}

	Camera3D * cam;

	MultiLinkList<VObj> indice;
	MultiLinkList<VObj> verts;
	MultiLinkList<VObj> verts_r;
	MultiLinkList<VObj> verts_f;


	EFTYPE reflection;

	EFTYPE transparent;
	Vert3D center;
	Vert3D center_w;
	Vert3D center_r;
	Object3D& setCenter(EFTYPE x, EFTYPE y, EFTYPE z) {
		this->center.set(x, y, z);

		return *this;
	}

	COLORREF color;
	COLORREF line;
	Object3D&  setLineColor(COLORREF line) {
		this->line = line;

		return *this;
	}
	Object3D&  setColor(COLORREF color) {
		this->color = color;

		return *this;
	}

	Object3D& setVertexType(int type) {
		this->vertex_type = type;

		return *this;
	}

	//vertext type :
	//0: triangle strip
	//1: triangle
	int vertex_type;

	int anti;
	//add vertex to object
	Object3D& addVert(EFTYPE x, EFTYPE y, EFTYPE z, int anti_n) {
		VObj * v = new VObj(x, y, z);

		int _anti = this->anti;
		this->anti *= anti_n;
		this->addVert(v);
		this->anti = _anti;

		return *this;
	}

	Object3D& addVert(VObj *v) {
		if (NULL == v) {
			return *this;
		}
		if (this->v0 && this->v1) {
			// get normal vector 
			if (EP_ISZERO(v->n.x + v->n.y + v->n.z)) {
				v->n.set(v0->v) - v->v;
				v->n_r.set(v1->v) - v->v;
				v->n * v->n_r;
				v->n.normalize();
			}
			// vertex aabb test
			for (int i = 0; i < 3; i++) {
				VObj * _v = NULL;
				if (i == 0) {
					_v = this->v0;
				}
				else if (i == 1) {
					_v = this->v1;
				}
				else {
					_v = v;
				}
				//max
				if (v->aabb[0].x < _v->v.x) v->aabb[0].x = _v->v.x;
				if (v->aabb[0].y < _v->v.y) v->aabb[0].y = _v->v.y;
				if (v->aabb[0].z < _v->v.z) v->aabb[0].z = _v->v.z;

				//min
				if (v->aabb[1].x > _v->v.x) v->aabb[1].x = _v->v.x;
				if (v->aabb[1].y > _v->v.y) v->aabb[1].y = _v->v.y;
				if (v->aabb[1].z > _v->v.z) v->aabb[1].z = _v->v.z;

				//set other 2 vertexes' normal vector
				if (vertex_type == 1) {
					if (EP_ISZERO(_v->n.x + _v->n.y + _v->n.z)) {
						_v->n.set(v->n);
					}
				}
			}
			if (vertex_type == 1) {
				this->v0 = NULL;
				this->v1 = NULL;
			}
			else {
				this->v0 = this->v1;
				this->v1 = v;
			}
		}
		else if (this->v0 == NULL) {
			this->v0 = v;
		}
		else if (this->v1 == NULL) {
			this->v1 = v;
		}

		if (this->render_aabb) {
			//max
			if (this->aabb[0].x < v->v.x) this->aabb[0].x = v->v.x;
			if (this->aabb[0].y < v->v.y) this->aabb[0].y = v->v.y;
			if (this->aabb[0].z < v->v.z) this->aabb[0].z = v->v.z;

			//min
			if (this->aabb[6].x > v->v.x) this->aabb[6].x = v->v.x;
			if (this->aabb[6].y > v->v.y) this->aabb[6].y = v->v.y;
			if (this->aabb[6].z > v->v.z) this->aabb[6].z = v->v.z;
		}

		this->verts.insertLink(v);

		//force normal vector to be negative
		//so that this point is not in backface
		if (this->anti < 0) {
			v->n.negative();
		}

		return *this;
	}


	Object3D& addVert(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE nx = 0, EFTYPE ny = 0, EFTYPE nz = 0) {
		VObj * v = new VObj(x, y, z);
		if (!EP_ISZERO(nx + ny + nz)) {
			v->n.set(nx, ny, nz);
		}

		return addVert(v);
	}

	Object3D& addIndice(EFTYPE x, EFTYPE y, EFTYPE z, EFTYPE nx = 0, EFTYPE ny = 0, EFTYPE nz = 0) {
		VObj * v = new VObj(x, y, z);
		if (!EP_ISZERO(nx + ny + nz)) {
			v->n.set(nx, ny, nz);
		}

		this->indice.insertLink(v);

		return *this;
	}

	Object3D& setIndice(INT pv, INT pv0, INT pv1) {
		if (this->indice.linkcount < 3) {
			return *this;
		}
		VObj * _v = this->indice.getPos(pv);
		VObj * _v0 = this->indice.getPos(pv0);
		VObj * _v1 = this->indice.getPos(pv1);

		if (NULL != _v && NULL != _v0 && NULL != _v1) {
			for (int i = 0; i < 3; i++) {
				VObj * __v = NULL;
				if (i == 0) {
					__v = _v0;
				}
				else if (i == 1) {
					__v = _v1;
				}
				else {
					__v = _v;
				}

				addVert(__v->v.x, __v->v.y, __v->v.z, __v->n.x, __v->n.y, __v->n.z);
				//addVert(__v);
			}
		}

		return *this;
	}

	VObj *v0, *v1;

	INT render_aabb;
	Vert3D aabb[8];

	Object3D& renderAABB() {
		this->render_aabb = 1;

		return *this;
	}

	void render_normalize() {
		if (this->cam == NULL) {
			return;
		}

		VObj * v = this->verts.link;
		if (v) {
			CM.setI() * M * this->cam->M;

			if (this->render_aabb > 0) {
				this->render_aabb = -this->render_aabb;
				//8 quadrants
				this->aabb[1].set(this->aabb[0].x, this->aabb[6].y, this->aabb[0].z, 1);
				this->aabb[2].set(this->aabb[6].x, this->aabb[6].y, this->aabb[0].z, 1);
				this->aabb[3].set(this->aabb[6].x, this->aabb[6].y, this->aabb[0].z, 1);
				this->aabb[4].set(this->aabb[0].x, this->aabb[0].y, this->aabb[6].z, 1);
				this->aabb[5].set(this->aabb[0].x, this->aabb[6].y, this->aabb[6].z, 1);
				this->aabb[7].set(this->aabb[6].x, this->aabb[0].y, this->aabb[6].z, 1);
			}
			if (this->render_aabb) {
				int i;
				for (i = 0; i < 8; i++) {
					// object coordinate -> world coordinate -> camera coordinate
					v->v_r.set(this->aabb[i]) * CM;
					v->v_r.w = EP_MAX;
					// cut in camera coordinate
					if (!this->cam->normalize(v->v_r)) {
						break;
					}
				}
				if (i == 8) {
					//not in camera
					this->verts_r.clearLink();
					this->verts_f.clearLink();
					return;
				}
			}

			int i = 0;
			// clear render link
			if (this->verts_r.linkcount > 0){
				//this->verts_r.clearLink();
				this->verts_r.linkcount = -this->verts_r.linkcount;
			}
			// clear reflection link
			if (this->verts_f.linkcount > 0){
				//this->verts_f.clearLink();
				this->verts_f.linkcount = -this->verts_f.linkcount;
			}
			this->v0 = NULL;
			this->v1 = NULL;
			do {
				if (this->reflection> 0) {
					/*this->reflection --;
					if (this->reflection == 0) {
						this->reflection--;
					}*/
					EFTYPE nx, ny, nz;
					EFTYPE x0, y0, z0;
					EFTYPE d;

					//any point in this plane(in world->camera coordinate)
					v->v_w.set(v->v) * CM;
					x0 = v->v_w.x; y0 = v->v_w.y; z0 = v->v_w.z;

					//normal vector(in world->camera coordinate)
					v->n_w.set(v->n) * CM;
					v->n_w.normalize();

					// get reflection matrix
					nx = v->n_w.x, ny = v->n_w.y, nz = v->n_w.z;
					d = -(v->n_w ^ v->v_w);// -nx * x0 - ny * y0 - nz * z0;
					v->R.mx.set(1 - 2 * nx * nx, -2 * ny * nx, -2 * nz * nx, -2 * d * nx);
					v->R.my.set(-2 * nx * ny, 1 - 2 * ny * ny, -2 * nz * ny, -2 * d * ny);
					v->R.mz.set(-2 * nx * nz, -2 * ny * nz, 1 - 2 * nz * nz, -2 * d * nz);
					v->R.mw.set(0, 0, 0, 1);

					//normal vector(anti, in world->camera coordinate)
					v->n_w.set(v->n).negative() * CM;
					v->n_w.normalize();

					// get reflection matrix(anti)
					nx = v->n_w.x; ny = v->n_w.y; nz = v->n_w.z;
					d = -(v->n_w ^ v->v_w);// -nx * x0 - ny * y0 - nz * z0;
					v->R_r.mx.set(1 - 2 * nx * nx, -2 * ny * nx, -2 * nz * nx, -2 * d * nx);
					v->R_r.my.set(-2 * nx * ny, 1 - 2 * ny * ny, -2 * nz * ny, -2 * d * ny);
					v->R_r.mz.set(-2 * nx * nz, -2 * ny * nz, 1 - 2 * nz * nz, -2 * d * nz);
					v->R_r.mw.set(0, 0, 0, 1);
				}

				// object coordinate -> world coordinate -> camera coordinate
				v->v_c.set(v->v) * CM;
				v->zz = v->v_c.z;
				v->v_r.set(v->v_c);
				// camera coordinate -> view region
				if (!this->cam->normalize(v->v_r)) {

					if (1){

						// get normal vector
						v->n_r.set(v->n) ^ CM;
						v->n_r.normalize();
						// set x0 for all vertexes(this is point which is for render)
						v->v_s.set(v->x * this->cam->scale_w + this->cam->offset_w, v->y * this->cam->scale_h + this->cam->offset_h, v->z);

						if (this->v0 && this->v1) {

							// back face cull mark, do not render if minus 0
							// though this point will not be rendered
							// it is necessary cause there'll be a 
							// hole if it is missing :>
							v->backface = (this->center_r.set(v->v_c).negative() ^ v->n_r);

							// get normal vector for depth
							//v->n_d.set(v0->x0 - v->x0, v0->y0 - v->y0, v0->z - v->z);
							//this->center_r.set(v1->x0 - v->x0, v1->y0 - v->y0, v1->z - v->z);
							//v->n_d.set(v0->v_s) - v->v_s;
							//this->center_r.set(v1->v_s) - v->v_s;
							//v->n_d * this->center_r;

							// get normal vector for original z
							//v->n_z.set(v0->x - v->x, v0->y - v->y, v0->zz - v->zz);
							//this->center_r.set(v1->x - v->x, v1->y - v->y, v1->zz - v->zz);
							//v->n_z * this->center_r;
							v->n_1_z.set(v0->x - v->x, v0->y - v->y, v0->z - v->z);
							this->center_r.set(v1->x - v->x, v1->y - v->y, v1->z - v->z);
							v->n_1_z * this->center_r;
							

							// get render range
							v->xs = max(Vert3D::get_minx(v0->v_s, v1->v_s, v->v_s), 0);
							v->xe = max(Vert3D::get_maxx(v0->v_s, v1->v_s, v->v_s), v->xs);
							v->ys = max(Vert3D::get_miny(v0->v_s, v1->v_s, v->v_s), 0);
							v->ye = max(Vert3D::get_maxy(v0->v_s, v1->v_s, v->v_s), v->ys);

							if (vertex_type == 1) {
								this->v0 = NULL;
								this->v1 = NULL;
							}
							else {
								this->v0 = this->v1;
								this->v1 = v;
							}
						}
						else if (this->v0 == NULL) {
							this->v0 = v;
						}
						else if (this->v1 == NULL) {
							this->v1 = v;
						}

						if (this->reflection) {
							if (this->verts_f.linkcount < 0) {
								this->verts_f.linkcount = -this->verts_f.linkcount;
								this->verts_f.clearLink();
							}
							if (this->verts_r.linkcount < 0) {
								this->verts_r.linkcount = -this->verts_r.linkcount;
								this->verts_r.clearLink();
							}
							this->verts_f.insertLink(v);
							this->verts_r.insertLink(v);
						}
						else {
							if (this->verts_r.linkcount < 0) {
								this->verts_r.linkcount = -this->verts_r.linkcount;
								this->verts_r.clearLink();
							}
							this->verts_r.insertLink(v);
						}
					}
				}

				v = this->verts.next(v);
			} while (v && v != this->verts.link);
			if (this->verts_r.linkcount > 0) {
				if (!EP_ISZERO(this->transparent)) {
					//this->center_w.set(this->center);
					//this->center_w * M;
					//this->center_r.set(this->center_w);
					//this->center_r * this->cam->M;
					this->center_r.set(this->center) * CM;
					if (!this->cam->normalize(this->center_r)) {
						this->center_r.x = this->center_r.x * this->cam->scale_w + this->cam->offset_w;
						this->center_r.y = this->center_r.y * this->cam->scale_h + this->cam->offset_h;
					}
				}
			}
		}
	}

	Matrix3D _M;
	Mat3D M;
	Mat3D M_1;
	Mat3D CM;
	Object3D& move(EFTYPE dx, EFTYPE dy, EFTYPE dz) {
		this->_M.move(dx, dy, dz);

		this->render_normalize();

		return *this;
	}

	Object3D& scale(EFTYPE sx, EFTYPE sy, EFTYPE sz) {
		this->_M.scale(sx, sy, sz);

		this->render_normalize();

		return *this;
	}

	Object3D& rotate(EFTYPE ax, EFTYPE ay, EFTYPE az) {

		this->_M.rotate(ax, ay, az);

		this->render_normalize();

		return *this;
	}
};

#endif