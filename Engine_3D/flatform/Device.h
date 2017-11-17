//Device.h
//

#ifndef _DEVICE_H_
#define _DEVICE_H_

#include "../math3d/Manager3D.h"

struct Device {
	INT width;
	INT height;

	FLOAT *depth;//Depth test buffer
	DWORD *image;//Image buffer
	DWORD *tango;//Target buffer
	FLOAT *shade;//Shade buffer
	DWORD *trans;//Transparent buffer
	FLOAT *deptr;//Reflection depth buffer
	DWORD *miror;//Reflection bufer

	INT draw_line = 1;
	INT render_linear = 1;
	INT render_proj = -1;
	INT render_light = -1;

	EPoint e, s, v, c;
	Vert3D p;
	ege_colpoint cps[3];
	Vert3D n, n_1, n_2, n0, n1, n2, n3, r;
	DWORD * _tango, *_image, *_trans, *_mirror;
	FLOAT * _depth;
	DWORD * __image, *__tango, *__trans, *__mirror;
	FLOAT *__depth, *__shade;
	DWORD ___image, ___last = 0;

	Device() :
		width(100),
		height(100),
		depth(NULL),
		image(NULL),
		tango(NULL),
		shade(NULL),
		trans(NULL),
		deptr(NULL),
		miror(NULL){

	}
	Device(INT w, INT h){
		Resize(w, h);
	}
	~Device() {
		Destory();
	}

	void Destory() {
		if (depth) {
			delete[] depth;
			depth = NULL;
		}
		if (image) {
			delete[] image;
			image = NULL;
		}
		if (tango) {
			delete[] tango;
			tango = NULL;
		}
		if (shade) {
			delete[] shade;
			shade = NULL;
		}
		if (trans) {
			delete[] trans;
			trans = NULL;
		}
		if (deptr) {
			delete[] deptr;
			deptr = NULL;
		}
		if (miror) {
			delete[] miror;
			miror = NULL;
		}
	}

	void Resize(INT w, INT h)  {
		Destory();
		width = w;
		height = h;

		depth = new FLOAT[width * height];
		image = new DWORD[width * height];
		tango = new DWORD[width * height];
		shade = new FLOAT[width * height];
		trans = new DWORD[width * height];
		deptr = new FLOAT[width * height];
		miror = new DWORD[width * height];
		_image = image;
		_tango = tango;
		_trans = trans;
		_mirror = miror;
		_depth = depth;
	}

