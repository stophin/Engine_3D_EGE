// Engine_3D.cpp : 定义控制台应用程序的入口点。
//

#include "./flatform/Drawable.h"

int main(int argc, char* argv[])
{
	EP_Init(800, 600);
	//EP_Init(-1, -1);

	Initialize();

	INT count = 0;
	char str[100];
	EP_RenderStart();
	do
	{
		onPaint(EP_GetWnd());

		if (!isInputBlocked()) {
			while (EP_MouseHit())
			{
				EP_MouseMsg msg = EP_GetMouseMsg();
				if (EP_IsWheel(msg)) {
					onScroll(msg.wheel);
				}
				if (EP_IsRight(msg)) {
					if (EP_IsDown(msg))
					{
						onMenu(msg.x, msg.y, 1);
					}
					else if (EP_IsUp(msg))
					{
						onMenu(msg.x, msg.y, 0);
					}
				}
				else if (EP_IsLeft(msg)) {
					if (EP_IsDown(msg))
					{
						onDrag(msg.x, msg.y, 1);
					}
					else if (EP_IsUp(msg))
					{
						onDrag(msg.x, msg.y, 0);
					}
				}
				if (EP_IsMove(msg))
				{
					onMenu(msg.x, msg.y, 2);
					onDrag(msg.x, msg.y, 2);
				}
			}
			if (EP_KBMsg()) {
				EP_MSG msg = EP_GetKBMsg();
				if (EP_KBIsUp(msg)) {
					onKeyUp(EP_GetKey(msg));
				}
				if (EP_KBIsDown(msg)) {
					onKeyDown(EP_GetKey(msg));
				}
				if (EP_Equal(msg, VK_ESCAPE)) {
					break;
				}
			}
		}
		else {
			EP_Delay(1000);
		}
		EP_FlushKey();
		EP_FlushMouse();

		if (count++ > 2) {
			count = 0;
			onTimer();
		}
		//delay_ms(100);
		EP_RenderFlush();
		if (isRenderRaytracing()) {
			sprintf_s(str, "Raytracing %f", getfps());
		}
		else {
			sprintf_s(str, "%f", getfps());
		}
		::SetWindowText(EP_GetWnd(), str);
	} while (1);
	onClose();
	closegraph();
}

