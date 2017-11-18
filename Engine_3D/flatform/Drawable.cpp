
#include "Drawable.h"


Manager3D man;
Device device;
TextureManage tman;

int DEBUG_MODE = DEBUG_GRADE_2;

INT isresize = -1;
INT isrefresh = -1;
INT width;
INT height;
INT draw_line = 1;
INT move_light = -1;
INT move_trans = -1;

VOID onResize(FLOAT width, FLOAT height)
{
	isresize = 1;
}

VOID onClose() {
}

EPoint org;
EFTYPE scalex, scaley;

VOID onPaint(HWND hWnd)
{
	if (isresize)
	{
		if (isresize != -1)
		{
			onClose();
		}
		isresize = 0;

		//
		width = getwidth();
		height = getheight();

		device.Resize(width, height);

		org.Set(width, height);
		org /= 2;
		scalex = org.x / 4, scaley = org.x / 4;

		man.setCameraRange(org.x, org.y, scalex, scaley);
	}
	if (isrefresh < 1) {
		return;
	}
	isrefresh = -1;
	// Place draw code here
	setcolor(BLACK);
	cleardevice();
	//Render in device buffer
	if (move_light > 0) {
		device.RenderShade(man);
	}
	device.ClearBeforeRender();
	device.Render(man, NULL, NULL, NULL);
	device.RenderMirror(man);
	//Blt buffer to window buffer
	DWORD * _tango = EP_GetImageBuffer();
	int i, j, index;
	for (i = 0; i < device.width; i++) {
		for (j = 0; j < device.height; j++){
			index = j *  device.width + i;
			if (device.tango[index] != BLACK)
			{
				//::SetPixel(memHDC, i, j, device.tango[index]);
				_tango[index] = device.tango[index];
			}
		}
	}

	//BitBlt(hdc, 0, 0, nWidth, nHeight, memHdc, 0, 0, SRCCOPY);
}

#include "../teapot.h"

