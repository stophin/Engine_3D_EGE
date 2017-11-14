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
	Vert3D n, n_1, n_2, n0, n1, n2, r;
	DWORD * _tango, *_image, *_trans, *_mirror;
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
	}

	//must be called after depth was rendered
	void RenderMirror(Manager3D & man, int move_light) {

		Obj3D * obj = man.refl.link, *temp = NULL;
		Mat3D mm, mm_1, mml, mml_1;
		if (obj) {
			// save original camera matrix
			mm.set(obj->cam->M);
			mm_1.set(obj->cam->M_1);
			do {

				VObj * v = obj->verts_f.link;
				VObj * v0 = NULL, *v1 = NULL;
				if (v && obj->verts_f.linkcount > 0) {

					// do not refresh relection surfaces
					temp = man.refl.link;
					man.refl.link = NULL;
					do {

						if (v0 && v1) {
							if (v->backface > 0) {

								// set camera matrix to vertex's reflection matrix
								obj->cam->M.set(mm) * v->R;
								obj->cam->M_1.set(v->R_r) * mm_1;
								man.refresh(0);

								// get reflection projection to array mirror
								// need to change target device and depth array
								DWORD * _temp = _tango;
								_tango = _mirror;
								FLOAT * _depth = depth;
								depth = deptr;
								//memset(depth, 0, width * height * sizeof(FLOAT));
								Render(man, v, v0, v1);
								// restore target device and depth array
								depth = _depth;
								_tango = _temp;

								DWORD * __trans, *__tango;
								INT index = 0, index_r = 0;
								for (int i = v->ys; i <= v->ye && i < height; i++) {
									for (int j = v->xs; j <= v->xe && j < width; j++) {
										index = i * width + j;
										__mirror = &_mirror[index];
										if (*__mirror != BLACK) 
										{
											EFTYPE z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z, (EFTYPE)j, (EFTYPE)i);
											__depth = &depth[index];
											if (EP_ISZERO(*__depth)) {
												*__depth = z;
											}
											if (*__depth <= z) {
												int res = Vert3D::IsInTriangle(v0->v_s, v1->v_s, v->v_s, p.set((FLOAT)j, (FLOAT)i, 0));
												if (res) {
													__tango = &_tango[index];
													//adding light reduction in reflection
													*__tango = Light3D::multi(*__mirror, 0.8);
													*__depth = z;
												}
											}
											*__mirror = BLACK;
										}
										//clear reflection depth at the same time
										deptr[index] = 0;
									}
								}
							}

							//ege::setcolor(RED);
							//ege::line(v0->x0, v0->y0, v1->x0, v1->y0);
							//ege::line(v1->x0, v1->y0, v->x0, v->y0);
							//ege::line(v->x0, v->y0, v0->x0, v0->y0);
							Draw_Line(_tango, width, height, v0->x0, v0->y0, v1->x0, v1->y0, WHITE);
							Draw_Line(_tango, width, height, v1->x0, v1->y0, v->x0, v->y0, WHITE);
							Draw_Line(_tango, width, height, v->x0, v->y0, v0->x0, v0->y0, WHITE);
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
			// restore original camera matrix
			obj->cam->M.set(mm);
			obj->cam->M_1.set(mm_1);
			man.refresh(0);
		}
	}


	void RenderShade(Manager3D& man) {
		//switch to the shadow camera
		Camera3D * cam = man.cams.link;
		for (int i = 0; i < man.cams.linkcount; i++) {
			if (man.cams.link && man.cams.link->type == 1) {
				break;
			}
			man.nextCamera();
		}
		if (man.cams.link->type != 1) {
			return;
		}
		if (NULL == man.cams.link) {
			return;
		}
		memset(shade, 0, width * height * sizeof(FLOAT));

		Mat3D mm, mm_1;

		VObj * _range = NULL;

		Obj3D * obj = man.objs.link;
		if (obj) {
			man.cams.link->M.set(man.lgts.link->M_1);
			man.cams.link->M_1.set(man.lgts.link->M);
			man.refresh(0);

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
									__image = &___image;

									EFTYPE z;
									INT index = 0;
									INT xs = _range == v ? v->xs : max(_range->xs, v->xs), ys = _range == v ? v->ys : max(_range->ys, v->ys),
										xe = _range == v ? v->xe : min(_range->xe, v->xe), ye = _range == v ? v->ye : min(_range->ye, v->ye);
									for (int i = ys; i <= ye && i < height; i++) {
										for (int j = xs; j <= xe && j < width; j++) {
											index = i * width + j;

											// linear interpolation
											int res = Vert3D::IsInTriangle(v0->v_s, v1->v_s, v->v_s, p.set((FLOAT)j, (FLOAT)i, 0));
											if (res > 0) {
												*__image = obj->color;
												//*__image = obj->line;
											}
											else if (res < 0) {
												*__image = obj->color;
											}
											else{
												*__image = BLACK;
											}

											if (*__image != BLACK) {
												__shade = &shade[index];
												// get shade
												//(-n.x * ((FLOAT)j - v.x) - n.y * ((FLOAT)i - v.y)) / n.z + v->z
												z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z, (EFTYPE)j, (EFTYPE)i);
												if (EP_ISZERO(*__shade)) {
													*__shade = z;
												}
												if (*__shade <= z) {
													*__shade = z;
												}
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

		//restore original camera
		for (int i = 0; i < man.cams.linkcount; i++) {
			if (man.cams.link == cam) {
				break;
			}
			man.nextCamera();
		}
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
									//if (Vert3D::CrossRect(range->xs, range->xe, range->ys, range->ye,v->xs, v->xe, v->ys, v->ye))
									_range = range;
								}
								else {
									_range = v;
								}
								// in range
								if (_range) {
									if (render_linear < 0) {
										cps[0].x = v0->x0; cps[0].y = v0->y0; cps[0].color = RED;
										cps[1].x = v1->x0; cps[1].y = v1->y0; cps[1].color = BLUE;
										cps[2].x = v->x0; cps[2].y = v->y0; cps[2].color = GREEN;

										// draw poly
										//ege::fillpoly_gradient(3, cps, image);
										// draw outline
										if (draw_line > 0) {
											Draw_Line(_image, width, height, v0->x0, v0->y0, v1->x0, v1->y0, WHITE);
											Draw_Line(_image, width, height, v1->x0, v1->y0, v->x0, v->y0, WHITE);
											Draw_Line(_image, width, height, v->x0, v->y0, v0->x0, v0->y0, WHITE);
											//ege::setcolor(WHITE);
											//ege::line(v0->x0, v0->y0, v1->x0, v1->y0, image);
											//ege::line(v1->x0, v1->y0, v->x0, v->y0, image);
											//ege::line(v->x0, v->y0, v0->x0, v0->y0, image);
										}
									}
									else {
										__image = &___image;
									}

									//step1: render the triangle
									index = 0;
									xs = _range == v ? v->xs : max(_range->xs, v->xs); ys = _range == v ? v->ys : max(_range->ys, v->ys);
									xe = _range == v ? v->xe : min(_range->xe, v->xe); ye = _range == v ? v->ye : min(_range->ye, v->ye);
									for (i = ys; i <= ye && i < height; i++) {
										for (j = xs; j <= xe && j < width; j++) {
											index = i * width + j;
											if (render_linear < 0) {
												__image = &_image[index];
											}
											else {
												// linear interpolation
												res = Vert3D::IsInTriangle(v0->v_s, v1->v_s, v->v_s, p.set((FLOAT)j, (FLOAT)i, 0));
												if (res > 0) {
													*__image = obj->color;
													//*__image = obj->line;
												}
												else if (res < 0) {
													*__image = obj->color;
												}
												else{
													*__image = BLACK;
												}
											}
											//step2: depth test
											if (*__image != BLACK) {
												__depth = &depth[index];
												// get depth
												//(-n.x * ((FLOAT)j - v.x) - n.y * ((FLOAT)i - v.y)) / n.z + v->z
												z = Vert3D::getZ(v->n_d, v->x0, v->y0, v->z, (EFTYPE)j, (EFTYPE)i);
												if (EP_ISZERO(*__depth)) {
													*__depth = z;
												}
												if (*__depth <= z) {
													__tango = &_tango[index];
													__trans = &_trans[index];

													if (render_linear < 0) {
														// replace gradient color to object's color
														// 11053224 is white, which changed from the 
														// setteled value 16579836
														// when painted in ege::fillpoly_gradient
														if (*__image == 11053224) {
															*__image = obj->line;
															//*__image = obj->color;
														}
														else {
															*__image = obj->color;
														}
													}
													else {
														//nothing todo
													}

													//step3: render light
													cam = obj->cam;
													if (cam) {
														n0.set((j - cam->offset_w) / cam->scale_w, (i - cam->offset_h) / cam->scale_h, z, 1);
														// get position
														//(-n_1.x * (n0.x - v->x) - n_1.y * (n0.y - v->y)) / n_1.z + v->zz;
														zz = Vert3D::getZ(v->n_z, v->x, v->y, v->zz, n0.x, n0.y);
														man.cams.link->anti_normalize(n0, zz);
														n1.set(n0) * cam->M_1;

														//set texture 
														n2.set(n1) * obj->M;
														*__image = obj->getTexture(n2.x, n2.y);

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
																	*__trans++;
																}
																if (trans_w1 < j) trans_w1 = j;
																if (trans_h1 < i) trans_h1 = i;
																if (trans_w0 > j) trans_w0 = j;
																if (trans_h0 > i) trans_h0 = i;
															}
														}
														else {
															*__image = Light3D::multi(*__image, f);
															*__tango = *__image;
															*__trans = BLACK;
														}

														//step5: render shadow map
														cam = man.cams.link;
														lgt = man.lgts.link;
														n2.set(n1) * lgt->M_1;
														man.cams.link->project(n1);
														_j = (int)(n1.x * cam->scale_w + cam->offset_w), _i = (int)(n1.y * cam->scale_h + cam->offset_h);

														if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
															_index = _i * width + _j;
															if (render_proj > 0) {
																_tango[_index] = RED;// obj->color;
															}

															//shadow
															if (EP_GTZERO(shade[_index] - z - 1e-1)) {
																*__tango = Light3D::multi(*__image, f / 10);
															}

														}

														if (render_proj > 0) {
															cam = man.cams.link;
															n2.set(n1) * cam->M;
															man.cams.link->project(n1);
															_j = (int)(n1.x * cam->scale_w + cam->offset_w), _i = (int)(n1.y * cam->scale_h + cam->offset_h);

															if (!(_i < 0 || _i > height - 1 || _j < 0 || _j > width - 1)) {
																_index = _i * width + _j;
																_tango[_index] = BLUE;// obj->color;
															}
														}
													}

													*__depth = z;
												}
												if (render_linear < 0) {
													*__image = BLACK;
												}
											}
										}
									}
									if (render_linear < 0) {
										for (i = v->ys; i <= v->ye && i < height; i++) {
											for (j = v->xs; j <= v->xe && j < width; j++) {
												_image[i * width + j] = BLACK;
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
		unsigned char color // 颜色像素
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
				}
				else if (ddx <= 0) {
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
				}
				else if (ddx <= 0) {
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
