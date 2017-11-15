
#include "Drawable.h"


Manager3D man;
Device device;
TextureManage tman;

int DEBUG_MODE = DEBUG_GRADE_2;

INT isresize = -1;
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
	// Place draw code here
	setcolor(BLACK);
	cleardevice();
	//Render in device buffer
	if (move_light > 0) {
		device.RenderShade(man);
	}
	device.ClearBeforeRender();
	device.Render(man, NULL, NULL, NULL);
	device.RenderMirror(man, move_light);
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


VOID Initialize()
{
	man.addCamera(50, 50, 50, 1000, 90, 90).move(0, 0, -200);
	man.addCamera(30, 30, 60, 600, 30, 30).move(0, 0, -100);
	man.addShadowCamera(50, 50, 60, 1000, 120, 120);

	//man.addLight(9, -51, -60);
	man.addLight(5, 8, 220);
	//man.addLight(-1000, 100, 100);

	/*
	man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10)
		.scale(10, 10, 10).move(0, -30, -300).setColor(GREEN);
		*/

	//load resource
	INT t0 = tman.addTexture(480, 480);
	INT t1 = tman.addTexture(L"1.jpg");
	INT t2 = tman.addTexture(L"2.jpg");
	INT t3 = tman.addTexture(L"3.jpg");
	INT t4 = tman.addTexture(64, 64);


	man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(10, 10, 10).move(100, 0, -50).setColor(RED).setLineColor(BLUE).setTexture(tman, t4);

	man.addObject().addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.scale(10, 10, 10).move(0, -30, -300).setColor(GREEN).setTexture(tman, t1);

	man.addTransparentObject(1.01).addVert(-10, -10, 10).addVert(10, -10, 10).addVert(-10, 10, 10).addVert(10, 10, 10, -1)
		.addVert(10, 10, -10).addVert(10, -10, 10, -1).addVert(10, -10, -10).addVert(-10, -10, 10, -1).addVert(-10, -10, -10)
		.addVert(-10, 10, 10, -1).addVert(-10, 10, -10).addVert(10, 10, -10, -1).addVert(-10, -10, -10).addVert(10, -10, -10, -1)
		.scale(0.5, 0.5, 0.5).move(-15, 0, -50).setColor(RED).setLineColor(BLUE).setTexture(tman, t3);
	man.addObject().addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
		.scale(10, 10, 10).rotate(-90, -90, 180).move(-100, -20, 0).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t3);

	man.addReflectionObject(1000).addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
		.scale(10, 10, 10).rotate(90, 90, 0).move(100, -20, 0).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t1);

	int count = 2;
	int c = 30;
	int i, j, k;
	EFTYPE r = 10;
	EFTYPE x_1, x_2, r_1, r_2, p_1 = PI / ((EFTYPE)c), p_2 = 2 * PI / ((EFTYPE)c);

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
			obj.addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2, -1).setCenter(0, 0, 0).move(x, y, z).rotate(0, 0, 0).setColor(GREEN).setLineColor(RED);
		}
	}

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
			obj.addVert(x_1, 0, -r_1).addVert(x_2, 0, -r_2, -1).setCenter(0, 0, 0).move(x, y, z).rotate(0, 0, 0).setColor(GREEN).setLineColor(RED);
		}
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

	for (int i = 0; i < 15; i++) {
		for (int j = 0; j < 15; j++) {
			man.addObject().addVert(-10, 0, -10).addVert(10, 0, -10).addVert(-10, 0, 10).addVert(10, 0, 10, -1)
				.scale(2, 2, 2).rotate(0, 0, 180).move(300 - 40 * j, -40, 300 - 40 * i).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t4);
		}
	}

	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 15; j++) {
			man.addObject().addVert(-10, -10, 0).addVert(10, -10, 0).addVert(-10, 10, 0).addVert(10, 10, 0, -1)
				.scale(2, 2, 2).move(0, -60, -600).move(300 - 40 * j, 200 - 40 * i, 0).setColor(LIGHTGRAY).setLineColor(RED).setTexture(tman, t0);
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
				Obj3D * obj = man.tras.link;
				if (obj) {
					do {

						obj->move(0, 0, scale);


						obj = man.objs.next(obj);
					} while (obj && obj != man.tras.link);
				}
			}
			else {
				Obj3D * obj = man.tras.link;
				if (obj) {
					do {

						obj->move(0, 0, -scale);


						obj = man.objs.next(obj);
					} while (obj && obj != man.tras.link);
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
	}
}

PointF menu;
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
		}
	}
	else	// mouse up
	{
		menu.X = 0;
		menu.Y = 0;
	}
}

PointF drag;
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
		case 'W':
			wParam = 'Y';
			break;
		case 'S':
			wParam = 'N';
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
	if (obj == NULL) {
		return;
	}
	switch (wParam)
	{
	case VK_CONTROL:
		is_control = 1;
		break;
	case VK_LEFT:
		man.moveCamera(-1, 0, 0);
		break;
	case VK_RIGHT:
		man.moveCamera(1, 0, 0);
		break;
	case VK_UP:
		man.moveCamera(0, 1, 0);
		break;
	case VK_DOWN:
		man.moveCamera(0, -1, 0);
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
	case ',':
		man.rotateCamera(-1, 0, 0);
		break;
	case 'P':
		man.nextCamera();
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
}