Object3D * cur_op = NULL;
VOID Initialize()
{
	man.addCamera(50, 50, 50, 1000, 90, 90).move(0, 0, -200);
	man.addCamera(30, 30, 60, 600, 30, 30).move(0, 0, -100);
	man.addShadowCamera();
	man.addReflectionCamera();

	//man.addLight(9, -51, -60);
	man.addLight(5, 100, 220);
	//man.addLight(-1000, 100, 100);

	//load resource
	INT t0 = tman.addTexture(480, 480, 10);
	INT t1 = tman.addTexture("1.jpg");
	INT t2 = tman.addTexture("2.jpg");
	INT t3 = tman.addTexture("3.jpg");
	INT t4 = tman.addTexture(64, 64, 8);
	INT t5 = tman.addTexture(64, 64, 2);
	INT t6 = tman.addTexture("6.jpg");
	INT t7 = tman.addTexture("7.jpg");
	INT t8 = tman.addTexture("8.jpg");

	// generate teapot
	Object3D& obj = man.addObject().renderAABB().setColor(RED).setLineColor(RED).setVertexType(1);
	int normal = -1;
	int vertex_count = 0;
	int triangle_count = 0;
	for (int i = 0; i <= g_teapotPositionNum - 3; i += 3) {
		vertex_count++;
		obj.addIndice(g_teapotPositions[i], g_teapotPositions[i + 1], g_teapotPositions[i + 2]);
			//,g_teapotNormals[i], g_teapotNormals[i + 1], g_teapotNormals[i + 2]);
	}
	for (int i = 0; i <= g_teapotIndicesNum - 3; i += 3) {
		triangle_count++;
		obj.setIndice(g_teapotIndices[i], g_teapotIndices[i + 1], g_teapotIndices[i + 2]);
	}
	obj.move(50, -30, 0).scale(2, 2, 2).rotate(-90, 30, 0);// .setTexture(tman, t7, 1);

	//for (int i = 0; i < 1; i++) {
	//	for (int j = 0; j < 1; j++) {
	//		man.addObject().addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
	//			.scale(2, 2, 2).rotate(0, 0, 180).move(300 - 40 * j, -40, 300 - 40 * i).setColor(LIGHTGRAY).setLineColor(RED);
	//	}
	//}

	//man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10)
	//	.scale(10, 10, 10).move(0, -30, -300).setColor(GREEN);
	//man.addObject().addVert(-10, -10, 10).addVert(10, 10, 10).addVert(-10, 10, 10)
	//	.scale(10, 10, 10).move(0, -30, -100).setColor(GREEN);

	//man.addObject().addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
	//	.rotate(-90, -90, -90).move(-100, -20, 0).setColor(LIGHTGRAY).setLineColor(RED);


	int count = 2;
	int c = 30;
	int i, j, k;
	EFTYPE r = 10;
	EFTYPE x_1, x_2, r_1, r_2, p_1 = PI / ((EFTYPE)c), p_2 = 2 * PI / ((EFTYPE)c);

	//man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
	//	.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
	//	.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
	//	.scale(5, 5, 5).move(100, 10, 50).setColor(RED).setLineColor(BLUE).setTexture(tman, t2);
	c = 30;
	p_1 = PI / ((EFTYPE)c); p_2 = 2 * PI / ((EFTYPE)c);
	count = 1;
	for (k = 0; k < count; k++) {
		EFTYPE x = rand() % 300 - 150;
		EFTYPE z = rand() % 300 - 150;
		EFTYPE y = rand() % 100;
		for (i = 0; i < c; i++) {
			x_1 = r * cos(i * p_1);
			r_1 = r * sin(i * p_1);
			x_2 = r * cos((i + 1) * p_1);
			r_2 = r * sin((i + 1) * p_1);
			Object3D& obj = man.addObject().renderAABB().addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2);
			for (j = 1; j < c; j++) {
				obj.addVert(x_1, r_1 * sin(j * p_2), -r_1 * cos(j * p_2))
					.addVert(x_2, r_2 * sin(j * p_2), -r_2 * cos(j * p_2), -1);
			}
			obj.addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2, -1).setCenter(0, 0, 0).scale(10, 10, 10).move(x, y, z).rotate(0, 0, 0)
				.setColor(GREEN).setLineColor(RED).setTexture(tman, t7, 1);

			cur_op = &obj;
		}
	}

	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			man.addObject().addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
				.scale(4, 2, 2).rotate(0, 0, 180).move(300 - 80 * j, -40, 300 - 40 * i).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t4);
		}
	}

	c = 50;
	p_1 = PI / ((EFTYPE)c); p_2 = 2 * PI / ((EFTYPE)c);
	count = 1;
	for (k = 0; k < count; k++) {
		EFTYPE x = rand() % 300 - 150;
		EFTYPE z = rand() % 300 - 150;
		EFTYPE y = rand() % 100;
		for (i = 0; i < c; i++) {
			x_1 = r * cos(i * p_1);
			r_1 = r * sin(i * p_1);
			x_2 = r * cos((i + 1) * p_1);
			r_2 = r * sin((i + 1) * p_1);
			Object3D& obj = man.addTransparentObject(-0.51).renderAABB().addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2);
			for (j = 1; j < c; j++) {
				obj.addVert(x_1, r_1 * sin(j * p_2), -r_1 * cos(j * p_2))
					.addVert(x_2, r_2 * sin(j * p_2), -r_2 * cos(j * p_2), -1);
			}
			obj.addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2, -1).setCenter(0, 0, 0).scale(10, 10, 10).move(x, y, z).rotate(0, 0, 0).setColor(GREEN).setLineColor(RED).setTexture(tman, t6, 1);

			cur_op = &obj;
		}
	}


	man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.scale(10, 10, 10).move(0, 100, -300).setColor(GREEN).setTexture(tman, t1).setUV(30, 30);


	man.addObject().addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
		.scale(10, 10, 10).rotate(-90, -90, 180).move(-100, -20, 0).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t0);
	man.addTransparentObject(1.01).addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(-15, 0, -50).setColor(RED).setLineColor(BLUE).setTexture(tman, t3);

	man.addReflectionObject(1000).addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
		.scale(10, 10, 10).rotate(90, 90, 0).move(200, -20, 0).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t1);

	for (i = 0; i < c; i++) {
		x_1 = r * cos(i * p_1);
		r_1 = r * sin(i * p_1);
		x_2 = r * cos((i + 1) * p_1);
		r_2 = r * sin((i + 1) * p_1);
		Object3D& obj = man.addTransparentObject(-0.51).renderAABB().addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2);
		for (j = 1; j < c; j++) {
			obj.addVert(x_1, r_1 * sin(j * p_2), -r_1 * cos(j * p_2))
				.addVert(x_2, r_2 * sin(j * p_2), -r_2 * cos(j * p_2), -1);
		}
		obj.addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2, -1).setCenter(0, 0, 0).move(0, 0, 50).rotate(0, 0, 0).setColor(GREEN).setLineColor(RED);
	}


	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, -15, -50).setColor(RED).setLineColor(BLUE);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, 15, -50).setColor(RED).setLineColor(BLUE);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(15, 0, -50).setColor(RED).setLineColor(BLUE);
	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(0, 0, -50).setColor(BLUE).setLineColor(BLUE);

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 15; j++) {
			man.addObject().addVert(-10, -10, 0).addVert(10, -10, 0).addVert(-10, 10, 0).addVert(10, 10, 0, -1)
				.scale(2, 2, 2).move(0, -60, -600).move(300 - 40 * j, 200 - 40 * i, 0).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t5);
		}
	}

	man.addObject().renderAABB().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.move(0, 0, -100).setColor(BROWN).setLineColor(RED);

}

