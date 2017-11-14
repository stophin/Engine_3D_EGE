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

		initialize();
	}

	void initialize() {
		for (INT i = 0; i < 3; i++)
		{
			this->prev[i] = NULL;
			this->next[i] = NULL;
		}
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

	EFTYPE zz;

	INT ys;
	INT ye;
	INT xs;
	INT xe;

	Mat3D R;
	Mat3D R_r;

	EFTYPE backface;

	// for multilinklist
	INT uniqueID;
	VObj * prev[3];
	VObj * next[3];
	void operator delete(void * _ptr){
		if (_ptr == NULL)
		{
			return;
		}
		for (INT i = 0; i < 3; i++)
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
		_M(&M, NULL, 1), texture(NULL),
		cam(NULL), verts(0), verts_r(1), verts_f(2), transparent(0), reflection(0), v0(NULL), v1(NULL), render_aabb(0){
		center.init();
		center_r.init();
		center_w.init();

		aabb[0].set(-EP_MAX, -EP_MAX, -EP_MAX, 1);
		aabb[6].set(EP_MAX, EP_MAX, EP_MAX, 1);
	}
	~Object3D() {
		this->verts.~MultiLinkList();
		this->verts_r.~MultiLinkList();
	}

	DWORD *texture;
	INT t_w;
	INT t_h;

	Object3D&   setTexture(TextureManage& tman, INT uID) {
		Texture * ptexture = tman.textures.getLink(uID);

		if (NULL == ptexture) {
			return *this;
		}

		t_w = ptexture->width;
		t_h = ptexture->height;
		texture = ptexture->texture;

		return *this;
	}

	DWORD getTexture(INT x, INT y) {
		if (NULL == texture) {
			return this->color;
		}
		x %= t_w;
		y %= t_h;
		if (x < 0) {
			x = t_w + x;
		}
		if (y < 0) {
			y = t_h + y;
		}
		return texture[x + y * t_w];
	}

	Camera3D * cam;

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

	//add vertex to object
	Object3D& addVert(EFTYPE x, EFTYPE y, EFTYPE z, int anti_n) {
		this->addVert(x, y, z);

		//force normal vector to be negative
		//so that this point is not in backface
		if (anti_n < 0) {
			VObj * v = this->verts.prev(this->verts.link);
			if (v) v->n.negative();
		}

		return *this;
	}

	Object3D& addVert(EFTYPE x, EFTYPE y, EFTYPE z) {
		VObj * v = new VObj(x, y, z);

		if (this->v0 && this->v1) {

			// get normal vector 
			v->n.set(v0->v) - v->v;
			v->n_r.set(v1->v) - v->v;
			v->n * v->n_r;
			v->n.normalize();

			//this->verts.insertLink(new VObj(*v1));
			//this->verts.insertLink(new VObj(*v0));

			this->v0 = this->v1;
			this->v1 = v;
		}
		else if (this->v0 == NULL) {
			this->v0 = v;
		}
		else if (this->v1 == NULL) {
			this->v1 = v;
		}

		if (this->render_aabb) {
			if (this->aabb[0].x < v->v.x) this->aabb[0].x = v->v.x;
			if (this->aabb[0].y < v->v.y) this->aabb[0].y = v->v.y;
			if (this->aabb[0].z < v->v.z) this->aabb[0].z = v->v.z;

			if (this->aabb[6].x > v->v.x) this->aabb[6].x = v->v.x;
			if (this->aabb[6].y > v->v.y) this->aabb[6].y = v->v.y;
			if (this->aabb[6].z > v->v.z) this->aabb[6].z = v->v.z;
		}

		this->verts.insertLink(v);

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

					//any point in this plan(in world->camera coordinate)
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
						// set x0 for all vertexes
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
							v->n_d.set(v0->v_s) - v->v_s;
							this->center_r.set(v1->v_s) - v->v_s;
							v->n_d * this->center_r;

							// get normal vector for original z
							v->n_z.set(v0->x - v->x, v0->y - v->y, v0->zz - v->zz);
							this->center_r.set(v1->x - v->x, v1->y - v->y, v1->zz - v->zz);
							v->n_z * this->center_r;

							// get render range
							v->xs = max(Vert3D::get_minx(v0->v_s, v1->v_s, v->v_s), 0);
							v->xe = max(Vert3D::get_maxx(v0->v_s, v1->v_s, v->v_s), v->xs);
							v->ys = max(Vert3D::get_miny(v0->v_s, v1->v_s, v->v_s), 0);
							v->ye = max(Vert3D::get_maxy(v0->v_s, v1->v_s, v->v_s), v->ys);

							this->v0 = this->v1;
							this->v1 = v;
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
							this->verts_f.insertLink(v);
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
					this->center_w.set(this->center);
					this->center_w * M;
					this->center_r.set(this->center_w);
					this->center_r * this->cam->M;
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