	//must be called after depth was rendered
	void RenderMirror(Manager3D & man) {
		//switch to the reflection camera
		Camera3D * cur_cam = man.cams.link;
		Camera3D * cam = man.getCamera(2);
		if (NULL == cam || NULL == cur_cam) {
			return;
		}
		//set cam parameters to current camera
		cam->M.set(cur_cam->M);
		cam->M_1.set(cur_cam->M_1);

		Obj3D * obj = man.refl.link, *temp = NULL;

		DWORD *__tango;
		INT index = 0;
		EFTYPE z;
		INT line_state = 0;
		INT line_l = 0, line_r = 0;
		if (obj) {
			do {

				VObj * v = obj->verts_f.link;
				VObj * v0 = NULL, *v1 = NULL;
				if (v && obj->verts_f.linkcount > 0) {

					// do not refresh relection surfaces to aviod dead loop
					temp = man.refl.link;
					man.refl.link = NULL;
					do {

						if (v0 && v1) {
							if (v->backface > 0) {

								// set camera matrix to vertex's reflection matrix
								cur_cam->M.set(cam->M) * v->R;
								cur_cam->M_1.set(v->R_r) * cam->M_1;
								man.refresh(cur_cam);

								// get reflection projection to array mirror
								// need to change target device and depth array
								DWORD * ___tango = _tango;
								_tango = _mirror;
								FLOAT * ___depth = _depth;
								_depth = deptr;
								//memset(depth, 0, width * height * sizeof(FLOAT));
								Render(man, v, v0, v1);
								// restore target device and depth array
								_depth = ___depth;
								_tango = ___tango;

								Draw_Line(_image, width, height, v0->x0, v0->y0, v1->x0, v1->y0, WHITE);
								Draw_Line(_image, width, height, v1->x0, v1->y0, v->x0, v->y0, WHITE);
								Draw_Line(_image, width, height, v->x0, v->y0, v0->x0, v0->y0, WHITE);

								for (int i = v->ys; i <= v->ye && i < height; i++) {
									//little trick^_^
									line_state = 0;
									line_l = 0; 
									line_r = 0;
									//trick: pre-judge
									___image = BLACK;
									for (int j = v->xs; j <= v->xe && j < width; j += 1) {
										__image = &_image[i * width + j];
										//up pulse
										if (*__image != BLACK && ___image == BLACK) {
											line_state++;
											if (line_state == 1) {
												line_l = j;
											}
											else {
												line_r = j;
												break;
											}
										}
										___image = *__image;
									}
									for (int j = v->xs; j <= v->xe && j < width; j++) {
										index = i * width + j;
										__mirror = &_mirror[index];

										__image = &_image[index];
										if (j >= line_l && j <= line_r) {
											*__image = obj->color;
										}
										if (*__image != BLACK) {
											__depth = &_depth[index];

											// get depth
											//(-n.x * ((FLOAT)j - v.x) - n.y * ((FLOAT)i - v.y)) / n.z + v->z
											n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, 0, 1);
											//z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z0, (EFTYPE)j, (EFTYPE)i);
											z = Vert3D::getZ(v->n_1_z, v->x, v->y, v->z, n0.x, n0.y);
											if (EP_ISZERO(*__depth)) {
												*__depth = z;
											}
											if (*__depth <= z) {
												*__depth = z;

												__tango = &_tango[index];
												//adding light reduction in reflection
												*__tango = Light3D::multi(*__mirror, 0.8);
											}
										}
										//clear reflection depth at the same time
										deptr[index] = 0;
										//clear drawing
										*__mirror = BLACK;
										*__image = BLACK;
									}
								}
							}
							v0 = v1;
							v1 = v;
						}
						else if (v0 == NULL) {
							v0 = v;
						}
						else if (v1 == NULL) {
							v1 = v;
						}

						v = obj->verts_f.next(v);
					} while (v && v != obj->verts_f.link);
					// do not refresh relection surfaces
					// so that there will be no loop
					// and the parameters which passed to
					// render() will not be changed
					man.refl.link = temp;
				}

				obj = man.refl.next(obj);
			} while (obj && obj != man.refl.link);
		}

