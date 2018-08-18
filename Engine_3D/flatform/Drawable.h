#pragma once

#include "../flatform/EPaint.h"

#define DEBUG_GRADE_1	0x01
#define DEBUG_GRADE_2	0x02
#define DEBUG_GRADE_3	0x03
#define DEBUG_GRADE_4	0x04

VOID Initialize();
VOID onPaint(HWND hWnd);
VOID onTimer();
VOID onClose();
VOID onKeyDown(WPARAM wParam);
VOID onKeyUp(WPARAM wParam);
VOID onResize(FLOAT width, FLOAT height);
VOID onScroll(FLOAT delta);
VOID onMenu(FLOAT x, FLOAT y, INT mode);
VOID onDrag(FLOAT x, FLOAT y, INT mode);
INT isInputBlocked();
INT isRenderRaytracing();

#include "Device.h"