EFTYPE scale = 10.0;
INT is_control = 0;
VOID onScroll(FLOAT delta) {
	if (is_control) {
		if (delta > 0) {
			scale += 1.0;
		}
		else {
			scale -= 1.0;
		}
		if (scale <= 0) {
			scale = 1.0;
		}
	}
	else {
		if (move_light > 0) {
			if (delta > 0) {
				man.moveLight(0, 0, scale);
			}
			else {
				man.moveLight(0, 0, -scale);
			}
		}
		else if (move_trans > 0) {

			if (delta > 0) {
				Obj3D * obj = man.objs.link;
				if (obj) {
					do {

						obj->move(0, 0, scale);


						obj = man.objs.next(obj);
					} while (obj && obj != man.objs.link);
				}
			}
			else {
				Obj3D * obj = man.objs.link;
				if (obj) {
					do {

						obj->move(0, 0, -scale);


						obj = man.objs.next(obj);
					} while (obj && obj != man.objs.link);
				}
			}
		}
		else {
			if (delta > 0) {
				man.moveCamera(0, 0, scale);
			}
			else {
				man.moveCamera(0, 0, -scale);
			}
		}

		isrefresh = 1;
	}
}

EPointF menu;
VOID onMenu(FLOAT x, FLOAT y, INT mode)
{
	if (mode == 1) // mouse down
	{
		menu.X = x;
		menu.Y = y;
	}
	else if (mode == 2) // mouse move
	{
		if (menu.X != 0 && menu.Y != 0)
		{
			if (move_light > 0) {
				man.moveLight(-(x - menu.X) / scale, -(y - menu.Y) / scale, 0);
			}
			else {
				man.moveCamera(-(x - menu.X) / scale, -(y - menu.Y) / scale, 0);
			}
			menu.X = x;
			menu.Y = y;

			isrefresh = 1;
		}
	}
	else	// mouse up
	{
		menu.X = 0;
		menu.Y = 0;
	}
}

EPointF drag;
VOID onDrag(FLOAT x, FLOAT y, INT mode)
{
	if (mode == 1) // mouse down
	{
		drag.X = x;
		drag.Y = y;
	}
	else if (mode == 2) // mouse move
	{
		if (drag.X != 0 && drag.Y != 0)
		{
			if (move_light > 0) {
				man.rotateLight(-(y - drag.Y) / scale, (x - drag.X) / scale, 0);
			}
			else {
				man.rotateCamera(-(y - drag.Y) / scale, (x - drag.X) / scale, 0);
			}
			drag.X = x;
			drag.Y = y;

			isrefresh = 1;
		}
	}
	else	// mouse up
	{
		drag.X = 0;
		drag.Y = 0;
	}
}