		// restore current camera and refresh
		cur_cam->M.set(cam->M);
		cur_cam->M_1.set(cam->M_1);
		man.refresh(NULL);
	}


	void RenderShade(Manager3D& man) {
		//switch to the shadow camera
		Camera3D * cur_cam = man.cams.link;
		Camera3D * cam = man.getCamera(1);
		if (NULL == cam || NULL == cur_cam) {
			return;
		}
		Light3D * lgt = man.lgts.link;
		if (NULL == lgt) {
			return;
		}
		cam->M.set(cur_cam->M);
		cam->M_1.set(cur_cam->M_1);

		memset(shade, 0, width * height * sizeof(FLOAT));

		Mat3D mm, mm_1;

		VObj * _range = NULL;

		Obj3D * obj = man.objs.link;

		EFTYPE z;
		INT index = 0, _index = 0;
		INT xs, xe, ys, ye;
		INT line_state = 0;
		INT line_l = 0, line_r = 0;
		if (obj) {
			//set camera matrix to anti-light matrix
			cur_cam->M.set(lgt->M_1);
			cur_cam->M_1.set(lgt->M);
			man.refresh(cur_cam);

			int render_trans = 0;
			int trans_w0 = EP_MAX, trans_h0 = EP_MAX;
			int trans_w1 = -EP_MAX, trans_h1 = -EP_MAX;
			do {
				VObj * v = obj->verts_r.link;
				if (v && obj->verts_r.linkcount > 0) {
					VObj *v0 = NULL, *v1 = NULL, *vtemp;
					do {
						if (v0 && v1) {
							// back face cull
							if (v->backface > 0) {

								_range = v;
								// in range
								if (_range) {
									//draw triangle contour
									Draw_Line(_image, width, height, v0->x0, v0->y0, v1->x0, v1->y0, WHITE);
									Draw_Line(_image, width, height, v1->x0, v1->y0, v->x0, v->y0, WHITE);
									Draw_Line(_image, width, height, v->x0, v->y0, v0->x0, v0->y0, WHITE);

									index = 0;
									xs = v->xs; xe = v->xe; ys = v->ys; ye = v->ye;
									for (int i = ys; i <= ye && i < height; i++) {

										//little trick^_^
										line_state = 0;
										line_l = 0, line_r = 0;
										//trick: pre-judge
										___image = BLACK;
										for (int j = xs; j <= xe && j < width; j += 1) {
											__image = &_image[i * width + j];
											//up pulse
											if (*__image != BLACK && ___image == BLACK) {
												line_state++;
												if (line_state == 1) {
													line_l = j;
												}
												else {
													line_r = j;
													break;
												}
											}
											___image = *__image;
										}
										for (int j = xs; j <= xe && j < width; j++) {
											index = i * width + j;
											__image = &_image[index];
											if (j >= line_l && j <= line_r) {
												*__image = obj->color;
											}

											if (*__image != BLACK) {
												__shade = &shade[index];
												// get shade
												//(-n.x * ((FLOAT)j - v.x) - n.y * ((FLOAT)i - v.y)) / n.z + v->z
												n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, 0, 1);
												//z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z0, (EFTYPE)j, (EFTYPE)i);
												z = Vert3D::getZ(v->n_1_z, v->x, v->y, v->z, n0.x, n0.y);
												if (EP_ISZERO(*__shade)) {
													*__shade = z;
												}
												if (*__shade <= z) {
													*__shade = z;
												}
											}
											*__image = BLACK;
										}
									}
								}
							}

							v0 = v1;
							v1 = v;
						}
						else if (v0 == NULL) {
							v0 = v;
						}
						else if (v1 == NULL) {
							v1 = v;
						}

						v = obj->verts_r.next(v);
					} while (v && v != obj->verts_r.link);
				}

				//first do objects till end
				//then do transparent object
				if (render_trans == 0) {
					obj = man.objs.next(obj);
					if (!(obj && obj != man.objs.link)) {
						obj = man.tras.link;
						render_trans = 1;
					}
				}
				else {
					obj = man.tras.next(obj);
					render_trans++;
					if (!(obj && obj != man.tras.link)) {
						break;
					}
				}
			} while (obj && obj != man.objs.link);
		}

		// restore current camera and refresh
		cur_cam->M.set(cam->M);
		cur_cam->M_1.set(cam->M_1);
		man.refresh(NULL);
	}

	void ClearBeforeRender() {
		memset(depth, 0, width * height * sizeof(FLOAT));
		memset(tango, 0, width * height * sizeof(DWORD));
		memset(image, 0, width * height * sizeof(DWORD));
		memset(trans, 0, width * height * sizeof(DWORD));
	}

	void Render(Manager3D& man, VObj * range, VObj * range0, VObj * range1) {
		VObj * _range;

		Mat3D mm;

		Obj3D * obj = man.objs.link;
		if (obj) {
			int render_trans = 0;
			int trans_w0 = EP_MAX, trans_h0 = EP_MAX;
			int trans_w1 = -EP_MAX, trans_h1 = -EP_MAX;
			VObj * v, *v0, *v1, *vtemp;

			EFTYPE z;
			INT index = 0, _index = 0;
			INT xs, xe, ys, ye;
			int i, j;
			int res;
			Camera3D* cam = NULL;
			Lgt3D * lgt;
			FLOAT zz;
			EFTYPE f, t, transparent, _i, _j;
			INT line_state = 0;
			INT line_l = 0, line_r = 0;
			int inrange;
			do {
				v = obj->verts_r.link;
				if (v && obj->verts_r.linkcount > 0) {
					v0 = NULL; v1 = NULL;
					do {
						//there must be three points
						if (v0 && v1) {
							// back face cull
							if (v->backface > 0) {

								_range = NULL;
								if (range) {
									_range = range;
								}
								else {
									_range = v;
								}
								// in range
								inrange = true;
								if (_range != v) {
									inrange = EPoint::RectIsIntersect(v->xs, v->ys, v->xe, v->ye, _range->xs, _range->ys, _range->xe, _range->ye);
									//inrange = Vert3D::CrossRect(v->xs, v->ys, v->xe, v->ye, _range->xs, _range->ys, _range->xe, _range->ye);
								}
								if (_range && inrange) {

									//step1: render the triangle
									index = 0;
									xs = v->xs; xe = v->xe; ys = v->ys; ye = v->ye;
									//xs = _range == v ? v->xs : max(_range->xs, v->xs); ys = _range == v ? v->ys : max(_range->ys, v->ys);
									//xe = _range == v ? v->xe : min(_range->xe, v->xe); ye = _range == v ? v->ye : min(_range->ye, v->ye);
									//draw triangle contour
									Draw_Line(_image, width, height, v0->x0, v0->y0, v1->x0, v1->y0, WHITE);
									Draw_Line(_image, width, height, v1->x0, v1->y0, v->x0, v->y0, WHITE);
									Draw_Line(_image, width, height, v->x0, v->y0, v0->x0, v0->y0, WHITE);

									for (i = ys; i <= ye && i < height; i += 1) {
										cam = obj->cam;
										if (cam == NULL) {
											break;
										}
										//little trick^_^
										line_state = 0;
										line_l = 0, line_r = 0;
										if (render_linear < 0) {
											line_l = xs;
											line_r = xe;
										}
										else {
											//trick: pre-judge
											___image = BLACK;
											for (j = xs; j <= xe && j < width; j += 1) {
												__image = &_image[i * width + j];
												//up pulse
												if (*__image != BLACK && ___image == BLACK) {
													line_state++;
													if (line_state == 1) {
														line_l = j;
													}
													else {
														line_r = j;
														break;
													}
												}
												___image = *__image;
											}
										}
										for (j = xs; j <= xe  && j < width; j += 1) {
											index = i * width + j;
											__image = &_image[index];
											if (render_linear < 0) {
											}
											else {
												if (j >= line_l && j <= line_r) {
													*__image = obj->color;
												}
											}
											//step2: depth test
											if (*__image != BLACK) {
												__depth = &_depth[index];

												// get depth
												//(-n.x * ((FLOAT)j - v.x) - n.y * ((FLOAT)i - v.y)) / n.z + v->z
												n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, 0, 1);
												//z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z0, (EFTYPE)j, (EFTYPE)i);
												z = Vert3D::getZ(v->n_1_z, v->x, v->y, v->z, n0.x, n0.y);
												if (EP_ISZERO(*__depth)) {
													*__depth = z;
												}
												if (*__depth <= z) {
													*__depth = z;

													__tango = &_tango[index];
													__trans = &_trans[index];

													//step3: render light
													//n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, z, 1);
													n0.z = z;
													// get position
													n0 * cam->proj_1;
													zz = (v->n_r.x * v->v_c.x + v->n_r.y * v->v_c.y + v->n_r.z * v->v_c.z) / 
														(v->n_r.x * n0.x + v->n_r.y * n0.y  + v->n_r.z);
													n0.x *= zz;
													n0.y *= zz;
													n0.z = zz;
													n0.w = 1;
													n1.set(n0)* cam->M_1;

													if (obj->type == 0) {
														//set texture 
														n2.set(n1)*obj->M_1;

														//*__image = obj->getTexture(n2.y * obj->t_w, n2.z * obj->t_h);
														//get the max projection plat： xy or yz or xz?
														EFTYPE mx = v->aabb[0].x - v->aabb[1].x, my = v->aabb[0].y - v->aabb[1].y, mz = v->aabb[0].z - v->aabb[1].z;
														//EFTYPE mx = obj->aabb[0].x - obj->aabb[6].x, my = obj->aabb[0].y - obj->aabb[6].y, mz = obj->aabb[0].z - obj->aabb[6].z;
														EFTYPE sxy = n3.set(0, 0, 1) ^ (v->n), syz = n3.set(1, 0, 0) ^ (v->n), sxz = n3.set(0, 1, 0) ^ (v->n);
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
													else if (obj->type == 1) {
														//sphere map
														// reflection vector
														// R = I - 2 ( I * N ) N
														n2.set(n0);// .normalize();
														n3.set(v->n_r);
														n3 * (n2 ^ n3) * 2;
														n2 - n3;
														// transition vector
														// m = r + cam(0, 0, 1)
														n2 + cam->lookat;
														//n2.z += 1;
														n2.normalize();

														n2.x = n2.x * 0.5 + 0.5;
														n2.y = n2.y * 0.5 + 0.5;
														n2.z = n2.z * 0.5 + 0.5;

														*__image = obj->getTexture(n2.x, n2.y);
													}

													//calculate sumption of light factors
													lgt = man.lgts.link;
													f = 0;
													if (lgt) {
														do {
															f += lgt->getFactor(v->n_r, n0);

															if (render_light < 0) {
																break;
															}

															lgt = man.lgts.next(lgt);
														} while (lgt && lgt != man.lgts.link);
													}

													//step4: render transparent
													if (!EP_ISZERO(obj->transparent)) {
														r.set(n0);
														t = r.negative() & v->n_r;
														if (t < 0) t = -t;
														transparent = obj->transparent;
														if (transparent < 0) transparent = -transparent;

														_i = (i - obj->center_r.y) * (transparent / t) + obj->center_r.y;
														_j = (j - obj->center_r.x) * (transparent / t) + obj->center_r.x;

														if (obj->transparent < 0) {
															_i = 2 * obj->center_r.y - _i;
															_j = 2 * obj->center_r.x - _j;
														}
														if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
															_index = (INT)_i * width + (INT)_j;
															//if (depth[_index] < z) 
															if (1)
															{
																*__trans = Light3D::add(*__image, _tango[_index], f);
															}
															else {
																*__trans = Light3D::multi(*__image, f);
															}
															if (*__trans == BLACK) {
																//*__trans++;
																*__trans = *__image;
															}
															//get transparent range
															if (trans_w1 < j) trans_w1 = j;
															if (trans_h1 < i) trans_h1 = i;
															if (trans_w0 > j) trans_w0 = j;
															if (trans_h0 > i) trans_h0 = i;
														}
													}
													else {
														*__image = Light3D::multi(*__image, f);
														*__tango = *__image;
													}

													//step5: render shadow map
													lgt = man.lgts.link;
													n2.set(n1) * lgt->M_1;
													cam->project(n2);
													_j = (int)(n2.x * cam->scale_w + cam->offset_w), _i = (int)(n2.y * cam->scale_h + cam->offset_h);

													if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
														_index = _i * width + _j;
														__shade = &shade[_index];
														if (render_proj > 0) {
															_tango[_index] = RED;// obj->color;
														}

														//shadow
														if (EP_GTZERO(*__shade - n2.z - 1e-1)) {
															*__tango = Light3D::multi(*__image, f / 5);
														}

													}

													if (render_proj > 0) {
														n2.set(n0)*cam->M_1 *obj->M_1* obj->M* cam->M;
														//n2.set(n1) * cam->M;
														cam->project(n2);

														INT __j = (int)(n2.x * cam->scale_w + cam->offset_w), __i = (int)(n2.y * cam->scale_h + cam->offset_h);
														//Draw_Line(_tango, width, height, __j, __i, _j, _i, RED);
														_j = __j, _i = __i;

														if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
															_index = _i * width + _j;
															_tango[_index] = BLUE;// obj->color
														}
													}
												}
											}

											if (render_linear < 0) {
												_image[index] = BLACK;
											}
											else {
												_image[index] = BLACK;
											}
										}
									}
								}
							}

							v0 = v1;
							v1 = v;
						}
						else if (v0 == NULL) {
							v0 = v;
						}
						else if (v1 == NULL) {
							v1 = v;
						}

						v = obj->verts_r.next(v);
					} while (v && v != obj->verts_r.link);
				}

				//first do objects till end
				//then do transparent object
				if (render_trans == 0) {
					obj = man.objs.next(obj);
					if (!(obj && obj != man.objs.link)) {
						obj = man.tras.link;
						render_trans = 1;
					}
				}
				else {
					obj = man.tras.next(obj);
					render_trans++;
					if (!(obj && obj != man.tras.link)) {
						//render transparent after all transparent objects were done
						index = 0;
						for (i = trans_h0; i <= trans_h1 && i < height; i++) {
							for (j = trans_w0; j <= trans_w1 && j < width; j++) {
								index = i * width + j;
								__trans = &_trans[index];
								if (*__trans != BLACK) {
									__tango = &_tango[index];
									*__tango = *__trans;
									*__trans = BLACK;
								}
							}
						}

						break;
					}
				}
			} while (obj && obj != man.objs.link);
		}
	}

	int Draw_Line(DWORD* vb_start, int lpitch, int height,
		int x1, int y1, // 起始点
		int x2, int y2, // 终点
		DWORD color // 颜色像素
		) // video buffer and memory pitch
	{

		// this function draws a line from xo,yo to x1,y1 using differential error
		// terms (based on Bresenahams work)

		int dx, // difference in x's
			dy, // difference in y's
			dx2, // dx,dy * 2
			dy2,
			x_inc, // amount in pixel space to move during drawing
			y_inc, // amount in pixel space to move during drawing
			error, // the discriminant i.e. error i.e. decision variable
			index; // used for looping

		// pre-compute first pixel address in video buffer
		//vb_start = vb_start + x1 + y1 * lpitch;

		// compute horizontal and vertical deltas
		dx = x2 - x1;
		dy = y2 - y1;

		// test which direction the line is going in i.e. slope angle
		if (dx >= 0)
		{
			x_inc = 1;

		} // end if line is moving right
		else
		{
			x_inc = -1;
			dx = -dx; // need absolute value

		} // end else moving left

		// test y component of slope

		if (dy >= 0)
		{
			y_inc = lpitch;
		} // end if line is moving down
		else
		{
			y_inc = -lpitch;
			dy = -dy; // need absolute value

		} // end else moving up

		// compute (dx,dy) * 2
		dx2 = dx << 1;
		dy2 = dy << 1;

		// now based on which delta is greater we can draw the line
		if (dx > dy)     //斜率小于1的情况
		{
			// initialize error term
			error = dy2 - dx;

			int ddy = y1 * lpitch, ddx = x1;
			// draw the line
			for (index = 0; index <= dx; index++)
			{
				// set the pixel
				if (ddx >= lpitch) {
					if (ddy >= height * lpitch) {
					}
					else if (ddy <= 0) {
					}
					else {
						vb_start[lpitch - 1 + ddy] = color;
					}
				}
				else if (ddx <= 0) {
					if (ddy >= height * lpitch) {
					}
					else if (ddy <= 0) {
					} else {
						vb_start[ddy] = color;
					}
				}
				else if (ddy >= height * lpitch) {
				}
				else if (ddy <= 0) {
				}
				else {
					//*vb_start = color;
					vb_start[ddx + ddy] = color;
				}

				// test if error has overflowed
				if (error >= 0)
				{
					error -= dx2;
					// move to next line
					//vb_start += y_inc;
					ddy += y_inc;
				}
				// adjust the error term
				error += dy2;
				// move to the next pixel
				//vb_start += x_inc;
				ddx += x_inc;
			}
		}
		else   //斜率大于等于1的情况
		{
			// initialize error term
			error = dx2 - dy;

			int ddy = y1 * lpitch, ddx = x1;
			for (index = 0; index <= dy; index++)
			{
				// set the pixel
				if (ddx >= lpitch) {
					if (ddy >= height * lpitch) {
					}
					else if (ddy <= 0) {
					}
					else {
						vb_start[lpitch - 1 + ddy] = color;
					}
				}
				else if (ddx <= 0) {
					if (ddy >= height * lpitch) {
					}
					else if (ddy <= 0) {
					}
					else {
						vb_start[ddy] = color;
					}
				}
				else if (ddy >= height * lpitch) {
				}
				else if (ddy <= 0) {
				}
				else {
					//*vb_start = color;
					vb_start[ddx + ddy] = color;
				}

				// test if error overflowed
				if (error >= 0)
				{
					error -= dy2;//这里按博主推导这里该为error+=dx2-dy2;

					// move to next line
					//vb_start += x_inc;
					ddx += x_inc;

				} // end if error overflowed

				// adjust the error term
				error += dx2;

				// move to the next pixel
				//vb_start += y_inc;
				ddy += y_inc;
			}
		}
		return(1);

	} // end Draw_Line

};



#endif//_DEVICE_H_