INT lean = 0;
VOID onTimer()
{
	return;
	int count = 0;
	Obj3D * obj = man.objs.link;
	if (obj) {
		//obj->rotate(0, 10, 0);
	}
	obj = man.objs.prev(man.objs.link);
	if (obj) {
		obj->rotate(0, 10, 0);
		//obj->scale(1.05, 1.05, 1.05);
	}

	obj = man.tras.link;
	if (obj) {
		do {

			obj->rotate(0, 10, 0);
			/*
			obj->move(0, -5, 0);

			if (obj->center_w.y + 30 < 0) {
				obj->move(0, 50, 0);
			}
			*/

			obj = man.tras.next(obj);
		} while (obj && obj != man.tras.link);
	}
}
VOID onKeyUp(WPARAM wParam) {
	switch (wParam) {
	case VK_CONTROL:
		is_control = 0;
		break;
	case 'Q':
		if (lean) {
			man.rotateCamera(0, 0, -lean);
			lean = 0;
		}
		break;
	case 'E':
		if (lean) {
			man.rotateCamera(0, 0, -lean);
			lean = 0;
		}
		break;
	}

	isrefresh = 1;
}
VOID onKeyDown(WPARAM wParam)
{
	if (1)
	{
		switch (wParam)
		{
		case VK_LEFT:
			wParam = 'J';
			break;
		case VK_RIGHT:
			wParam = 'L';
			break;
		case VK_UP:
			wParam = 'I';
			break;
		case VK_DOWN:
			wParam = 'K';
			break;
		case 'A':
			wParam = VK_LEFT;
			break;
		case 'D':
			wParam = VK_RIGHT;
			break;
		//case 'W':
		//	wParam = 'Y';
		//	break;
		//case 'S':
		//	wParam = 'N';
		case 'W':
			wParam = VK_UP;
			break;
		case 'S':
			wParam = VK_DOWN;
			break;
		//case 'Q':
		//	if (lean == 0) {
		//		lean = -scale * 4;
		//		man.rotateCamera(0, 0, lean);
		//	}
		//	break;
		//case 'E':
		//	if (lean == 0) {
		//		lean = scale * 4;
		//		man.rotateCamera(0, 0, lean);
		//	}
		//	break;
		}
	}
	Object3D * obj = (Object3D *)man.objs.link;
	obj = cur_op;
	if (obj == NULL) {
		return;
	}
	switch (wParam)
	{
	case VK_CONTROL:
		is_control = 1;
		break;
	//case VK_LEFT:
	//	man.moveCamera(-1, 0, 0);
	//	break;
	//case VK_RIGHT:
	//	man.moveCamera(1, 0, 0);
	//	break;
	//case VK_UP:
	//	man.moveCamera(0, 1, 0);
	//	break;
	//case VK_DOWN:
	//	man.moveCamera(0, -1, 0);
	//	break;
	case VK_LEFT:
		if (cur_op) {
			//cur_op->setUV(cur_op->u + 1, cur_op->v);
			cur_op->move(1, 0, 0);
		}
		break;
	case VK_RIGHT:
		if (cur_op) {
			//cur_op->setUV(cur_op->u - 1, cur_op->v);
			cur_op->move(-1, 0, 0);
		}
		break;
	case VK_UP:
		if (cur_op) {
			//cur_op->setUV(cur_op->u, cur_op->v + 1);
			cur_op->move(0, 1, 0);
		}
		break;
	case VK_DOWN:
		if (cur_op) {
			//cur_op->setUV(cur_op->u, cur_op->v - 1);
			cur_op->move(0, -1, 0);
		}
		break;
	case 'Y':
		man.moveCamera(0, 0, 1);
		break;
	case 'N':
		man.moveCamera(0, 0, -1);
		break;
	case 'U':
		man.rotateCamera(0, 0, -1);
		break;
	case 'O':
		man.rotateCamera(0, 0, 1);
		break;
	case 'M':
		man.rotateCamera(1, 0, 0);
		break;
	case '/':
		man.rotateCamera(-1, 0, 0);
		break;
	case 'P':
		man.nextCamera();
		man.setCameraRange(org.x, org.y, scalex, scaley);
		break;
	case 'X':
		draw_line = -draw_line;
		break;
	case 'T':
		move_light = -move_light;
		break;
	case 'R':
		move_trans = -move_trans;
		break;
	case 'L':
		man.nextLight();
		break;
	case 'H':
		man.lgts.link->mode = -man.lgts.link->mode;
		break;
	case 'J':
		device.render_linear = -device.render_linear;
		break;
	case 'I':
		device.render_proj = -device.render_proj;
		break;
	case 'K':
		device.render_light = -device.render_light;
		break;
	case 'A':
		obj->move(1, 0, 0);
		break;
	case 'D':
		obj->move(-1, 0, 0);
		break;
	case 'S':
		obj->move(0, -1, 0);
		break;
	case 'W':
		obj->move(0, 1, 0);
		break;
	case 'G':
		obj->move(0, 0, -1);
		break;
	case 'F':
		obj->move(0, 0, 1);
		break;
	case 'Q':
		obj->rotate(1, 0, 0);
		break;
	case 'E':
		obj->rotate(-1, 0, 0);
		break;
	case 'Z':
		obj->rotate(0, 1, 0);
		break;
	case 'C':
		obj->rotate(0, -1, 0);
		break;
	case 'V':
		obj->rotate(0, 0, -1);
		break;
	case 'B':
		DEBUG_MODE = DEBUG_MODE >> 1;
		if (DEBUG_MODE == 0)
		{
			DEBUG_MODE = 0x0B;
		}
		break;
	}

	isrefresh = 1;
}